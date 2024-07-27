#include "pch.h"
#include "CBossMgr.h"
#include "CUnitScript.h"
#include "CFSMScript.h"

CGameObject* CBossMgr::m_Boss = nullptr;
CUnitScript* CBossMgr::m_BossUnit = nullptr;
CFSMScript* CBossMgr::m_BossFSM = nullptr;

CBossMgr::CBossMgr()
    : CScript(BOSSMGR)
    , m_BossName(L"")
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_BossName, "Boss Object Name");
}

CBossMgr::~CBossMgr()
{
}

void CBossMgr::begin()
{
    if (!m_BossName.empty())
    {
        CGameObject* pBoss = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(m_BossName, LAYER_MONSTER);
        SetBoss(pBoss);
    }
}

void CBossMgr::SetBoss(CGameObject* _BossObj)
{
    if (!_BossObj)
    {
        MessageBox(nullptr, L"�������� �ʴ� ���� ������Ʈ�Դϴ�", L"BossMgr ��� ����", MB_OK);
        return;
    }

    CUnitScript* pBossUnit = _BossObj->GetScript<CUnitScript>();
    CFSMScript* pBossFSM = _BossObj->GetScript<CFSMScript>();

    if (!pBossUnit || !pBossFSM)
    {
        MessageBox(nullptr, L"Boss���� ������ ��ũ��Ʈ�� ã�� �� �����ϴ�", L"BossMgr ��� ����", MB_OK);
        return;
    }

    m_Boss = _BossObj;
    m_BossUnit = pBossUnit;
    m_BossFSM = pBossFSM;
}

UINT CBossMgr::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += SaveWStringToFile(m_BossName, _File);
    return MemoryByte;
}

UINT CBossMgr::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += LoadWStringFromFile(m_BossName, _File);
    return MemoryByte;
}