#include "pch.h"
#include "CKirbyFinalCutterEnd.h"

#include "CKirbyHatBlade.h"

CKirbyFinalCutterEnd::CKirbyFinalCutterEnd()
{
}

CKirbyFinalCutterEnd::~CKirbyFinalCutterEnd()
{
}


void CKirbyFinalCutterEnd::tick()
{
    if (PLAYER->Animator()->IsFinish())
    {
        ChangeState(L"FINALCUTTERENDAFTER");
    }
}

void CKirbyFinalCutterEnd::Enter()
{
    PLAYER->Animator()->Play(ANIMPREFIX("FinalCutterEnd"), false, false, 1.5f, 0);

    // ����, ����, �̵� Lock
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockDirection();
    PLAYERCTRL->LockJump();

    // Material ���
    Ptr<CMaterial> CutterMaterial = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\MetalCutter_BladeC.mtrl");

    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(CutterMaterial, 0);

    
    // ���� ����
    PLAYERFSM->SetInvincible(true);

        // �����ʿ� Blade ��������ϱ�
    CKirbyHatBlade* HatBladeScript = dynamic_cast<CKirbyHatBlade*>(PLAYERFSM->GetCurHatBlade()->GetScripts()[0]);

    if (HatBladeScript != nullptr)
    {
        HatBladeScript->Attack();
    }

    // Bullet �߻�
    // Prefab ��������
    Ptr<CPrefab> CutterBulletPrefab = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\MetalCutter.pref");

    // �ʱⰪ ����
    CGameObject* InstObj = CutterBulletPrefab->Instantiate();
    Vec3 KirbyPos = PLAYER->Transform()->GetWorldPos();
    Vec3 KirbyFront = PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    KirbyFront.y = 0.f;
    KirbyFront.Normalize();

    Vec3 Offset = Vec3(0.f, 50.f, 0.f);

    // Transform
    InstObj->Transform()->SetWorldPos(KirbyPos + Offset);
    InstObj->Transform()->SetDirection(-KirbyFront);

    GamePlayStatic::SpawnGameObject(InstObj, InstObj->GetLayerIdx());
}

void CKirbyFinalCutterEnd::Exit()
{
    // Material ����
    PLAYERFSM->GetCurWeapon()->MeshRender()->SetMaterial(nullptr, 0);

    // ����, ����, �̵� Unlock
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockJump();

    // ���� ����
    PLAYERFSM->SetInvincible(false);
}
