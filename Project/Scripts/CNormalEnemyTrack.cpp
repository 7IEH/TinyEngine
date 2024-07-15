#include "pch.h"
#include "CNormalEnemyTrack.h"

#include "CMonsterMgr.h"
#include "CMonsterMoveController.h"

CNormalEnemyTrack::CNormalEnemyTrack()
    : m_pMgr(nullptr)
    , m_pTransform(nullptr)
    , m_pMMC(nullptr)
{
}

CNormalEnemyTrack::~CNormalEnemyTrack()
{
}

void CNormalEnemyTrack::tick()
{
    bool _IsTrack = m_pMgr->IsTrack(m_pTransform->GetLocalPos(), m_pMMC->GetTrackDist());

    if (!_IsTrack)
    {
        ChangeState(L"IDLE");
    }
}

void CNormalEnemyTrack::Enter()
{
    if (nullptr == m_pMgr)
    {
        m_pMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CMonsterMgr>();
        m_pTransform = GetOwner()->GetComponent<CTransform>();
        m_pMMC = GetOwner()->GetScript<CMonsterMoveController>();
    }
}

void CNormalEnemyTrack::Exit()
{
    m_pMMC->SetTrack(false);
}