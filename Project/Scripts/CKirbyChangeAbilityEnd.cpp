#include "pch.h"
#include "CKirbyChangeAbilityEnd.h"

CKirbyChangeAbilityEnd::CKirbyChangeAbilityEnd()
{
}

CKirbyChangeAbilityEnd::~CKirbyChangeAbilityEnd()
{
}

void CKirbyChangeAbilityEnd::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"IDLE");
    }

}

void CKirbyChangeAbilityEnd::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("EvolutionCopyEnd"), false, false, 1.5f);
}

void CKirbyChangeAbilityEnd::Exit()
{
    // Ŀ�� ǥ�� ����
    CPlayerMgr::ClearMouthMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthNormal));

    //@TODO
    //ī�޶� ���� ����
    //��� �� ȿ�� ����
    //Ŀ�� ������ ���� ����
    //Ŀ�� �ǿ��� ������Ʈ �ٽ� DT�޵��� ����

}
