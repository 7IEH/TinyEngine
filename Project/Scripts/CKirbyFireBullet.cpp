#include "pch.h"
#include "CKirbyFireBullet.h"

#include "CMonsterUnitScript.h"

CKirbyFireBullet::CKirbyFireBullet()
    : CScript(KIRBYFIREBULLET)
    , m_Step((UINT)Fire_Step::LV0)
    , m_AccFireNumber(0)
    , m_RequiredFireNumber(10)
    , m_Acc(0.f)
    , m_Duration(4.f)
    , m_bFirstTime(false)
{
    AddScriptParam(SCRIPT_PARAM::INT, &m_Step, "Fire Level(0~2)");
    AddScriptParam(SCRIPT_PARAM::INT, &m_RequiredFireNumber, "Required Fire Number");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Duration, "Duration");
}

CKirbyFireBullet::CKirbyFireBullet(const CKirbyFireBullet& _Origin)
    : CScript(KIRBYFIREBULLET)
    , m_Step(_Origin.m_Step)
    , m_AccFireNumber(_Origin.m_AccFireNumber)
    , m_RequiredFireNumber(_Origin.m_RequiredFireNumber)
    , m_Acc(_Origin.m_Acc)
    , m_Duration(_Origin.m_Duration)
    , m_bFirstTime(false)
{
    AddScriptParam(SCRIPT_PARAM::INT, &m_Step, "Fire Level(0~2)");
    AddScriptParam(SCRIPT_PARAM::INT, &m_RequiredFireNumber, "Required Fire Number");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Duration, "Duration");
}

CKirbyFireBullet::~CKirbyFireBullet()
{
}

void CKirbyFireBullet::begin()
{
    m_Acc = 0.f;
    m_AccFireNumber = 0;
    m_DestroyDuration = 0.1f;
    m_bDestroy = false;

    // Rigidbody�� ���ٸ� �߰��Ѵ�.
    if (Rigidbody() == nullptr)
    {
        GetOwner()->AddComponent(new CRigidbody);
        Rigidbody()->SetUseGravity(true);
    }
    // Kirby���Լ� ���°��̶�� ���󰡰� �Ѵ�.
    if (m_bFirstTime)
    {
        m_Dir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Rigidbody()->AddForce(m_Dir * 15.f, ForceMode::Impulse);
        m_bFirstTime = false;
    }
}

void CKirbyFireBullet::tick()
{
    m_Acc += DT;

    if (m_bIsFreeze)
    {
        Rigidbody()->SetFreezePosition(AXIS_TYPE::X, true);
        Rigidbody()->SetFreezePosition(AXIS_TYPE::Y, true);
        Rigidbody()->SetFreezePosition(AXIS_TYPE::Z, true);
        m_bIsFreeze = false;
    }

    if (m_bDestroy == false && m_Acc > m_Duration)
    {
        StepDown();
    }

    if (m_bDestroy && m_Acc > m_DestroyDuration)
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
    }
}

void CKirbyFireBullet::StepUp()
{
    Ptr<CPrefab> NextFirePref = nullptr;

    switch (m_Step)
    {
    case (UINT)Fire_Step::LV0:
    {
        NextFirePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV1.pref");
    }
        break;

    case (UINT)Fire_Step::LV1:
    {
        NextFirePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV2.pref");
    }
        break;

    case (UINT)Fire_Step::LV2:
    {
        return;
    }
        break;

    default:
        break;
    }

    if (NextFirePref.Get() != nullptr)
    {
        Vec3 CurPos = Transform()->GetWorldPos();

        // ���� �ܰ��� Fire ������Ʈ�� �����Ͽ� ���� �ڸ��� �־��ش�.
        CGameObject* NextFireObj = NextFirePref->Instantiate();
        NextFireObj->Transform()->SetWorldPos(CurPos);

        // ���� ������Ʈ�� ����
        GamePlayStatic::SpawnGameObject(NextFireObj, NextFireObj->GetLayerIdx());

        // ������ ������Ʈ�� �����Ѵ�
        //GamePlayStatic::DestroyGameObject(GetOwner());

        GetOwner()->SphereCollider()->SetEnabled(false);
    }
}

void CKirbyFireBullet::StepDown()
{
    Ptr<CPrefab> NextFirePref = nullptr;

    switch (m_Step)
    {
    case (UINT)Fire_Step::LV0: {
    }
    break;

    case (UINT)Fire_Step::LV1: {
        NextFirePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV0.pref");
    }
    break;

    case (UINT)Fire_Step::LV2: {
        NextFirePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\FireBulletLV1.pref");
    }
    break;

    default:
        break;
    }

    if (NextFirePref.Get() != nullptr)
    {
        Vec3 CurPos = Transform()->GetWorldPos();

        // ���� �ܰ��� Fire ������Ʈ�� �����Ͽ� ���� �ڸ��� �־��ش�.
        CGameObject* NextFireObj = NextFirePref->Instantiate();
        NextFireObj->Transform()->SetWorldPos(CurPos);

        // ���� ������Ʈ�� ����
        GamePlayStatic::SpawnGameObject(NextFireObj, NextFireObj->GetLayerIdx());

        // ������ ������Ʈ�� �����Ѵ�
        // GamePlayStatic::DestroyGameObject(GetOwner());
    }
    GetOwner()->SphereCollider()->SetEnabled(false);

    m_Acc = 0.f;
    m_bDestroy = true;
}

void CKirbyFireBullet::OnCollisionEnter(CCollider* _OtherCollider)
{
    int LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    // World Static�̶� �浹�� ���
    if (LayerIdx == LAYER_STATIC)
    {
        m_bIsFreeze = true;

        // @TODO Fire Decal �����
    }

    // FireBulluet(�ڱ� �ڽŰ� �浹�� ���
    if (LayerIdx == LAYER_FIREBULLET)
    {
        CKirbyFireBullet* OtherFireScript = _OtherCollider->GetOwner()->GetScript<CKirbyFireBullet>();

        // ������ FireScript�� ���� ��� �����Ѵ�.
        if (OtherFireScript == nullptr)
        {
            GamePlayStatic::DestroyGameObject(_OtherCollider->GetOwner());
            return;
        }

        UINT OtherStep = OtherFireScript->GetSetp();

        // ��밡 ������ �ܰ谡 ���ٸ� �ƹ�ó���� ���� �ʴ´�.
        if (OtherStep > m_Step)
        {
            return;
        }
        else if (OtherStep == m_Step)
        {
            // �� Acc�� �� ª�� ��� ó������ �ʴ´�.
            float OtherAcc = OtherFireScript->GetAcc();
            if (OtherAcc >= m_Acc)
            {
                return;
            }
        }
        else if (m_Step == (UINT)Fire_Step::LV2)
        {
            m_Acc = 0.f;
        }

        // ��뺸�� Step�� ũ�ų� Acc�� ū Fire�� ��� �ݴ��� Fire�� �����ϰ� �ڽ��� AccNumber�� �����ش�.
        GamePlayStatic::DestroyGameObject(_OtherCollider->GetOwner());
        ++m_AccFireNumber;

        // AccFireNumber�� �ʿ��� ������ �Ѿ�� ��� ���� ū �ҷ� �ٲ��ش�.
        if (m_AccFireNumber >= m_RequiredFireNumber)
        {
            StepUp();
        }
    }
}

void CKirbyFireBullet::OnTriggerEnter(CCollider* _OtherCollider)
{
    // Monster�� �浹
 
    UINT LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    if (LayerIdx == LAYER_MONSTER_TRIGGER && _OtherCollider->GetOwner()->GetName() == L"Body Collider")
    {
        float Damage = 3.f;

        if (m_Step == 1)
        {
            Damage = 6.f;
        }
        else if (m_Step == 2)
        {
            Damage = 9.f;
        }


        CUnitScript* Monster = _OtherCollider->GetOwner()->GetParent()->GetScript<CUnitScript>();
        if (nullptr != Monster)
        {
            Vec3 HitDir = _OtherCollider->Transform()->GetWorldPos() - Transform()->GetWorldPos();
            HitDir.Normalize();
            UnitHit HitInfo = {DAMAGE_TYPE::NORMAL, HitDir, Damage, 0.f, 0.f};

            Monster->GetDamage(HitInfo);
        }

        GamePlayStatic::DestroyGameObject(GetOwner());
    }

}

UINT CKirbyFireBullet::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fwrite(&m_Step, sizeof(UINT), 1, _File);
    fwrite(&m_RequiredFireNumber, sizeof(int), 1, _File);
    fwrite(&m_Duration, sizeof(float), 1, _File);

    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(int);
    MemoryByte += sizeof(float);

    return MemoryByte;
}

UINT CKirbyFireBullet::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fread(&m_Step, sizeof(UINT), 1, _File);
    fread(&m_RequiredFireNumber, sizeof(int), 1, _File);
    fread(&m_Duration, sizeof(float), 1, _File);

    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(int);
    MemoryByte += sizeof(float);

    return MemoryByte;
}
