#include "pch.h"
#include "CCanJuice.h"

#include <random>

#include "CUnitScript.h"

CCanJuice::CCanJuice()
    : CScript(CANJUICE)
    , m_Acc(0.f)
    , m_Duration(5.f)
    , m_InitSpeed(18.f)
    , m_Damage(10.f)
{
}

CCanJuice::~CCanJuice()
{
}

void CCanJuice::begin()
{
    m_Acc = 0.f;

    // �ڽ� ������Ʈ ��������
    CGameObject* JuiceMesh = GetOwner()->GetChildObject()[0];

    // ���� ����
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 4);

    // Type�� �����ѵ� ���� ���͸��� ����
    JuiceType Type = (JuiceType)dis(gen);
    JuiceMesh->MeshRender()->GetDynamicMaterial(0);

    // Texture ���ε�
    switch (Type)
    {
    case JuiceType::Green:
        JuiceMesh->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Green.png",
                                                        L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Green.png"));
        break;
    case JuiceType::Yellow:
        JuiceMesh->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Yellow.png",
                                                        L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Yellow.png"));
        break;
    case JuiceType::Red:
        JuiceMesh->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Red.png",
                                                        L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Red.png"));
        break;
    case JuiceType::Purple:
        JuiceMesh->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Purple.png",
                                                        L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Purple.png"));
        break;
    case JuiceType::Orange:
        JuiceMesh->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Orange.png",
                                                        L"fbx\\Characters\\Kirby\\OriginObj\\DeformVendingMachine\\CanJuice\\CanJuice_Orange.png"));
        break;
    }

    // ���� ����
    Vec3 Front = GetOwner()->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    Front.y = 0.f;
    Front.Normalize();

    // �¿�� �ణ�� ����
    float diff = float(int(Type) - 2) / 2.f;
    Vec3 Right = GetOwner()->Transform()->GetWorldDir(DIR_TYPE::RIGHT);
    Right.Normalize();
    Front += Right * diff * sinf(10.f / XM_PI);
    Front.Normalize();

    // ���ư��� �ϴ� ���� ����
    Vec3 FlyDir = Front + Vec3(0.f, 0.2f, 0.f);
    FlyDir.Normalize();

    Rigidbody()->AddForce(FlyDir * m_InitSpeed, ForceMode::Impulse);
}

void CCanJuice::tick()
{
    m_Acc += DT;

    // �ð��� ������ ����
    if (m_Acc > m_Duration)
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
    }
}

void CCanJuice::OnTriggerEnter(CCollider* _OtherCollider)
{
    int LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    if (LayerIdx == LAYER_MONSTER_TRIGGER && _OtherCollider->GetOwner()->GetName() == L"Body Collider")
    {
        Vec3 CanPos = GetOwner()->Transform()->GetWorldPos();
        Vec3 MonsterPos = _OtherCollider->GetOwner()->Transform()->GetWorldPos();
        Vec3 HitDir = MonsterPos - CanPos;
        HitDir.Normalize();

        UnitHit JuiceHit = {DAMAGE_TYPE::NORMAL, HitDir, m_Damage, 0.f, 0.f};

        CGameObject* pParent = _OtherCollider->GetOwner()->GetParent();
        if (nullptr != pParent)
            pParent->GetScript<CUnitScript>()->GetDamage(JuiceHit);

        Ptr<CPrefab> CanAttackSmokeSpawner = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\(M)ImpactSmokeSpanwer.pref");
        CGameObject* Spawner = CanAttackSmokeSpawner->Instantiate();
        Spawner->Transform()->SetWorldPos(MonsterPos);
        GamePlayStatic::SpawnGameObject(Spawner, Spawner->GetLayerIdx());

        GamePlayStatic::DestroyGameObject(GetOwner());
    }
}

UINT CCanJuice::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CCanJuice::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}
