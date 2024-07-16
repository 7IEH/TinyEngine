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
    CGameObject* pObj = _OtherCollider->GetOwner();
    CMonsterUnitScript* OwnerScript = GetOwner()->GetParent()->GetScript<CMonsterUnitScript>();

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

void CDetectAreaScript::SaveToLevelFile(FILE* _File)
{
}

void CDetectAreaScript::LoadFromLevelFile(FILE* _File)
{
}
