#include "pch.h"
#include "CKirbyVacuumCollider.h"
#include "CPlayerMgr.h"
#include "CMonsterUnitScript.h"
#include "CKirbyCopyAbilityScript.h"
#include "CKirbyCopyObjScript.h"

CKirbyVacuumCollider::CKirbyVacuumCollider()
    : CScript(KIRBYVACUUMCOLLIDER)
    , m_FindTarget(nullptr)
    , m_FindDistance(FBXSDK_FLOAT_MAX)
    , m_FindType(EatType::NONE)
    , m_FindAbilityType(AbilityCopyType::NONE)
    , m_FindObjType(ObjectCopyType::NONE)
    , m_bDrawing(false)
{
}

CKirbyVacuumCollider::CKirbyVacuumCollider(const CKirbyVacuumCollider& _Origin)
    : CScript(KIRBYVACUUMCOLLIDER)
    , m_FindTarget(nullptr)
    , m_FindDistance(FBXSDK_FLOAT_MAX)
    , m_FindType(EatType::NONE)
    , m_FindAbilityType(AbilityCopyType::NONE)
    , m_FindObjType(ObjectCopyType::NONE)
    , m_bDrawing(false)
{
}

CKirbyVacuumCollider::~CKirbyVacuumCollider()
{
}

void CKirbyVacuumCollider::begin()
{
    if (!GetOwner()->SphereCollider())
    {
        MessageBox(nullptr, L"Vacuum Collider�� Collider ������Ʈ�� �������� �ʽ��ϴ� (���� ���� ���)", L"CKirbyVacuumCollider ���", MB_OK);

        CSphereCollider* newCol = new CSphereCollider();
        newCol->SetTrigger(true);
        GetOwner()->AddComponent(newCol);
    }

    GetOwner()->SphereCollider()->SetEnabled(false);
}

void CKirbyVacuumCollider::tick()
{
    // ���� �浹 ���� �� ã�� ����� �ִٸ�
    if (!m_bDrawing && m_FindTarget)
    {
        //@TODO target object�� monster��� Vacuum Drawed�� ���� ����
        if (m_FindType == EatType::Copy_Monster || m_FindType == EatType::Monster)
        {
            // m_FindTarget->GetScript<CFSMScript>()->ChangeState(L"VACUUM_DRAWED");
        }

        // collider clear
        EnableCollider(false);

        //@TODO ��¦ ȿ�� ���̴�
        m_bDrawing = true;
    }

    // Ÿ�� ������Ʈ ���Ƶ���
    if (m_bDrawing)
    {
        Vec3 Force = PLAYER->Transform()->GetWorldPos() - m_FindTarget->Transform()->GetWorldPos();
        Force = (Force).Normalize() * 40.f;
        m_FindTarget->Rigidbody()->AddForce(Force, ForceMode::Acceleration);
    }
}

void CKirbyVacuumCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    // ����� �� �ִ� ��ü���
    int i = _OtherCollider->GetOwner()->GetLayerIdx();
    if (_OtherCollider->GetOwner()->GetLayerIdx() != LAYER_DYNAMIC && _OtherCollider->GetOwner()->GetLayerIdx() != LAYER_MONSTER)
        return;

    bool TriggerEnable = GetOwner()->SphereCollider()->IsEnabled();
    bool bChanged = false;
    AbilityCopyType newAbility = AbilityCopyType::NONE;
    ObjectCopyType newObject = ObjectCopyType::NONE;
    EatType newType = GetEatType(_OtherCollider->GetOwner(), newAbility, newObject);
    float newDist = (PLAYER->Transform()->GetWorldPos() - _OtherCollider->Transform()->GetWorldPos()).Length();

    // ���� ��������� ������Ʈ�� ���� �켱���� �Ǵ�
    if ((UINT)newType < (UINT)m_FindType)
    {
        bChanged = true;
    }
    else if ((UINT)newType == (UINT)m_FindType)
    {
        if (newDist < m_FindDistance)
        {
            bChanged = true;
        }
    }

    if (bChanged)
    {
        PLAYERFSM->SetGlobalState(true);

        m_FindTarget = _OtherCollider->GetOwner();
        m_FindType = newType;
        m_FindDistance = newDist;
        m_FindAbilityType = newAbility;
        m_FindObjType = newObject;
    }
}

void CKirbyVacuumCollider::DrawingCollisionEnter(CGameObject* _CollisionObject)
{
    if (_CollisionObject != m_FindTarget)
        return;

    m_bDrawing = false;
    PLAYERFSM->SetGlobalState(false);

    // Change Player State
    switch (m_FindType)
    {
    case EatType::Copy_Ability:
    case EatType::Copy_Monster:
        PLAYERFSM->ChangeAbilityCopy(m_FindAbilityType);
        break;
    case EatType::Copy_Object:
        PLAYERFSM->ChangeObjectCopy(m_FindObjType);
        break;
    case EatType::Etc:
    case EatType::Monster:
        PLAYERFSM->StartStuffed(m_FindTarget->Clone());
        break;
    }

    // �浹 ������Ʈ ����
    GamePlayStatic::DestroyGameObject(m_FindTarget);
    m_FindTarget = nullptr;
}

void CKirbyVacuumCollider::EnableCollider(bool _bEnable)
{
    GetOwner()->SphereCollider()->SetEnabled(_bEnable);

    if (_bEnable)
    {
        m_FindTarget = nullptr;
        m_FindDistance = FBXSDK_FLOAT_MAX;
        m_FindType = EatType::NONE;
        m_FindAbilityType = AbilityCopyType::NONE;
        m_FindObjType = ObjectCopyType::NONE;
        m_bDrawing = false;
    }
}

EatType CKirbyVacuumCollider::GetEatType(CGameObject* _pObj, AbilityCopyType& _outAbility, ObjectCopyType& _outObj)
{
    if (_pObj->GetLayerIdx() == LAYER_DYNAMIC)
    {
        if (_pObj->GetScript<CKirbyCopyObjScript>())
        {
            _outObj = _pObj->GetScript<CKirbyCopyObjScript>()->GetObjType();
            return EatType::Copy_Object;
        }
        else if (_pObj->GetScript<CKirbyCopyAbilityScript>())
        {
            _outAbility = _pObj->GetScript<CKirbyCopyAbilityScript>()->GetAbilityType();
            return EatType::Copy_Ability;
        }
        else
        {
            return EatType::Etc;
        }
    }

    else if (_pObj->GetLayerIdx() == LAYER_MONSTER)
    {
        if (_pObj->GetScript<CKirbyCopyAbilityScript>())
        {
            _outAbility = _pObj->GetScript<CKirbyCopyAbilityScript>()->GetAbilityType();
            return EatType::Copy_Monster;
        }
        else
        {
            return EatType::Monster;
        }
    }
}

void CKirbyVacuumCollider::SaveToLevelFile(FILE* _File)
{
}

void CKirbyVacuumCollider::LoadFromLevelFile(FILE* _File)
{
}