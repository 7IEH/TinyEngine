#include "pch.h"
#include "CKirbyAttack.h"

CKirbyAttack::CKirbyAttack()
{
}

CKirbyAttack::~CKirbyAttack()
{
}

void CKirbyAttack::tick()
{
    //wstring NextState = L"IDLE";
    PLAY_CURSTATE(Attack)

    // �⺻������ �����ؾ� �ϴ� ����

    // Change State
    //if (KEY_TAP(KEY::Q))
    //    NextState = L"ATTACK_COMBO";

    //ChangeState(NextState);
}

void CKirbyAttack::Enter()
{
    PLAY_CURSTATE(AttackEnter)
}

void CKirbyAttack::Exit()
{
    PLAY_CURSTATE(AttackExit)
}