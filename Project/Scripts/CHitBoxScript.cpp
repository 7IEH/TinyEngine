#include "pch.h"
#include "CHitBoxScript.h"

#include "CMonsterUnitScript.h"

CHitBoxScript::CHitBoxScript()
    : CScript(HITBOXSCRIPT)
    , m_IsinvincibilityObj(false)
    , m_IsReflectionDamageObj(true)
{
}

CHitBoxScript::~CHitBoxScript()
{
}

void CHitBoxScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    CGameObject* pObj = _OtherCollider->GetOwner();
    bool IsDamaged = false;
    bool IsHitDamagePlayerBody = false;

    UnitHit hit;
    ZeroMemory(&hit, sizeof(hit));
    /**********************
    | 1. Player ATK Hit
    ***********************/

    // 1-1. �������� ���� �ʴ� ������Ʈ���� Ȯ��
    if (!m_IsinvincibilityObj)
    {
        // 1-2. �浹�� Collider�� �÷��̾��� �������� Ȯ��
        if (LAYER_PLAYERATK == pObj->GetLayerIdx())
        {
            // TODO : �ش� ������ ������ ����

            IsDamaged = true;
        }
    }

    /**********************
    | 2. Player Body Hit
    ***********************/

    // 2-1. �浹�� Collider�� �÷��̾��� �ٵ����� Ȯ��
    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        /*CMonsterUnitScript* pOwnerScript = GetOwner()->GetParent()->GetScript<CMonsterUnitScript>();
        hit.Damage = pOwnerScript->m_fBodyDamage;
        pObj->GetParent()->GetScript<CUnitScript>()->GetDamage(hit);*/

        // 2-2. �浹�� Object�� �ݻ絩�� ���� �������� Ȯ��
        if (m_IsReflectionDamageObj)
        {
            IsHitDamagePlayerBody = true;
        }
    }

    if (IsDamaged || IsHitDamagePlayerBody)
    {
        CGameObject* pTemp = pObj;
        if (pObj->GetParent())
        {
            pTemp = pObj->GetParent();
        }

        GetOwner()->GetParent()->GetScript<CMonsterUnitScript>()->TakeHit(hit, IsDamaged, IsHitDamagePlayerBody,
                                                                          -1 * pTemp->GetComponent<CTransform>()->GetWorldDir(DIR_TYPE::FRONT));
    }
}

void CHitBoxScript::OnTriggerExit(CCollider* _OtherCollider)
{
    bool IsDamaged = true;
    bool IsHitDamagePlayerBody = true;
    UnitHit hit = {};

    CGameObject* pObj = _OtherCollider->GetOwner();

    if (!m_IsinvincibilityObj)
    {
        // 1-2. �浹�� Collider�� �÷��̾��� �������� Ȯ��
        if (LAYER_PLAYERATK == pObj->GetLayerIdx())
        {
            IsDamaged = false;
        }
    }

    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        if (m_IsReflectionDamageObj)
        {
            IsDamaged = false;
        }
    }

    if (!IsDamaged || !IsHitDamagePlayerBody)
    {
        GetOwner()->GetParent()->GetScript<CMonsterUnitScript>()->TakeHit(hit, IsDamaged, IsHitDamagePlayerBody, Vec3(0.f, 0.f, 0.f));
    }
}

void CHitBoxScript::SaveToLevelFile(FILE* _File)
{
}

void CHitBoxScript::LoadFromLevelFile(FILE* _File)
{
}