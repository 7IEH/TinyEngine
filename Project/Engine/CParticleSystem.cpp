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
    , m_MaxParticleCount(1000)
    , m_Module{}
    , m_ModuleBuffer(nullptr)
    , m_RWBuffer(nullptr)
    , m_CSParticleUpdate()
    , m_AccTime(0)
{
    // ���� �޽��� ���� ���� ���
    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ParticleMtrl"));

    // ������ �ػ�
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();

    // �ӽ÷� 5���� ��ƼŬ�� �ʱ� �����͸� �Է��ϸ鼭 ����ȭ ���� ����
    tParticle arrParticle[1000] = {};
    for (int i = 0; i < m_MaxParticleCount; ++i)
    {
        arrParticle[i].vWorldPos = Vec3((vResol.x / -2.f) + (i + 1) * vResol.x / (m_MaxParticleCount + 1), 0.f, 200.f);
        arrParticle[i].vWorldScale = Vec3(10.f, 10.f, 1.f);
        arrParticle[i].Active = 0;
    }

    // ��ƼŬ�� �����ϴ� ����ȭ ����
    m_ParticleBuffer = new CStructuredBuffer;
    m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::READ_WRITE, true, arrParticle);

    // ��ƼŬ ��������� �����ϴ� ����ȭ����
    m_ModuleBuffer = new CStructuredBuffer;
    m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true);

    // ��ƼŬ ������Ʈ�� ��ǻƮ ���̴� ����
    m_CSParticleUpdate =
        (CParticleUpdate*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"ParticleUpdateShader").Get();

    // ������ �б�/���� �� ����ȭ����
    m_RWBuffer = new CStructuredBuffer;
    m_RWBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::READ_WRITE, true);
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
    // ��ƼŬ ��Ⱚ ����
    m_Module.SpaceType = 1;
    m_Module.vSpawnColor = Vec4(1.f, 0.f, 0.f, 1.f);
    m_Module.vSpawnMinScale = Vec4(20.f, 20.f, 1.f, 1.f);
    m_Module.vSpawnMaxScale = Vec4(20.f, 20.f, 1.f, 1.f);
    m_Module.MinLife = 5.f;
    m_Module.MaxLife = 5.f;
    m_Module.SpawnRate = 100;

    m_AccTime += DT;
    if ((1.f / m_Module.SpawnRate) < m_AccTime)
    {
        m_AccTime = 0.f;
        tSpawnCount count = tSpawnCount{
            100,
        };
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

    // ��ƼŬ ������Ʈ ��ǻƮ ���̴�
    m_CSParticleUpdate->SetParticleBuffer(m_ParticleBuffer);
    m_CSParticleUpdate->SetModuleBuffer(m_ModuleBuffer);
    m_CSParticleUpdate->SetRWBuffer(m_RWBuffer);

    m_CSParticleUpdate->Execute();
}

void CParticleSystem::render()
{
    // View, Proj ��� ����
    Transform()->UpdateData();

    // ParticleBuffer ���ε�
    m_ParticleBuffer->UpdateData(20);

    // ��� ��ƼŬ ������
    // ��ƼŬ ���� ������ -> �ν��Ͻ�
    GetMaterial()->UpdateData();
    GetMesh()->render_particle(m_MaxParticleCount);

    // �������� ����� ���ҽ� ���ε� Clear
    m_ParticleBuffer->Clear(20);
}

void CParticleSystem::UpdateData()
{
}