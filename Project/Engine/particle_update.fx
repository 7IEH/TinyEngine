#ifndef _PARTICLE_UPDATE
#define _PARTICLE_UPDATE

#include "global.hlsli"
#include "struct.hlsli"
#include "func.hlsli"

StructuredBuffer<tParticleModule> g_Module : register(t20);
RWStructuredBuffer<tParticle> g_ParticleBuffer : register(u0);
RWStructuredBuffer<int4> g_SpawnCount : register(u1);

#define MAX_COUNT   g_int_0 
#define SpawnCount  g_SpawnCount[0].x
#define Particle    g_ParticleBuffer[id.x]
#define Module      g_Module[0]
#define CenterPos   g_vec4_0.xyz

#define SPAWN_MODULE 0
#define DRAG_MODULE 1
#define SCALE_MODULE 2
#define ADD_VELOCITY_MODULE 3
#define SPIRAL_VELOCITY_MODULE 4
#define NOISE_FORCE_MODULE 5
#define CALCULATE_FORCE_MODULE 6
#define RENDER_MODULE 7

[numthreads(32, 1, 1)]
void CS_ParticleUpdate(int3 id : SV_DispatchThreadID)
{
    if (MAX_COUNT <= id.x)
        return;
    
    // ��ƼŬ�� ��Ȱ��ȭ ���¶��
    if (0 == Particle.Active)
    {
        // ���� ��� Ȱ��ȭ üũ
        if (0 == Module.arrModuleCheck[SPAWN_MODULE])
            return;
        
        while (0 < SpawnCount)
        {
            // Atomic �Լ� 
            int originValue = SpawnCount;
            int outputValue = 0;
           
            // ������ ���� ��ġ�� ��쿡�� ��ü
            InterlockedCompareExchange(SpawnCount, originValue, SpawnCount - 1, outputValue);
            
            if (originValue == outputValue)
            {
                Particle.Active = 1;
                Particle.vNoiseForce = float4(0.f, 0.f, 0.f, 1.f);
                Particle.NoiseForceTime = 0.f;
                
                // ����
                float2 vUV = float2((1.f / (MAX_COUNT - 1)) * id.x, 0.f);
                
                vUV.x += g_Time * 0.2f;
                //                 ( ���ļ� )    (����)  (V �� offset)
                vUV.y = sin(vUV.x * 20.f * PI) * 0.2f + g_Time * 0.1f;
                                
                float4 vRand0 = g_NoiseTex.SampleLevel(g_PointSampler, vUV, 0);
                float4 vRand1 = g_NoiseTex.SampleLevel(g_PointSampler, vUV - float2(0.1f, 0.1f), 0);
                float4 vRand2 = g_NoiseTex.SampleLevel(g_PointSampler, vUV - float2(0.2f, 0.2f), 0);
                
                // SpawnShape - Sphere 
                if (0 == Module.SpawnShape)
                {
                    float RandomRadius = vRand0.r * Module.Radius;
                    float RandomAngleX = vRand0.g * 2 * PI;
                    
                    float randomX = vRand0.r * 2.f - 1.f;
                    float randomY = vRand0.g * 2.f - 1.f;
                    float randomZ = vRand0.b * 2.f - 1.f;
                   
                    // Particle ������Ʈ(��ü) �� �߽���ġ(����) ����
                    // ���� ����, ���� �������� �ش��ϴ� ��ġ�� ����ؼ� ��ƼŬ�� �ʱ� ��ġ�� �ش�.
                    //Particle.vLocalPos.xyz = float3(cos(RandomAngle), sin(RandomAngle), 0.f) * RandomRadius;
                    Particle.vLocalPos.xyz = float3(randomX, randomY, randomZ) * RandomRadius;
                }
                // SpawnShape - Box
                else if (1 == Module.SpawnShape)
                {
                    Particle.vLocalPos.x = vRand0.r * Module.vSpawnBoxScale.x - (Module.vSpawnBoxScale.x / 2.f);
                    Particle.vLocalPos.y = vRand0.g * Module.vSpawnBoxScale.y - (Module.vSpawnBoxScale.y / 2.f);
                    Particle.vLocalPos.z = vRand0.b * Module.vSpawnBoxScale.z - (Module.vSpawnBoxScale.z / 2.f);
                }
                
                Particle.vWorldPos.xyz = Particle.vLocalPos.xyz + CenterPos;
                
                // ���� �÷� ����
                Particle.vColor = Module.vSpawnColor;
                
                // ���� ũ�� ����                
                Particle.vWorldScale = Particle.vWorldInitScale = (Module.vSpawnMaxScale - Module.vSpawnMinScale) * vRand1.r + Module.vSpawnMinScale;
                
                // ���� Life ����
                Particle.Age = 0.f;
                Particle.Life = (Module.MaxLife - Module.MinLife) * vRand1.g + Module.MaxLife;
                                   
                // ���� Mass ����
                Particle.Mass = (Module.MaxMass - Module.MinMass) * vRand1.b + Module.MinMass;
                              
                Particle.Radius = 0.f;
                
                // Add VelocityModule
                if (Module.arrModuleCheck[ADD_VELOCITY_MODULE])
                {
                    if (0 == Module.AddVelocityType)  // 0 : From Center
                    {
                        float3 vDir = normalize(Particle.vLocalPos.xyz);
                        
                        Particle.vVelocity.xyz = vDir * ((Module.MaxSpeed - Module.MinSpeed) * vRand2.r + Module.MinSpeed);
                    }
                    else if (1 == Module.AddVelocityType)  // 1 : To Center
                    {
                        float3 vDir = -normalize(Particle.vLocalPos.xyz);
                        Particle.vVelocity.xyz = vDir * ((Module.MaxSpeed - Module.MinSpeed) * vRand2.g + Module.MinSpeed);
                    }
                    else if (2 == Module.AddVelocityType)  // 2 : Fixed Angle
                    {
                        float randomRatio = Module.FixedAngle / 180.f;
                        
                        // ������ ���� ���� -1 ~ 1 �� �������� ���ؼ� �������� ���� �Ѵ�.
                        float randomX = (vRand2.r * 2.f - 1.f) * randomRatio;
                        float randomY = (vRand2.g * 2.f - 1.f) * randomRatio;
                        float randomZ = (vRand2.b * 2.f - 1.f) * randomRatio;
                        
                        // ���� ���� + ���� ���� �������� �������� ����
                        Particle.vVelocity.xyz = normalize(Module.vFixedDirection.xyz + float3(randomX, randomY, randomZ)) * ((Module.MaxSpeed - Module.MinSpeed) * vRand0.r + Module.MinSpeed);
                    }
                }
                else
                {
                    Particle.vVelocity.xyz = float3(0.f, 0.f, 0.f);
                }
                
                break;
            }
        }
    }
    
    // ��ƼŬ�� Ȱ��ȭ ���¶��
    else
    {
        Particle.Age += g_DT;
        if (Particle.Life < Particle.Age)
        {
            Particle.Active = 0;
            return;
        }
        
        // Add VelocityModule - To Center Ÿ�� �ϰ��
        if (Module.arrModuleCheck[ADD_VELOCITY_MODULE] && 1 == Module.AddVelocityType)
        {
            float len = length(CenterPos - Particle.vWorldPos.xyz);
            if (len < 10)
            {
                Particle.Active = 0;
                return;
            }
        }
            
        // ������ ����
        float fNormalizeThreadID = (float) id.x / (float) MAX_COUNT;
        float3 Rand = float3(0.f, 0.f, 0.f);
        GaussianSample(g_NoiseTex, g_NoiseTexResolution, fNormalizeThreadID, Rand);

        Particle.vForce.xyz = float3(0.f, 0.f, 0.f);
        
        // Normalize Age ���
        Particle.NormalizeAge = Particle.Age / Particle.Life;
        
                  
        // Spiral Velocity
        if (Module.arrModuleCheck[SPIRAL_VELOCITY_MODULE])
        {
            // X
            if (0 == Module.SpiralVelocityAxis)
            {

            }
            // Y
            else if (1 == Module.SpiralVelocityAxis)
            {
                    
            }
            // Z
            else if (2 == Module.SpiralVelocityAxis)
            {
                Particle.Radius += g_DT * Module.SpiralVelocityRadiusSpeed;
                
                // �ڻ��ΰ� ���� �Լ��� ���� ��θ� ���� �̵�
                Particle.vWorldPos.x += cos(Particle.vWorldPos.y * Module.SpiralVelocityRotateSpeed + g_DT) * Particle.Radius;
                Particle.vWorldPos.z += sin(Particle.vWorldPos.y * Module.SpiralVelocityRotateSpeed + g_DT) * Particle.Radius;
                
                Particle.vWorldPos.y += g_DT * Module.SpiralVelocityAxisSpeed;
            }
            // -X
            else if (3 == Module.SpiralVelocityAxis)
            {
                    
            }
            // -Y
            else if (4 == Module.SpiralVelocityAxis)
            {
                    
            }
            // -Z
            else if (5 == Module.SpiralVelocityAxis)
            {
                    
            }
        }
        else
        {
            // Scale ���
            if (Module.arrModuleCheck[SCALE_MODULE])
            {
                Particle.vWorldScale = Particle.vWorldInitScale * (1.f + (Module.vScaleRatio - 1.f) * Particle.NormalizeAge);
            }
        
            // Noise Force
            if (Module.arrModuleCheck[NOISE_FORCE_MODULE])
            {
                if (Particle.NoiseForceTime == 0.f) // �ʱ� Force
                {
                    Particle.vNoiseForce.xyz = normalize(Rand.xyz * 2.f - 1.f) * Module.NoiseForceScale;
                    Particle.NoiseForceTime = g_Time;
                }
                else if (Module.NoiseForceTerm < g_Time - Particle.NoiseForceTime) // Term ���� Force ������Ʈ
                {
                    Particle.vNoiseForce.xyz = normalize(Rand.xyz * 2.f - 1.f) * Module.NoiseForceScale;
                    Particle.NoiseForceTime = g_Time;
                }
            }
                
            // Calculate Force
            if (Module.arrModuleCheck[CALCULATE_FORCE_MODULE])
            {
                Particle.vForce.xyz += Particle.vNoiseForce.xyz;
            
                // Force ����
                // F = M x A
                float3 vAccel = Particle.vForce.xyz / Particle.Mass;
                  
                // Accel ����
                Particle.vVelocity.xyz += vAccel * g_DT;
            
                // Drag ���
                if (Module.arrModuleCheck[DRAG_MODULE])
                {
                    float4 DragForce = float4(0.f, 0.f, 0.f, 0.f);
                    float LimitTime = Module.DragTime - Particle.Age;
            
                    if (LimitTime <= 0.f)
                    {
                        DragForce = float4(0.f, 0.f, 0.f, 0.f);
                    }
                    else
                    {
                        float DT = g_DT / LimitTime;
                        DragForce = Particle.vVelocity * DT;
                    }

                    if (length(Particle.vVelocity) > length(DragForce))
                    {
                    Particle.vVelocity -= DragForce;
                    }
                }
              
                // Velocity ����
                if (0 == Module.SpaceType) // Local
                {
                  Particle.vLocalPos.xyz += Particle.vVelocity.xyz * g_DT;
                  Particle.vWorldPos.xyz = Particle.vLocalPos.xyz + CenterPos;
                }
                else if (1 == Module.SpaceType) // World
                {
                    Particle.vWorldPos.xyz += Particle.vVelocity.xyz * g_DT;
                }
            }
        }
    }
}

#endif
