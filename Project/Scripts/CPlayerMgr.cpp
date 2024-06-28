#include "pch.h"
#include "CPlayerMgr.h"
#include "CUnitScript.h"
#include "CKirbyFSM.h"
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>

CGameObject* CPlayerMgr::m_PlayerObj = nullptr;
CUnitScript* CPlayerMgr::m_PlayerUnit = nullptr;
CKirbyFSM* CPlayerMgr::m_PlayerFSM = nullptr;

CPlayerMgr::CPlayerMgr()
    : CScript(PLAYERMGR)
{
}

CPlayerMgr::~CPlayerMgr()
{
}

void CPlayerMgr::begin()
{
    //CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(L"Character")->
    CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Main Player");
    SetPlayer(pPlayer);
}

void CPlayerMgr::tick()
{
}

void CPlayerMgr::SetPlayer(CGameObject* _PlayerObj)
{
    CUnitScript* pPlayerUnit = _PlayerObj->GetScript<CUnitScript>();
    CKirbyFSM* pPlayerFSM = _PlayerObj->GetScript<CKirbyFSM>();

    if (!pPlayerUnit || !pPlayerFSM)
    {
        MessageBox(nullptr, L"Player���� Unit�̳� FSM Script�� ã�� �� �����ϴ�.", L"�÷��̾� ��� ����", MB_OK);
        return;
    }

    m_PlayerObj = _PlayerObj;
    m_PlayerUnit = pPlayerUnit;
    m_PlayerFSM = pPlayerFSM;
}

void CPlayerMgr::SaveToLevelFile(FILE* _File)
{
}

void CPlayerMgr::LoadFromLevelFile(FILE* _File)
{
}