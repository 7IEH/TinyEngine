#include "pch.h"
#include "CElfilisG_Teleport.h"
#include "CElfilisFSM.h"
#include <Engine\CAssetMgr.h>
#include <Engine\CPrefab.h>

CElfilisG_Teleport::CElfilisG_Teleport()
    : m_BeforeObj(nullptr)
    , m_BeforeEffect(nullptr)
    , m_AfterEffect(nullptr)
    , m_EffectSpeed(400.f)
{
    m_Effect = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Effect_ElfilisTeleport.pref", L"prefab\\Effect_ElfilisTeleport.pref");
}

CElfilisG_Teleport::~CElfilisG_Teleport()
{
}

void CElfilisG_Teleport::tick()
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

void CElfilisG_Teleport::Enter_Step()
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
        m_AfterPos.x = GetRandomfloat(-MapSizeRadius, MapSizeRadius);
        m_AfterPos.y = 0.f;
        m_AfterPos.z = GetRandomfloat(-MapSizeRadius, MapSizeRadius);
        m_AfterPos = ELFFSM->GetMapFloorOffset() + m_AfterPos.Normalize() * GetRandomfloat(0.f, MapSizeRadius);

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

void CElfilisG_Teleport::Exit_Step()
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
            m_BeforeObj = nullptr;
        }
        if (m_BeforeEffect)
        {
            GamePlayStatic::DestroyGameObject(m_BeforeEffect);
            m_BeforeEffect = nullptr;
        }
        if (m_AfterEffect)
        {
            GamePlayStatic::DestroyGameObject(m_AfterEffect);
            m_AfterEffect = nullptr;
        }
    }
    break;
    }
}

void CElfilisG_Teleport::Start()
{
    // Teleport (After 1 tick : Spawn ���� ��ٸ�)
    Vec3 Dir = PLAYER->Transform()->GetWorldPos() - m_AfterPos;
    Dir.y = 0.f;
    Dir.Normalize();

    GetOwner()->Transform()->Slerp(Dir, 1.f);
    GetOwner()->Transform()->SetWorldPos(m_AfterPos);

    ChangeStep(StateStep::End);
}

void CElfilisG_Teleport::End()
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
        ElfilisStateGroup NextState = ELFFSM->FindNextStateGroup();
        ELFFSM->ChangeStateGroup(NextState);
    }
}