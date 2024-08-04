#include "pch.h"
#include "CBossMgr.h"
#include "CUnitScript.h"
#include "CFSMScript.h"

CGameObject* CBossMgr::m_Boss = nullptr;
CUnitScript* CBossMgr::m_BossUnit = nullptr;
CFSMScript* CBossMgr::m_BossFSM = nullptr;

CBossMgr::CBossMgr()
    : CScript(BOSSMGR)
    , m_BossName("")
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
        SetBoss();
    }
}

void CBossMgr::SetBoss()
{
    CGameObject* pBoss = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(ToWstring(m_BossName), LAYER_MONSTER);

    if (!pBoss)
    {
        MessageBox(nullptr, L"�������� �ʴ� ���� ������Ʈ�Դϴ�", L"BossMgr ��� ����", MB_OK);
        return;
    }

    CUnitScript* pBossUnit = pBoss->GetScript<CUnitScript>();
    CFSMScript* pBossFSM = pBoss->GetScript<CFSMScript>();

    if (!pBossUnit || !pBossFSM)
    {
        MessageBox(nullptr, L"Boss���� ������ ��ũ��Ʈ�� ã�� �� �����ϴ�", L"BossMgr ��� ����", MB_OK);
        return;
    }

    m_Boss = pBoss;
    m_BossUnit = pBossUnit;
    m_BossFSM = pBossFSM;
}

UINT CBossMgr::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += SaveWStringToFile(ToWstring(m_BossName), _File);
    return MemoryByte;
}

UINT CBossMgr::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;
    wstring BossName = L"";
    MemoryByte += LoadWStringFromFile(BossName, _File);
    m_BossName = ToString(BossName);

    return MemoryByte;
}