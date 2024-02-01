#include "pch.h"
#include "CParticleSystem.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

#include "CTimeMgr.h"

#include "CAssetMgr.h"
#include "CMesh.h"
#include "CMaterial.h"

#include "components.h"

CParticleSystem::CParticleSystem()
    : CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
    , m_ParticleBuffer(nullptr)
    , m_ModuleBuffer(nullptr)
    , m_RWBuffer(nullptr)
    , m_MaxParticleCount(1000)
    , m_Module{}
    , m_CSParticleUpdate()
    , m_ParticleTex()
    , m_AccTime(0)
{
    // ���� �޽��� ���� ���� ���
    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ParticleRenderMtrl"));

    // ��ƼŬ�� �����ϴ� ����ȭ ����
    m_ParticleBuffer = new CStructuredBuffer;
    m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::READ_WRITE, true);

    // ��ƼŬ ��������� �����ϴ� ����ȭ����
    m_ModuleBuffer = new CStructuredBuffer;
    m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true);

    // ������ �б�/���� �� ����ȭ����
    m_RWBuffer = new CStructuredBuffer;
    m_RWBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::READ_WRITE, true);

    // ��ƼŬ ������Ʈ�� ��ǻƮ ���̴� ����
    m_CSParticleUpdate =
        (CParticleUpdate*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"ParticleUpdateShader").Get();

    // �ʱ� ��� ����
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::SPAWN] = 1;

    m_Module.SpaceType = 1;
    m_Module.vSpawnColor = Vec4(0.2f, 0.4f, 0.9f, 1.f);
    m_Module.vSpawnMinScale = Vec4(100.f, 30.f, 1.f, 1.f);
    m_Module.vSpawnMaxScale = Vec4(100.f, 30.f, 1.f, 1.f);
    m_Module.MinLife = 5.f;
    m_Module.MaxLife = 5.f;
    m_Module.MinMass = 1.f;
    m_Module.MaxMass = 1.f;
    m_Module.SpawnShape = 1; // 0 : Sphere, 1 : Box
    m_Module.Radius = 100.f;
    m_Module.vSpawnBoxScale = Vec4(500.f, 500.f, 0.f, 0.f);
    m_Module.SpawnRate = 50;

    // Add Velocity Module
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = 0;
    m_Module.AddVelocityType = 0;
    m_Module.MinSpeed = 100;
    m_Module.MaxSpeed = 150;
    m_Module.vFixedDirection;
    m_Module.FixedAngle;

    // Scale
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::SCALE] = 0;
    m_Module.vScaleRatio = Vec3(2.f, 2.f, 2.f);

    // Noise Force
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = 1;
    m_Module.NoiseForceScale = 50.f;
    m_Module.NoiseForceTerm = 0.3f;

    // Render
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = 1;
    m_Module.VelocityAlignment = 1; // �ӵ��� ���� ���� ����

    // Calculate Force
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::CALCULATE_FORCE] = 1;

    
	// �ʱ� ��� ����
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::SPAWN] = 1;

    m_Module.SpaceType = 1;
    m_Module.vSpawnColor = Vec4(0.2f, 0.4f, 0.9f, 1.f);
    m_Module.vSpawnMinScale = Vec4(100.f, 30.f, 1.f, 1.f);
    m_Module.vSpawnMaxScale = Vec4(100.f, 30.f, 1.f, 1.f);
    m_Module.MinLife = 3.f;
    m_Module.MaxLife = 5.f;
    m_Module.MinMass = 1.f;
    m_Module.MaxMass = 1.f;
    m_Module.SpawnShape = 1; // 0 : Sphere, 1 : Box
    m_Module.Radius = 100.f;
    m_Module.vSpawnBoxScale = Vec4(500.f, 500.f, 0.f, 0.f);
    m_Module.SpawnRate = 50;

    // Add Velocity Module
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = 0;
    m_Module.AddVelocityType = 0;
    m_Module.MinSpeed = 100;
    m_Module.MaxSpeed = 150;
    m_Module.vFixedDirection;
    m_Module.FixedAngle;

    // Scale
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::SCALE] = 0;
    m_Module.vScaleRatio = Vec3(0.1f, 0.1f, 0.1f);

    // Noise Force
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = 0;
    m_Module.NoiseForceScale = 50.f;
    m_Module.NoiseForceTerm = 0.3f;

    // Calculate Forec
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::CALCULATE_FORCE] = 1;

    // Render
    m_Module.arrModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = 1;
    m_Module.VelocityAlignment = 1; // �ӵ��� ���� ���� ����


    m_ParticleTex = CAssetMgr::GetInst()->Load<CTexture>(L"Textures\\particle\\Bubbles50px.png",
                                                         L"Textures\\particle\\Bubbles50px.png");
}

CParticleSystem::~CParticleSystem()
{
    if (nullptr != m_ParticleBuffer)
        delete m_ParticleBuffer;

    if (nullptr != m_ModuleBuffer)
        delete m_ModuleBuffer;

    if (nullptr != m_RWBuffer)
        delete m_RWBuffer;
}

void CParticleSystem::finaltick()
{
    m_AccTime += DT;

    if ((1.f / m_Module.SpawnRate) < m_AccTime)
    {
        // ���� �ð��� ���� �������� ���� ��
        float fSpawnCount = m_AccTime / (1.f / m_Module.SpawnRate);

        // ���� ������ ������ �ܷ��� ���� �����ð����� ����
        m_AccTime -= (1.f / m_Module.SpawnRate) * floorf(fSpawnCount);

        tSpawnCount count = {};
        count.SpawnCount = (int)fSpawnCount; 
        m_RWBuffer->SetData(&count);
    }
    else
    {
        tSpawnCount count = tSpawnCount{
            0,
        };
        m_RWBuffer->SetData(&count);
    }

    // ��ƼŬ ������� ������Ʈ
    m_ModuleBuffer->SetData(&m_Module);
    m_ModuleBuffer->UpdateData_CS_SRV(20);
    m_ModuleBuffer->UpdateData(21);

    // ��ƼŬ ������Ʈ ��ǻƮ ���̴�
    m_CSParticleUpdate->SetParticleBuffer(m_ParticleBuffer);
    m_CSParticleUpdate->SetModuleBuffer(m_ModuleBuffer);
    m_CSParticleUpdate->SetRWBuffer(m_RWBuffer);
    m_CSParticleUpdate->SetParticleWorldPos(Transform()->GetWorldPos());

    m_CSParticleUpdate->Execute();
}

void CParticleSystem::UpdateData()
{
    // View, Proj ��� ����
    Transform()->UpdateData();

    // ParticleBuffer ���ε�
    m_ParticleBuffer->UpdateData(20);

    // ��� ��ƼŬ ������
    // ��ƼŬ ���� ������ -> �ν��Ͻ�
    GetMaterial()->UpdateData();
    GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
}

void CParticleSystem::render()
{
    UpdateData();

    GetMesh()->render_particle(m_MaxParticleCount);

    // �������� ����� ���ҽ� ���ε� Clear
    m_ParticleBuffer->Clear(20);
    m_ModuleBuffer->Clear(21);
}

void CParticleSystem::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);

    fwrite(&m_MaxParticleCount, sizeof(UINT), 1, _File);
    fwrite(&m_Module, sizeof(tParticleModule), 1, _File);

    SaveAssetRef(m_CSParticleUpdate.Get(), _File);
    SaveAssetRef(m_ParticleTex.Get(), _File);
}

void CParticleSystem::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);

    fread(&m_MaxParticleCount, sizeof(UINT), 1, _File);
    m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::READ_WRITE, true);
    fread(&m_Module, sizeof(tParticleModule), 1, _File);

    int i = 0;
    fread(&i, sizeof(i), 1, _File);

    if (i)
    {
        wstring strKey, strRelativePath;
        LoadWString(strKey, _File);
        LoadWString(strRelativePath, _File);

        m_CSParticleUpdate = (CParticleUpdate*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(strKey).Get();
    }

    LoadAssetRef<CTexture>(m_ParticleTex, _File);
}
