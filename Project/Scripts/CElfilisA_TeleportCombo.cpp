#include "pch.h"
#include "CElfilisA_TeleportCombo.h"
#include "CElfilisFSM.h"
#include "CCameraController.h"
#include <Engine\CAssetMgr.h>
#include <Engine\CPrefab.h>

CElfilisA_TeleportCombo::CElfilisA_TeleportCombo()
    : m_BeforeObj(nullptr)
    , m_BeforeEffect(nullptr)
    , m_AfterEffect(nullptr)
    , m_EffectSpeed(200.f)
    , m_ComboLevel(0)
{
    m_Effect = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Effect_ElfilisTeleport.pref", L"prefab\\Effect_ElfilisTeleport.pref");
}

CElfilisA_TeleportCombo::~CElfilisA_TeleportCombo()
{
}

void CElfilisA_TeleportCombo::tick()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        Start();
    }
    break;
    case StateStep::End: {
        End();
    }
    break;
    }
}

void CElfilisA_TeleportCombo::Enter()
{
    m_ComboLevel = 0;
    m_Step = StateStep::Start;

    // @CAMERA : Teleport Camera Set

    Enter_Step();
}

void CElfilisA_TeleportCombo::Exit()
{
    Exit_Step();

    GetOwner()->Animator()->SetPlay(true);

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

    // Camera : �� Ÿ��
    CAMERACTRL->SetElfilisTwoTarget();
}

void CElfilisA_TeleportCombo::Enter_Step()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        GetOwner()->Animator()->Play(ANIMPREFIX("Wait"));
        GetOwner()->Animator()->SetPlay(false);

        //@Effect �Ϻκи� �׸��� ���̴� �ۼ� �ʿ�

        // copy object
        m_BeforeObj = new CGameObject;
        m_BeforeObj->AddComponent(GetOwner()->Transform()->Clone());
        m_BeforeObj->AddComponent(GetOwner()->MeshRender()->Clone());
        m_BeforeObj->AddComponent(GetOwner()->Animator()->Clone());
        m_BeforeObj->SetName(L"Effect_ElfilisTelport Body");
        GamePlayStatic::SpawnGameObject(m_BeforeObj, LAYER_MONSTER);

        // teleport
        float MapSizeRadius = ELFFSM->GetMapSizeRadius();
        Vec3 MapFloorOffset = ELFFSM->GetMapFloorOffset();

        if (m_ComboLevel == 0 || m_ComboLevel == 5)
        {
            // air to ground
            m_AfterPos = PLAYER->Transform()->GetWorldPos() + PLAYER->Transform()->GetWorldDir(DIR_TYPE::FRONT) * 100.f;
        }
        else
        {
            // ground teleport combo
            m_AfterPos = GetOwner()->Transform()->GetWorldPos();
            m_AfterPos.x += GetRandomfloat(-MapSizeRadius * 2.f, MapSizeRadius * 2.f);
            m_AfterPos.z += GetRandomfloat(-MapSizeRadius * 2.f, MapSizeRadius * 2.f);
        }

        if (m_AfterPos.x < 0)
        {
            if (m_AfterPos.x < MapSizeRadius * -1.f + MapFloorOffset.x)
            {
                m_AfterPos.x = MapSizeRadius * -1.f;
            }
        }
        else
        {
            if (m_AfterPos.x > MapSizeRadius + MapFloorOffset.x)
            {
                m_AfterPos.x = MapSizeRadius;
            }
        }

        if (m_AfterPos.z < 0)
        {
            if (m_AfterPos.z < MapSizeRadius * -1.f + MapFloorOffset.z)
            {
                m_AfterPos.z = MapSizeRadius * -1.f;
            }
        }
        else
        {
            if (m_AfterPos.z > MapSizeRadius + MapFloorOffset.z)
            {
                m_AfterPos.z = MapSizeRadius;
            }
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
    break;
    case StateStep::End:
        break;
    }
}

void CElfilisA_TeleportCombo::Exit_Step()
{
    switch (m_Step)
    {
    case StateStep::Start:
        break;
    case StateStep::End: {
        GetOwner()->Animator()->SetPlay(true);

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
    }
}

void CElfilisA_TeleportCombo::Start()
{
    // Teleport (After 1 tick : Spawn ���� ��ٸ�)
    Vec3 Dir = PLAYER->Transform()->GetWorldPos() - m_AfterPos;
    Dir.y = 0.f;
    Dir.Normalize();

    GetOwner()->Transform()->Slerp(Dir, 1.f);
    GetOwner()->Transform()->SetWorldPos(m_AfterPos);

    ChangeStep(StateStep::End);
}

void CElfilisA_TeleportCombo::End()
{
    // move effect
    Vec3 Pos = m_BeforeEffect->Transform()->GetWorldPos();
    Pos.y -= m_EffectSpeed * DT;
    m_BeforeEffect->Transform()->SetWorldPos(Pos);

    Pos = m_AfterEffect->Transform()->GetWorldPos();
    Pos.y -= m_EffectSpeed * DT;
    m_AfterEffect->Transform()->SetWorldPos(Pos);

    if (Pos.y <= 0.f)
    {
        if (m_ComboLevel < 5)
        {
            m_ComboLevel++;
            ChangeStep(StateStep::Start);
        }
        else
        {
            ElfilisStateGroup NextState = ELFFSM->FindNextStateGroup();
            ELFFSM->ChangeStateGroup(NextState);
        }
    }
}