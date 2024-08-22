#include "pch.h"
#include "CDetectAreaScript.h"
#include "CMonsterUnitScript.h"

CDetectAreaScript::CDetectAreaScript()
    : CScript(DETECTAREASCRIPT)
{
}

CDetectAreaScript::~CDetectAreaScript()
{
}

void CDetectAreaScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    // CDetectAreaScript�� �����ϰ��ִ� ������Ʈ�� �ڽĿ�����Ʈ�� �ƴѰ��
    if (!GetOwner()->GetParent())
        return;

    CGameObject* pObj = _OtherCollider->GetOwner(); 
    CMonsterUnitScript* OwnerScript = GetOwner()->GetParent()->GetScript<CMonsterUnitScript>();

    // CDetectAreaScript�� �����ϰ��ִ� ������Ʈ�� �θ� Monster�� �ƴ� ���
    if (nullptr == OwnerScript)
        return;

    // �÷��̾� ����
    if (pObj->GetLayerIdx() == LAYER_PLAYER)
    {
        OwnerScript->SetTarget(pObj);
    }
}

void CDetectAreaScript::OnTriggerExit(CCollider* _OtherCollider)
{
    CGameObject* pObj = _OtherCollider->GetOwner();
    if (pObj->GetLayerIdx() == LAYER_PLAYER)
    {
        GetOwner()->GetParent()->GetScript<CMonsterUnitScript>()->SetTarget(nullptr);
    }
}

UINT CDetectAreaScript::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CDetectAreaScript::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}
