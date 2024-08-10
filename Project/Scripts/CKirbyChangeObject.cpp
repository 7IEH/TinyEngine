#include "pch.h"
#include "CKirbyChangeObject.h"

#include "CCameraController.h"

CKirbyChangeObject::CKirbyChangeObject()
{
}

CKirbyChangeObject::~CKirbyChangeObject()
{
}

void CKirbyChangeObject::tick()
{
    PLAYERFSM->GetCurObject()->ChangeObject();

    // State Change
    switch (PLAYERFSM->GetCurObjectIdx())
    {
    case ObjectCopyType::CONE:
    case ObjectCopyType::VENDING_MACHINE:
    case ObjectCopyType::LIGHT: {
        if (PLAYER->Animator()->IsFinish())
        {
            PLAYERFSM->SetGlobalState(false);
            ChangeState(L"CHANGE_OBJECT_END");
        }
    }
        break;
    }
}

void CKirbyChangeObject::Enter()
{
    PLAYERFSM->SetGlobalState(true);
    PLAYERFSM->GetCurObject()->ChangeObjectEnter();

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;
    CamCtrl->SaveSetting();

    CamCtrl->SetOffset(Vec3(0.f, 15.f, 0));
    CamCtrl->SetLookDir(Vec3(0.f, -0.35f, 0.937f));
    CamCtrl->SetLookDist(170.f);

    // @TODO ī�޶� ���� ����
    Vec3 CamPos = CamCtrl->GetOwner()->Transform()->GetWorldPos();

    // @TODO Ŀ�� ���� ����
    Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();
    Vec3 Dir = CamPos - PlayerPos;
    Dir.y = 0.f;
    Dir.Normalize();

    ForceDirInfo DirInfo = {ForceDirType::STAGEEVENT, Dir, false};
    PLAYERCTRL->ForceDir(DirInfo);

    // @TODO Ÿ�� ������ ���� 


    // @TODO
    // ��� �� ȿ��
    // Ŀ�� ������ ��� ������Ʈ�� ���߾�� ��
}

void CKirbyChangeObject::Exit()
{
    PLAYERFSM->GetCurObject()->ChangeObjectExit();

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;
    CamCtrl->LoadSetting();

    //@TODO
    // ��� �� ȿ�� ����
    // Ŀ�� �ǿ��� ������Ʈ �ٽ� DT�޵��� ����
}
