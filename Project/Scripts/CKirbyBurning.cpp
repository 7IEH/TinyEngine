#include "pch.h"
#include "CKirbyBurning.h"

CKirbyBurning::CKirbyBurning()
    : m_SaveRotSpeed(0.f)
    , m_Duration(0.5f)
    , m_Acc(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurning::~CKirbyBurning()
{
}

void CKirbyBurning::tick()
{
    m_Acc += DT;

    // ��ü �۶��̵� �ð��� ���� FSM�� Gliding �ð��� �����ش�.
    PLAYERFSM->AddGlidingTime(DT);

    // �߷°� ����
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 1.f)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }

    // ���罺����Ʈ�� ���ѽð��� �Ѿ�ų�, ���� ����� ��� BurningEnd
    if (m_Acc > m_Duration)
    {
        if (KEY_RELEASED(KEY_ATK))
        {
            // MeshData �����ֱ�
            PLAYER->MeshRender()->SetMeshData(CAssetMgr::GetInst()->Load<CMeshData>(L"meshdata\\Kirby.mdat", L"meshdata\\Kirby.mdat"));

            PLAYER->MeshRender()->SetMaterial(nullptr, 0);
            PLAYER->MeshRender()->SetMaterial(nullptr, 2);
            PLAYER->MeshRender()->SetMaterial(nullptr, 4);
            PLAYER->MeshRender()->SetMaterial(nullptr, 6);
            PLAYER->MeshRender()->SetMaterial(nullptr, 7);
            PLAYER->MeshRender()->SetMaterial(nullptr, 8);

            // ���� �ٽ� ���̰��ϱ�
            Ptr<CMaterial> HatMat = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\KiryDragonHat_DragonFireC.mtrl");
            PLAYERFSM->GetCurHat()->MeshRender()->SetMaterial(HatMat, 0);
            ChangeState(L"JUMP_FALL");
        }
        else
        {
            ChangeState(L"BURNING_END");
        }
    }
}

void CKirbyBurning::Enter()
{
    m_Acc = 0.f;

    // �ִϸ��̼� ���
    PLAYER->Animator()->Play(ANIMPREFIX("Burning"), false, false, 1.5f);

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

void CKirbyBurning::Exit()
{
    // Movement
    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);

    PLAYERCTRL->SetSpeed(m_SaveSpeed);

    //  ���� ����
    PLAYERFSM->SetInvincible(false);
}
