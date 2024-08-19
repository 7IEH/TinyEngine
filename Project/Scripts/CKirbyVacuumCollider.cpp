#include "pch.h"
#include "CKirbyVacuumCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyMoveController.h"
#include "CMonsterUnitScript.h"
#include "CKirbyCopyAbilityScript.h"
#include "CKirbyCopyObjScript.h"

CKirbyVacuumCollider::CKirbyVacuumCollider()
    : CScript(KIRBYVACUUMCOLLIDER)
    , m_FindTarget(nullptr)
    , m_FindType(EatType::NONE)
    , m_FindAbilityType(AbilityCopyType::NONE)
    , m_FindObjType(ObjectCopyType::NONE)
    , m_FindDistance(FBXSDK_FLOAT_MAX)
    , m_FindHoldTime(0.f)
    , m_bDrawing(false)
{
}

CKirbyVacuumCollider::CKirbyVacuumCollider(const CKirbyVacuumCollider& _Origin)
    : CScript(KIRBYVACUUMCOLLIDER)
    , m_FindTarget(nullptr)
    , m_FindType(EatType::NONE)
    , m_FindAbilityType(AbilityCopyType::NONE)
    , m_FindObjType(ObjectCopyType::NONE)
    , m_FindDistance(FBXSDK_FLOAT_MAX)
    , m_FindHoldTime(0.f)
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
        // target object�� monster��� ��Ƽ�� ���·� ���������
        if (m_FindType == EatType::Copy_Monster || m_FindType == EatType::Monster)
        {
            if (m_FindTarget->Rigidbody())
            {
                m_FindTarget->Rigidbody()->SetVelocity(Vec3());
                m_FindTarget->Rigidbody()->SetAngularVelocity(Vec3());
            }

            m_FindTarget->GetScript<CMonsterUnitScript>()->SetResistState(true);
        }

        // collider clear
        EnableCollider(false);

        //@Effect ��¦ ȿ�� ���̴�
        m_bDrawing = true;
    }

    // Ÿ�� ������Ʈ ���Ƶ���
    if (m_bDrawing)
    {
        // safety check
        Vec3 Dist = PLAYER->Transform()->GetWorldPos() - m_FindTarget->Transform()->GetWorldPos();
        float Radius = PLAYER->GetChildObject(L"Body Collider")->CapsuleCollider()->GetRadius();
        Radius *= PLAYER->Transform()->GetWorldScale().x;

        if (Dist.Length() <= Radius + 1e-3)
        {
            DrawingCollisionEnter(m_FindTarget);
            return;
        }

        // force
        Vec3 Force = Dist.Normalize();
        float PrevSpeed = m_FindTarget->Rigidbody()->GetVelocity().Length();
        m_FindTarget->Rigidbody()->SetVelocity(Force * PrevSpeed);

        if (m_FindHoldTime > 0.f)
        {
            m_FindHoldTime -= DT;
            Force = Force.Normalize() * 7.5f;
        }
        else
        {
            Force = Force.Normalize() * 60.f;
        }

        m_FindTarget->Rigidbody()->AddForce(Force, ForceMode::Acceleration);
    }
}

void CKirbyVacuumCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    // ����� �� �ִ� ��ü���
    int i = _OtherCollider->GetOwner()->GetLayerIdx();
    if (_OtherCollider->GetOwner()->GetLayerIdx() != LAYER_DYNAMIC && _OtherCollider->GetOwner()->GetLayerIdx() != LAYER_MONSTER)
        return;

    bool bChanged = false;
    AbilityCopyType newAbility = AbilityCopyType::NONE;
    ObjectCopyType newObject = ObjectCopyType::NONE;
    float newHoldTime = 0.f;
    float newDist = (PLAYER->Transform()->GetWorldPos() - _OtherCollider->Transform()->GetWorldPos()).Length();
    EatType newType = GetEatType(_OtherCollider->GetOwner(), newAbility, newObject, newHoldTime);

    if (newType == EatType::NONE)
        return;

    // ���� ��������� ������Ʈ�� ���� �켱���� �Ǵ�
    if (PLAYERFSM->GetCurAbilityIdx() == AbilityCopyType::NORMAL)
    {
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
    }
    else
    {
        if (newType == EatType::Copy_Object && newDist < m_FindDistance)
        {
            bChanged = true;
        }
    }

    if (bChanged)
    {
        PLAYERCTRL->LockJump();
        PLAYERCTRL->LockDirection();
        PLAYERCTRL->LockMove();
        PLAYERFSM->SetGlobalState(true);

        m_FindTarget = _OtherCollider->GetOwner();
        m_FindType = newType;
        m_FindDistance = newDist;
        m_FindAbilityType = newAbility;
        m_FindObjType = newObject;
        m_FindHoldTime = newHoldTime;
    }
}

void CKirbyVacuumCollider::DrawingCollisionEnter(CGameObject* _CollisionObject)
{
    if (!m_bDrawing || _CollisionObject != m_FindTarget)
        return;

    m_bDrawing = false;
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERFSM->SetGlobalState(false);

    // Change Player State
    switch (m_FindType)
    {
    case EatType::Copy_Ability:
    case EatType::Copy_Monster:
        PLAYERFSM->ChangeAbilityCopy(m_FindAbilityType);
        GamePlayStatic::DestroyGameObject(m_FindTarget);
        break;
    case EatType::Copy_Object:
        PLAYERFSM->ChangeObjectCopy(m_FindObjType);
        GamePlayStatic::DestroyGameObject(m_FindTarget);
        break;
    case EatType::Etc:
    case EatType::Monster:
        PLAYERFSM->StartStuffed(m_FindTarget);
        m_FindTarget->SetActive(false);
        break;
    }

    m_FindTarget = nullptr;
}

void CKirbyVacuumCollider::EnableCollider(bool _bEnable)
{
    GetOwner()->SphereCollider()->SetEnabled(_bEnable);

    if (_bEnable)
    {
        m_FindTarget = nullptr;
        m_FindType = EatType::NONE;
        m_FindAbilityType = AbilityCopyType::NONE;
        m_FindObjType = ObjectCopyType::NONE;
        m_bDrawing = false;
        m_FindDistance = FBXSDK_FLOAT_MAX;
        m_FindHoldTime = 0.f;
    }
}

EatType CKirbyVacuumCollider::GetEatType(CGameObject* _pObj, AbilityCopyType& _outAbility, ObjectCopyType& _outObj, float& _outHoldTime)
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
            CMonsterUnitScript* pMonster = _pObj->GetScript<CMonsterUnitScript>();
            
            if (!pMonster || !pMonster->IsEatable())
            {
                return EatType::NONE;
            }

            _outHoldTime = pMonster->GetResistTime();
            return EatType::Monster;
        }
    }

    return EatType::NONE;
}

UINT CKirbyVacuumCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CKirbyVacuumCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}