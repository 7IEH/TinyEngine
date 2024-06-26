#include "pch.h"
#include "CKirbyIdle.h"
#include "CKirbyFSM.h"
#include "CPlayerMgr.h"

#define KEY_TAP_ARROW (KEY_TAP(KEY::UP) || KEY_TAP(KEY::DOWN) || KEY_TAP(KEY::LEFT) || KEY_TAP(KEY::RIGHT))
#define KEY_PRESSED_ARROW (KEY_PRESSED(KEY::UP) || KEY_PRESSED(KEY::DOWN) || KEY_PRESSED(KEY::LEFT) || KEY_PRESSED(KEY::RIGHT))

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

    // �⺻������ �����ؾ� �ϴ� ����
    if (KEY_TAP_ARROW || KEY_PRESSED_ARROW)
        ChangeState(L"RUN");
}

void CKirbyIdle::Enter()
{
}

void CKirbyIdle::Exit()
{
}