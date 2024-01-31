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
    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
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

        tSpawnCount count = tSpawnCount{(int)fSpawnCount, 0, 0, 0};
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
