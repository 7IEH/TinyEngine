#include "pch.h"
#include "CKirbyFinalCutterDrop.h"

CKirbyFinalCutterDrop::CKirbyFinalCutterDrop()
{
}

CKirbyFinalCutterDrop::~CKirbyFinalCutterDrop()
{
}

void CKirbyFinalCutterDrop::tick()
{
    if (PLAYERCTRL->IsGround())
    {
        ChangeState(L"FINALCUTTEREND");
    }
}

void CKirbyFinalCutterDrop::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("FinalCutterDrop"), true, false, 1.5f, 0);

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    // �߷� ����
    m_SaveGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(m_SaveGravity * 5.f);

    PLAYERCTRL->SetVelocity(Vec3(0.f, -50.f, 0.f));


    // Material ���
    Ptr<CMaterial> CutterMaterial = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalCutter_BladeC.mtrl");

    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(CutterMaterial, 0);

    // �Ӹ��� Blade�� �Ⱥ��̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(nullptr, 0);

    // ���� ����
    PLAYERFSM->SetInvincible(true);

    PLAYERFSM->GetCurWeapon()->SetActive(true);
}

void CKirbyFinalCutterDrop::Exit()
{
    // Material ����
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    // �߷� ��ȯ
    PLAYERCTRL->SetGravity(m_SaveGravity);

    // �Ӹ��� Blade�� ���̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalHat_BladeC.mtrl"), 0);

    // ���� ����
    PLAYERFSM->SetInvincible(false);

    PLAYERFSM->GetCurWeapon()->SetActive(true);
}
