#include "pch.h"
#include "CKirbyFinalCutterRise.h"

CKirbyFinalCutterRise::CKirbyFinalCutterRise()
    : m_SaveGravity(0.f)
{
}

CKirbyFinalCutterRise::~CKirbyFinalCutterRise()
{
}


void CKirbyFinalCutterRise::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"FINALCUTTERDROP");
    }
}

void CKirbyFinalCutterRise::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("FinalCutterRise"), false, false, 1.5f, 0);

    // ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();

    // �߷� ����
    m_SaveGravity = PLAYERCTRL->GetGravity();
    PLAYERCTRL->SetGravity(m_SaveGravity * 1.5f);

    //������ ����
    m_SaveJumpPower = PLAYERCTRL->GetJumpPower();
    PLAYERCTRL->SetJumpPower(m_SaveJumpPower * 2.f);

    // JUMP
    PLAYERCTRL->Jump();
    
    // Material ���
    Ptr<CMaterial> CutterMaterial = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalCutter_BladeC.mtrl");

    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(CutterMaterial, 0);

    // �Ӹ��� Blade�� �Ⱥ��̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(nullptr, 0);

    // ���� ����
    PLAYERFSM->SetInvincible(true);

    // ���� Active
    PLAYERFSM->GetCurWeapon()->SetActive(true);

}

void CKirbyFinalCutterRise::Exit()
{
    // ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();

    // �߷� ��ȯ
    PLAYERCTRL->SetGravity(m_SaveGravity);

    // ������ ��ȯ
    PLAYERCTRL->SetJumpPower(m_SaveJumpPower);

    // Material ����
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // �Ӹ��� Blade�� ���̰� �Ѵ�
    PLAYERFSM->GetCurHatBlade()->MeshRender()->SetMaterial(CAssetMgr::GetInst()->Load<CMaterial>(L"material\\MetalHat_BladeC.mtrl"), 0);

    // ���� ����
    PLAYERFSM->SetInvincible(false);

    PLAYERFSM->GetCurWeapon()->SetActive(false);
}
