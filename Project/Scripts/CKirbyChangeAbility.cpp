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
    // Ŀ�� ���͸��� �ٽ� ����
    CPlayerMgr::ClearBodyMtrl();

    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(BodyNormal));
    CPlayerMgr::SetPlayerMtrl(PLAYERMESH(MouthSmileOpen));

    CPlayerMgr::SetPlayerFace(FaceType::Normal);

    // ���� �Ⱦ��ֱ�
    PLAYERFSM->GetNextAbility()->ChangeAbilityEnter();

    // @TODO ī�޶� ����

    // @TODO Ŀ�� ī�޶����� �ٶ󺸰� ȸ��
    
    // @TODO ��� �� ȿ��

    // @TODO Ŀ�� ���� �ִϸ��̼� ���
    PLAYER->Animator()->Play(KIRBYANIM(L"EvolutionCopy"), false, false, 1.5f);

    // @TODO Ŀ�� ������ ����

    // @TODO Ŀ�� ������ ��� ������Ʈ�� ���߾�� ��



}

void CKirbyChangeAbility::Exit()
{
    PLAYERFSM->GetNextAbility()->ChangeAbilityExit();
}
