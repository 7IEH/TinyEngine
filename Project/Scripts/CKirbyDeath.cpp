#include "pch.h"
#include "CKirbyDeath.h"

#include "CCameraController.h"

#include "CLevelFlowMgr.h"

CKirbyDeath::CKirbyDeath()
{
}

CKirbyDeath::~CKirbyDeath()
{
}

void CKirbyDeath::tick()
{
    m_Acc += DT_ENGINE;

    if (m_Acc != 0.f)
    {
        PLAYER->Animator()->SetPlay(false);
    }

    // ���ߴ� �ð��� �������
    if (m_Acc > m_Duration)
    {
        // �ִϸ��̼� ���
        PLAYER->Animator()->SetPlay(true);
    }

    if (m_Acc > m_FaceDuraion)
    {
        CPlayerMgr::SetPlayerFace(FaceType::Close);

        // Camera ����
        CCameraController* CamCtrl = CAMERACTRL;
        CamCtrl->Normal(false);
        CamCtrl->SetOffset(Vec3(0.f, 0.f, 0));
        CamCtrl->SetLookDir(Vec3(0.f, -1.f, 0.f));
        CamCtrl->SetLookDist(100.f);
        CamCtrl->SetZoomMinSpeed(50.f);
        CamCtrl->SetZoomMaxSpeed(100.f);
        CamCtrl->SetZoomThreshold(500.f);

        // Ŀ��� UI�� ������ ����� ���������� Fade Out
        if (m_bFadeEffect == false)
        {
            CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
            FlowMgr->OnDimensionFade(1.f, 0.f, 2.5f);

            m_bFadeEffect = true;
        }
    }

    if (m_Acc > m_DeathDuraion)
    {
        CTimeMgr::GetInst()->SetTimeScale(1.f);

        // Level Restart
        CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
        FlowMgr->LevelRestart();
    }
}

void CKirbyDeath::Enter()
{
    // Effect Lock
    CAMERACTRL->SetEffectLock(true);

    // Death State ���� �� ���Ϳ� ���õ� ��� ������Ʈ�� �����Ѵ�.
    //for (int i = LAYER_MONSTER; i <= LAYER_MONSTERATK_TRIGGER; ++i)
    //{
    //    CLayer* CurLayer = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(i);

    //    const vector<CGameObject*>& Objects = CurLayer->GetParentObjects();

    //    for (size_t j = 0; j < Objects.size(); ++j)
    //    {
    //        GamePlayStatic::DestroyGameObject(Objects[j]);
    //    }
    //}

    m_Acc = 0.f;
    m_Duration = 1.f;
    m_FaceDuraion = 3.f;
    m_DeathDuraion = 6.f;
    m_bFadeEffect = false;

    // FlowMgr���� ƽ�� �ʱ� ������ ùƽ���� ȿ���� ������ ���� ������ ����Ǿ��ִ� coef���� �������� Fade�� �ȴ�. �׷��� coef�� �̸� 1�� �ʱ�ȭ
    CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
    FlowMgr->OnDimensionFade(1.f);
    FlowMgr->OffDimensionFade();

    // m_Duration ��ŭ �ð��� ���߱�
    CTimeMgr::GetInst()->SetTimeScale(0.f, m_Duration);

    // Ŀ�� ǥ��
    CPlayerMgr::SetPlayerFace(FaceType::Frown);

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;

    // Camera Shake
    CamCtrl->Shake(0.5f, 50.f, 50.f);

    Vec3 CamPos = CamCtrl->GetOwner()->Transform()->GetWorldPos();

    // @TODO Ŀ�� ���� ����
    Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();
    Vec3 Dir = CamPos - PlayerPos;
    Dir.y = 0.f;
    Dir.Normalize();

    ForceDirInfo DirInfo = {ForceDirType::STAGEEVENT, Dir, true};
    PLAYERCTRL->ForceDir(DirInfo);
    PLAYERFSM->SetGlobalState(true);
    GetOwner()->Animator()->Play(ANIMPREFIX("Death"), false);

    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();

    // UI ����
    {
        CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
        FlowMgr->TurnOffBossHP();
        FlowMgr->TurnOffPlayerHP();
        FlowMgr->ActiveOffDropUI();
    }
}

void CKirbyDeath::Exit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
}