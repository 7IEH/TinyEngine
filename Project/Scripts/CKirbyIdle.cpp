#include "pch.h"
#include "CKirbyIdle.h"

CKirbyIdle::CKirbyIdle()
{
}

CKirbyIdle::~CKirbyIdle()
{
}

void CKirbyIdle::tick()
{
    CKirbyFSM* KirbyFSM = CPlayerMgr::GetPlayerFSM();
    // CKirbyFSM* KirbyFSM = dynamic_cast<CKirbyFSM*>(GetOwnerFSM());
    // PlayerManager ������ �� -> manager�� player�� ��� �ְ�, �װ� �������°� ������ (level �Ǵ� ���� ���۽� 1ȸ ã��)

    // CASE: Object Copy�� ��� (�켱)
    if (KirbyFSM->GetCurObject())
    {
        KirbyFSM->GetCurObject()->Idle();
    }

    // CASE: Ability Copy�� ���
    else
    {
        // ���� state�� �´� �߰�����
        KirbyFSM->GetCurAbility()->Idle();
    }

    // �⺻������ �����ؾ� �ϴ� ������ ���Ⱑ �ƴѵ�;
    // Change State
    // ex, Change State�� ���⼭ �����ϸ� ...
    // (1) ���� ���� �򰥸�
    // (2) ���ʿ��� Change State �߻��ϰ� �Ǵٽ� �߻��ϴ� ��� �ǵ��Ѵ�� ���°� �� ���� ��� ����
    // - ex Ư������Start && ������ȯ -> Ư������~End���� �ǵ��Ѵ�� ���� ���� ��? �� ? �� ?
}

void CKirbyIdle::Enter()
{
    PLAY_CURSTATE(IdleEnter)
}

void CKirbyIdle::Exit()
{
    PLAY_CURSTATE(IdleExit)
}