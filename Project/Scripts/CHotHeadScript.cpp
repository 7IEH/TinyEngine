#include "pch.h"
#include "CHotHeadScript.h"

CHotHeadScript::CHotHeadScript()
    : CMonsterUnitScript(HOTHEADSCRIPT)
    , m_eState(HOTHEAD_STATE::Idle)
    , m_vDamageDir{}
{
}

CHotHeadScript::CHotHeadScript(const CHotHeadScript& _Origin)
    : CMonsterUnitScript(_Origin)
    , m_eState(HOTHEAD_STATE::Idle)
    , m_vDamageDir{}
{
}

CHotHeadScript::~CHotHeadScript()
{
}

void CHotHeadScript::begin()
{
}

void CHotHeadScript::tick()
{
    ChangeState(HOTHEAD_STATE::Damage);
}

void CHotHeadScript::EnterState(HOTHEAD_STATE _state)
{
    switch (_state)
    {
    case HOTHEAD_STATE::Idle:
        break;
    case HOTHEAD_STATE::Damage:
        break;
    case HOTHEAD_STATE::End:
        break;
    default:
        break;
    }
}

void CHotHeadScript::ChangeState(HOTHEAD_STATE _state)
{
    ExitState(m_eState);
    m_eState = _state;
    EnterState(m_eState);
}

void CHotHeadScript::ExitState(HOTHEAD_STATE _state)
{
    switch (_state)
    {
    case HOTHEAD_STATE::Idle:
        break;
    case HOTHEAD_STATE::Damage:
        break;
    case HOTHEAD_STATE::End:
        break;
    default:
        break;
    }
}

void CHotHeadScript::SaveToLevelFile(FILE* _File)
{
    CMonsterUnitScript::SaveToLevelFile(_File);
}

void CHotHeadScript::LoadFromLevelFile(FILE* _File)
{
    CMonsterUnitScript::LoadFromLevelFile(_File);
}

void CHotHeadScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (HOTHEAD_STATE::Eaten == m_eState)
        return;

    CGameObject* pObj = _OtherCollider->GetOwner();
    bool flag = false;

    UnitHit hit;
    ZeroMemory(&hit, sizeof(hit));
    /**********************
    | 1. Player ATK Hit
    ***********************/

    // �浹�� ������Ʈ�� �÷��̾� �������� Ȯ��
    if (LAYER_PLAYERATK == pObj->GetLayerIdx())
    {
        flag = true;
        // TODO : �÷��̾� ���� ������ ������ ����

        GetDamage(hit);
        ChangeState(HOTHEAD_STATE::Damage);
        m_vDamageDir = pObj->GetParent()->GetComponent<CTransform>()->GetWorldDir(DIR_TYPE::FRONT);
    }

    /**********************
    | 2. Player Body Hit
    ***********************/
    // �浹�� ������Ʈ�� PlayerBody���� Ȯ��

    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        // �浹�� ������Ʈ Vaccum �̶�� Collider�� ���� ������ ��, ���Ƶ��̰� �ִ� ����
        if (L"Vacuum Collider" == pObj->GetName())
        {
            ChangeState(HOTHEAD_STATE::Eaten);
            m_vDamageDir = -pObj->GetComponent<CTransform>()->GetWorldDir(DIR_TYPE::FRONT);
            return;
        }

        pObj->GetScript<CUnitScript>()->GetDamage(GetHitInfo());

        flag = true;
    }

    // �� �� �ϳ��� �ǰ� �Ǿ��ٸ� ü�� Ȯ��
    if (flag)
    {
        if (GetCurInfo().HP - hit.Damage <= 0.f)
        {
            ChangeState(HOTHEAD_STATE::Death);
        }
    }
}

void CHotHeadScript::OnTriggerExit(CCollider* _OtherCollider)
{
    CGameObject* pObj = _OtherCollider->GetOwner();
    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        // �浹�� ������Ʈ Vaccum �̶�� Collider�� ���� ������ ��, ���Ƶ��̴°� ���� ����
        if (L"Vacuum Collider" == pObj->GetName())
        {
            ChangeState(HOTHEAD_STATE::Idle);
        }
    }
}

void CHotHeadScript::Idle()
{
}

void CHotHeadScript::Find()
{
}

void CHotHeadScript::AttackShotStart()
{
}

void CHotHeadScript::AttackShot()
{
}

void CHotHeadScript::AttackShotEnd()
{
}

void CHotHeadScript::AttackFlameStart()
{
}

void CHotHeadScript::AttackFlame()
{
}

void CHotHeadScript::AttackFlameEnd()
{
}

void CHotHeadScript::AttackFlameRotStart()
{
}

void CHotHeadScript::AttackFlameRot()
{
}

void CHotHeadScript::AttackFlameRotEnd()
{
}

void CHotHeadScript::Damage()
{
}

void CHotHeadScript::Death()
{
}
