#include "pch.h"
#include "CKirbyBurningEnd.h"

CKirbyBurningEnd::CKirbyBurningEnd()
    : m_SaveRotSpeed(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurningEnd::~CKirbyBurningEnd()
{
}
void CKirbyBurningEnd::tick()
{
    // ��ü �۶��̵� �ð��� ���� FSM�� Gliding �ð��� �����ش�.
    PLAYERFSM->AddGlidingTime(DT);

    // �߷°� ����
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 10.f)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }

    float CurGlidingTime = PLAYERFSM->GetCurGlidingTime();

    // �۶��̵� �ð��� �����ų� ����Ű�� ������ ������
    if (CurGlidingTime > PLAYERFSM->GetGlidingDuration() || (KEY_RELEASED(KEY_ATK)||KEY_NONE(KEY_ATK)))
    {
        ChangeState(L"JUMP_FALL");
    }
}

void CKirbyBurningEnd::Enter()
{
    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("BurningEnd"), false, false, 1.5f);

    // Movement
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetForwardMode(true);
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(0.5f);

    m_SaveSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(13.f);

    //  ���� ����
    PLAYERFSM->SetInvincible(true);
}

void CKirbyBurningEnd::Exit()
{
    // Movement
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);

    PLAYERCTRL->SetGravity(-20.f);
    PLAYERCTRL->SetSpeed(m_SaveSpeed);

    // MeshData �����ֱ�
    PLAYER->MeshRender()->SetMeshData(CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\Kirby.mdat", L"meshdata\\Kirby.mdat"));

    PLAYER->MeshRender()->SetMaterial(nullptr, 0);
    PLAYER->MeshRender()->SetMaterial(nullptr, 2);
    PLAYER->MeshRender()->SetMaterial(nullptr, 4);
    PLAYER->MeshRender()->SetMaterial(nullptr, 6);
    PLAYER->MeshRender()->SetMaterial(nullptr, 7);
    PLAYER->MeshRender()->SetMaterial(nullptr, 8);

    // ���� �ٽ� ���̰��ϱ�
    Ptr<CMaterial> HatMat =
        CAssetMgr::GetInst()->Load<CMaterial>(L"material\\KiryDragonHat_DragonFireC.mtrl", L"material\\KiryDragonHat_DragonFireC.mtrl");
    PLAYERFSM->GetCurHat()->MeshRender()->SetMaterial(HatMat, 0);

    //  ���� ����
    PLAYERFSM->SetInvincible(false);
}

