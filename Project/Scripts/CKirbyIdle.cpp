#include "pch.h"
#include "CKirbyIdle.h"

// ��?��?
#include "CKirbyFSM.h"
#include "CPlayerMgr.h"
#include "CKirbyObject.h"
#include "CKirbyAbility.h"

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
        //KirbyFSM->GetCurAbility()->Idle();
    }

    // �⺻������ �����ؾ� �ϴ� ����

    // Change State
    if (KEY_TAP(KEY::Q))
        ChangeState(L"ATTACK");
    else if (KEY_TAP_ARROW || KEY_PRESSED_ARROW)
        ChangeState(L"RUN");
}

void CKirbyIdle::Enter()
{
}

void CKirbyIdle::Exit()
{
}