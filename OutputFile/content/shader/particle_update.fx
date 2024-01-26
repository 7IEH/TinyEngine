#ifndef _PARTICLE_UPDATE
#define _PARTICLE_UPDATE

#include "global.hlsli"
#include "struct.hlsli"

RWStructuredBuffer<tParticle> g_ParticleBuffer : register(u0);
RWStructuredBuffer<int4> g_SpawnCount : register(u1);
StructuredBuffer<tParticleModule> g_Module : register(t20);

#define MAX_COUNT   g_int_0 
#define SpawnCount  g_SpawnCount[0].x
#define Particle    g_ParticleBuffer[id.x]

[numthreads(1024, 1, 1)]
void CS_ParticleUpdate(uint3 id : SV_DispatchThreadID)
{
    // ��ƼŬ�� ��Ȱ��ȭ ���¶��
    if (0 == Particle.Active)
    {
        if (0 < SpawnCount)
        {
            // Atomic �Լ� 
            int AliveCount = SpawnCount;
            int Exchange = SpawnCount - 1;
            int Origin = 0;
                        
            //InterlockedCompareExchange(SpawnCount, AliveCount, Exchange, Origin);
            //if (AliveCount == Origin)
            //{
            //    Particle.Active = 1;
            //}
            
            InterlockedExchange(SpawnCount, Exchange, Origin);
        
            if (SpawnCount < Origin)
            {
                Particle.Active = 1;
            }
        }
    }
    
    // ��ƼŬ�� Ȱ��ȭ ���¶��
    else
    {
        Particle.vWorldPos.y += g_dt * 100.f;
    }
}

#endif