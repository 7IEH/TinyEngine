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
    PLAY_CURSTATE(Attack)

    //if (�ִϸ��̼� ������)
    //{
    //    // ���� ���� ���� ��������� ���� �޶�ߵ�
    //    switch (����)
    //    {
    //    case �븻:
    //        break;

    //    case Fire:
    //        break;

    //    default:
    //        break;
    //    }
    //}
}

void CKirbyAttack::Enter()
{
    PLAY_CURSTATE(AttackEnter)
}

void CKirbyAttack::Exit()
{
    PLAY_CURSTATE(AttackExit)
}