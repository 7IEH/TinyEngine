#include "pch.h"
#include "CKirbyDeath.h"

#include "CCameraController.h"

CKirbyDeath::CKirbyDeath()
{
}

CKirbyDeath::~CKirbyDeath()
{
}

void CKirbyDeath::tick()
{
    m_Acc += DT;

    if (m_Acc != 0.f)
    {
        PLAYER->Animator()->SetPlay(false);
    }

    // ���ߴ� �ð��� �������
    if (m_Acc > m_Duration)
    {
        // �ִϸ��̼� ���
        PLAYER->Animator()->SetPlay(true);

        // @TODO Ŀ��� UI�� ������ ����� ���������� Fade Out (����ŷ�� Ŀ���� ��ġ�� ������ ������ ���� ���������� �ٲ��ִ� Post Process ���̴� �����ϸ� �� ��)


        
    }

    if (m_Acc > m_FaceDuraion)
    {
        CPlayerMgr::SetPlayerFace(FaceType::Close); 

        // Camera ����
        CCameraController* CamCtrl = CAMERACTRL;
        CamCtrl->SetOffset(Vec3(0.f, 0.f, 0));
        CamCtrl->SetLookDir(Vec3(0.f, -1.f, 0.f));
        CamCtrl->SetLookDist(100.f);
        CamCtrl->SetZoomMinSpeed(50.f);
        CamCtrl->SetZoomMaxSpeed(100.f);
        CamCtrl->SetZoomThreshold(500.f);
    }

    if (m_Acc > m_DeathDuraion)
    {
        // @TODO �ִϸ��̼��� ������ UI�� ȭ���� ����

        // @TODO UI�ִϸ��̼� ��� �����ٸ� Ŀ�� �ǻ�� ��

    }
}

void CKirbyDeath::Enter()
{
    // @TODO m_Duration ��ŭ �ð��� ���߱�

    // Ŀ�� ǥ��
    CPlayerMgr::SetPlayerFace(FaceType::Frown);

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;

    // Camera Shake
    CamCtrl->Shake(0.5f, 30.f, 30.f);
     
    Vec3 CamPos = CamCtrl->GetOwner()->Transform()->GetWorldPos();

    // @TODO Ŀ�� ���� ����
    Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();
    Vec3 Dir = CamPos - PlayerPos;
    Dir.y = 0.f;
    Dir.Normalize();

    ForceDirInfo DirInfo = {ForceDirType::STAGEEVENT, Dir,true};
    PLAYERCTRL->ForceDir(DirInfo);
    PLAYERFSM->SetGlobalState(true);
    GetOwner()->Animator()->Play(ANIMPREFIX("Death"), false);

    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();

    m_Acc = 0.f;
    m_Duration = 1.f;
    m_FaceDuraion = 3.f;
    m_DeathDuraion = 6.f;
}

void CKirbyDeath::Exit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
}