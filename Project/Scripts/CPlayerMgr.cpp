#include "pch.h"
#include "CPlayerMgr.h"

static CGameObject* m_PlayerObj = nullptr;
static CUnitSCript* m_PlayerUnit = nullptr;
static CKirbyFSM* m_PlayerFSM = nullptr;

CPlayerMgr::CPlayerMgr()
    : CScript(PLAYERMGR)
{
}

CPlayerMgr::~CPlayerMgr()
{
}

void CPlayerMgr::begin()
{
}

void CPlayerMgr::tick()
{
}

void CPlayerMgr::SetPlayer(CGameObject* _PlayerObj)
{
    CUnitSCript* pPlayerUnit = _PlayerObj->GetScript<CUnitSCript>();
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