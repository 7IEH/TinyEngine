#include "pch.h"
#include "CElfilisD_Roar.h"
#include "CBossMgr.h"
#include "CElfilisFSM.h"
#include "CElfilisBigFSM.h"

CElfilisD_Roar::CElfilisD_Roar()
    : m_AccTime(0.f)
    , m_bFrmEnter(true)
{
}

CElfilisD_Roar::~CElfilisD_Roar()
{
}

void CElfilisD_Roar::tick()
{
    if (m_bFrmEnter && CHECK_ANIMFRM(GetOwner(), 140))
    {
        //@EFFECT ���Ǹ��� BURN IN ����Ʈ
        ELFBIGFSM->Activate();
        ELFBIGFSM->ChangeState(L"ROAR");

        //@CAMERA ���, ����ũ

        m_bFrmEnter = false;
    }

    if (GetOwner()->Animator()->IsFinish())
    {
        ELFFSM->SetPattern(ElfilisPatternType::Appear2);
        ELFFSM->ProcPatternStep();
    }
}

void CElfilisD_Roar::Enter()
{
    GetOwner()->Animator()->Play(ANIMPREFIX("Roar"), false);
    m_bFrmEnter = true;

    //@CAMERA Ȯ��, ����ũ
}

void CElfilisD_Roar::Exit()
{
    //@CAMERA ��Ÿ��
}