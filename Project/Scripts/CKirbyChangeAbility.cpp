#include "pch.h"
#include "CKirbyChangeAbility.h"

CKirbyChangeAbility::CKirbyChangeAbility()
{
}

CKirbyChangeAbility::~CKirbyChangeAbility()
{
}

void CKirbyChangeAbility::tick()
{
    // �ִϸ��̼� ����� �ʿ��� �۾� 
    PLAYERFSM->GetNextAbility()->ChangeAbility();

    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"CHANGE_ABILITY_WAIT");
    }

}

void CKirbyChangeAbility::Enter()
{
    // ���� �Ⱦ��ֱ�
    PLAYERFSM->GetNextAbility()->ChangeAbilityEnter();
    
    // Ŀ�� ���� �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"EvolutionCopy"), false, false, 1.5f);

    // Ŀ�� ���͸��� �ٽ� ����
    CPlayerMgr::ClearBodyMtrl();
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthSmileOpen));
    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    // @TODO 
    // ī�޶� ����
    // Ŀ�� ī�޶����� �ٶ󺸰� ȸ��
    // ��� �� ȿ��
    // Ŀ�� ������ ����
    // Ŀ�� ������ ��� ������Ʈ�� ���߾�� ��
}

void CKirbyChangeAbility::Exit()
{
    PLAYERFSM->GetNextAbility()->ChangeAbilityExit();
}
