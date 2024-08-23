#include "pch.h"
#include "CElfilisG_NormalAtkTeleportL.h"
#include "CElfilisFSM.h"

CElfilisG_NormalAtkTeleportL::CElfilisG_NormalAtkTeleportL()
    : m_BeforeObj(nullptr)
    , m_BeforeEffect(nullptr)
    , m_AfterEffect(nullptr)
    , m_EffectSpeed(400.f)
    , m_bComboSuccess(false)
    , m_bFrmEnter(true)
{
    m_Effect = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Effect_ElfilisTeleport.pref");
}

CElfilisG_NormalAtkTeleportL::~CElfilisG_NormalAtkTeleportL()
{
}

void CElfilisG_NormalAtkTeleportL::tick()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        Start();
    }
    break;
    case StateStep::Wait: {
        StartWait();
    }
    break;
    case StateStep::StartEnd: {
        StartEnd();
    }
    break;
    case StateStep::Progress: {
        Progress();
    }
    break;
    case StateStep::End: {
        End();
    }
    break;
    }
}

void CElfilisG_NormalAtkTeleportL::Enter()
{
    if (ELFFSM->GetComboLevel() == 0)
    {
        m_Step = StateStep::Start;
    }
    else
    {
        m_Step = StateStep::Wait;
    }

    Enter_Step();
}

void CElfilisG_NormalAtkTeleportL::Exit()
{
    Exit_Step();

    if (m_Step < StateStep::Progress)
    {
        ELFFSM->ClearComboLevel();
        ELFFSM->OffWeaponTrigger();
    }
}

void CElfilisG_NormalAtkTeleportL::Enter_Step()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeftStart"), false, false, 1.f, 0.5f);
    }
    break;
    case StateStep::Wait: {
        GetOwner()->Animator()->Play(ANIMPREFIX("ChainRL"), false, false, 1.f, 0.5f);
    }
    break;
    case StateStep::StartEnd: {
        GetOwner()->Rigidbody()->SetKinematic(true);
        m_bFrmEnter = true;
        SpawnTeleport();
    }
    break;
    case StateStep::Progress: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeft"), false, false, 1.f);
        ELFFSM->OnWeaponTrigger();
    }
    break;
    case StateStep::End: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeftEnd"), false, false, 1.f);
    }
    break;
    }
}

void CElfilisG_NormalAtkTeleportL::Exit_Step()
{
    switch (m_Step)
    {
    case StateStep::Start:
        break;
    case StateStep::Wait:
        break;
    case StateStep::StartEnd: {
        GetOwner()->Rigidbody()->SetKinematic(false);

        if (m_BeforeObj)
        {
            GamePlayStatic::DestroyGameObject(m_BeforeObj);
        }
        if (m_BeforeEffect)
        {
            GamePlayStatic::DestroyGameObject(m_BeforeEffect);
        }
        if (m_AfterEffect)
        {
            GamePlayStatic::DestroyGameObject(m_AfterEffect);
        }
    }
    break;
    case StateStep::Progress: {
        if (m_bComboSuccess)
        {
            ELFFSM->AddComboLevel();
        }
        else
        {
            ELFFSM->ClearComboLevel();
        }
        ELFFSM->OffWeaponTrigger();
    }
    break;
    case StateStep::End:
        break;
    }
}

void CElfilisG_NormalAtkTeleportL::Start()
{
    // rotate
    RotateToPlayer();

    if (GetOwner()->Animator()->IsFinish())
    {
        ChangeStep(StateStep::Wait);
    }
}

void CElfilisG_NormalAtkTeleportL::StartWait()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ChangeStep(StateStep::StartEnd);
    }
}

void CElfilisG_NormalAtkTeleportL::StartEnd()
{
    if (m_bFrmEnter)
    {
        // Teleport (After 1 tick : Spawn ���� ��ٸ�)
        Vec3 Dir = PLAYER->Transform()->GetWorldPos() - m_AfterPos;
        Dir.y = 0.f;
        Dir.Normalize();

        GetOwner()->Transform()->Slerp(Dir, 1.f);
        GetOwner()->Transform()->SetWorldPos(m_AfterPos);

        m_bFrmEnter = false;
    }
    else
    {
        // move effect
        Vec3 Pos = m_BeforeEffect->Transform()->GetWorldPos();
        float ChangeHeight = Pos.y - m_EffectSpeed * DT;
        Pos.y = ChangeHeight;
        m_BeforeEffect->Transform()->SetWorldPos(Pos);

        Pos = m_AfterEffect->Transform()->GetWorldPos();
        Pos.y = ChangeHeight;
        m_AfterEffect->Transform()->SetWorldPos(Pos);

        if (ChangeHeight <= 0.f)
        {
            ChangeStep(StateStep::Progress);
        }
    }
}

void CElfilisG_NormalAtkTeleportL::Progress()
{
    // resize Hitbox
    CBoxCollider* pHitbox = ELFFSM->GetHitbox();
    if (pHitbox && !GetOwner()->Animator()->IsChainging())
    {
        if (GetOwner()->Animator()->GetClipFrameIndex() < 13)
        {
            pHitbox->GetOwner()->SetActive(true);
            pHitbox->Transform()->SetLocalPos(Vec3(0.71f, 1.f, 2.64f));
            pHitbox->Transform()->SetLocalRotation(Vec3(0.f));
            pHitbox->Transform()->SetLocalScale(Vec3(5.96f, 1.f, 4.81f));
        }
        else
        {
            pHitbox->GetOwner()->SetActive(false);
        }
    }

    if (GetOwner()->Animator()->IsFinish())
    {
        // Check Combo
        float Rand = 0.f;
        if (ELFFSM->GetPhase() == 1)
        {
            Rand = GetRandomfloat(1.f, 100.f);
        }

        if (Rand <= 90)
        {
            m_bComboSuccess = true;

            if (ELFFSM->IsNearPlayer())
            {
                ELFFSM->ChangeStateGroup(ElfilisStateGroup::GroundAtkNear, L"GROUND_ATK_NORMAL_R");
            }
            else
            {
                ELFFSM->ChangeStateGroup(ElfilisStateGroup::GroundAtkNear, L"GROUND_ATK_NORMALTELEPORT_R");
            }
        }
        else
        {
            ChangeStep(StateStep::End);
        }
    }
}

void CElfilisG_NormalAtkTeleportL::End()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ElfilisStateGroup NextState = ELFFSM->FindNextStateGroup();

        if (NextState == ELFFSM->GetCurStateGroup())
        {
            ELFFSM->RepeatState(L"GROUND_ATK_NORMAL");
        }
        else
        {
            ELFFSM->ChangeStateGroup(NextState);
        }
    }
}

void CElfilisG_NormalAtkTeleportL::SpawnTeleport()
{
    //@Effect �Ϻκи� �׸��� ���̴� �ۼ� �ʿ�

    // copy object
    m_BeforeObj = new CGameObject;
    m_BeforeObj->AddComponent(GetOwner()->Transform()->Clone());
    m_BeforeObj->AddComponent(GetOwner()->MeshRender()->Clone());
    m_BeforeObj->AddComponent(GetOwner()->Animator()->Clone());
    m_BeforeObj->SetName(L"Effect_ElfilisTelport Body");
    GamePlayStatic::SpawnGameObject(m_BeforeObj, LAYER_MONSTER);

    // get teleport pos
    Vec3 Dist = GetOwner()->Transform()->GetWorldPos() - PLAYER->Transform()->GetWorldPos();
    Dist.y = 0.f;
    Dist.Normalize();
    Dist *= ELFFSM->GetNearDist() * (GetRandomfloat(30.f, 100.f) / 100.f);
    m_AfterPos = PLAYER->Transform()->GetWorldPos() + Dist;
    m_AfterPos.y = 0.f;

    // map limit
    float MapSizeRadius = ELFFSM->GetMapSizeRadius();
    Vec3 MapFloorOffset = ELFFSM->GetMapFloorOffset();
    Vec3 NewPosMapDist = m_AfterPos - MapFloorOffset;

    if (NewPosMapDist.Length() > MapSizeRadius)
    {
        Vec3 Dir = NewPosMapDist.Normalize();
        m_AfterPos = MapFloorOffset + Dir * MapSizeRadius;
    }

    //@Effect �ڷ����� ����Ʈ
    Vec3 Pos = GetOwner()->Transform()->GetWorldPos();
    Pos.y += 100.f;
    m_BeforeEffect = m_Effect->Instantiate();
    m_BeforeEffect->Transform()->SetWorldPos(Pos);
    GamePlayStatic::SpawnGameObject(m_BeforeEffect, LAYER_EFFECT);

    Pos = m_AfterPos;
    Pos.y += 100.f;
    m_AfterEffect = m_Effect->Instantiate();
    m_AfterEffect->Transform()->SetWorldPos(Pos);
    GamePlayStatic::SpawnGameObject(m_AfterEffect, LAYER_EFFECT);
}
