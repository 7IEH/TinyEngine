#include "pch.h"
#include "CNoddyScript.h"

CNoddyScript::CNoddyScript()
    : CMonsterUnitScript(NODDYSCRIPT)
    , m_State(NODDY_STATE::Sleep)
    , m_PassedTime(0.f)
    , m_SnotBubble(nullptr)
{
}

CNoddyScript::CNoddyScript(const CNoddyScript& origin)
    : CMonsterUnitScript(origin)
    , m_State(origin.m_State)
    , m_PassedTime(0.f)
    , m_SnotBubble(nullptr)
{
}

CNoddyScript::~CNoddyScript()
{
}

void CNoddyScript::begin()
{
    SetSnotBubble();

    ChangeState(m_State);
}

void CNoddyScript::tick()
{
    // Damage Proc
    ClearHitDir();
    m_PrevInfo = m_CurInfo;

    float DamageAmount = DamageProc();
    if (DamageAmount > 0.f)
    {
        m_CurInfo.HP -= DamageAmount;
        ChangeState(NODDY_STATE::Damage);
    }

    // SnotBubble
    if (nullptr != m_SnotBubble)
    {
        if (m_State == NODDY_STATE::SleepStart || m_State == NODDY_STATE::Sleep)
        {
            m_SnotBubble->SetActive(true);
        }
        else
        {
            m_SnotBubble->SetActive(false);
        }
    }
    else
    {
        SetSnotBubble();
    }

    // FSM
    switch (m_State)
    {
    case NODDY_STATE::Damage:
        Damage();
        break;
    case NODDY_STATE::Fall:
        Fall();
        break;
    case NODDY_STATE::FallSleep:
        FallSleep();
        break;
    case NODDY_STATE::Landing:
        Landing();
        break;
    case NODDY_STATE::LandingSleep:
        LandingSleep();
        break;
    case NODDY_STATE::LookAround:
        LookAround();
        break;
    case NODDY_STATE::Move:
        Move();
        break;
    case NODDY_STATE::Sleep:
        Sleep();
        break;
    case NODDY_STATE::SleepStart:
        SleepStart();
        break;
    case NODDY_STATE::Wait:
        Wait();
        break;
    case NODDY_STATE::Wakeup:
        Wakeup();
        break;
    }
}

void CNoddyScript::ChangeState(NODDY_STATE _NextState)
{
    ExitState();
    m_State = _NextState;
    m_PassedTime = 0.f;
    EnterState();
}

void CNoddyScript::SetSnotBubble()
{
    for (CGameObject* pChild : GetOwner()->GetChildObject())
    {
        if (L"SleepSnotBubble" == pChild->GetName())
        {
            m_SnotBubble = pChild;
            break;
        }
    }
}

void CNoddyScript::EnterState()
{
    // FSM
    switch (m_State)
    {
    case NODDY_STATE::Damage: {
        // 피격 방향으로 회전
        Transform()->Slerp(GetHitDir(), 1.f);

        // 피격 방향으로 Impulse
        Vec3 Impulse = GetHitDir();
        Impulse.Normalize();
        Impulse *= 5.f;
        Rigidbody()->AddForce(Impulse, ForceMode::Impulse);

        Animator()->Play(ANIMPREFIX("Damage"), false);

        Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(0);
        pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\Noddy\\ChNoddy.01.png"));
    }
    break;
    case NODDY_STATE::Fall: {
        Animator()->Play(ANIMPREFIX("Fall"));
    }
    break;
    case NODDY_STATE::FallSleep: {
        Animator()->Play(ANIMPREFIX("FallSleep"));
    }
    break;
    case NODDY_STATE::Landing: {
        Animator()->Play(ANIMPREFIX("Landing"), false);
    }
    break;
    case NODDY_STATE::LandingSleep: {
        Animator()->Play(ANIMPREFIX("LandingSleep"), false);
    }
    break;
    case NODDY_STATE::LookAround: {
        Animator()->Play(ANIMPREFIX("LookAround"), false, false, 1.f, 0.5);
    }
    break;
    case NODDY_STATE::Move: {
        Animator()->Play(ANIMPREFIX("Move"));
    }
    break;
    case NODDY_STATE::Sleep: {
        Animator()->Play(ANIMPREFIX("Sleep"), true, false, 2.5f, 0.5);
        if (nullptr != m_SnotBubble)
        {
            m_SnotBubble->Animator()->Play(ANIMPREFIX("Main"));
        }
    }
    break;
    case NODDY_STATE::SleepStart: {
        Animator()->Play(ANIMPREFIX("SleepStart"), false);
        Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(0);
        pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\Noddy\\ChNoddy.00.png"));
        if (nullptr != m_SnotBubble)
        {
            m_SnotBubble->Animator()->Play(ANIMPREFIX("BubbleStart"), false);
        }
    }
    break;
    case NODDY_STATE::Wait: {
        Animator()->Play(ANIMPREFIX("Wait"));
        Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(0);
        pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\Noddy\\ChNoddy.02.png"));
    }
    break;
    case NODDY_STATE::Wakeup: {
        Animator()->Play(ANIMPREFIX("Wakeup"), false, false, 1.f);
        Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(0);
        pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\Noddy\\ChNoddy.02.png"));
    }
    break;
    }
}

void CNoddyScript::ExitState()
{
    // FSM
    switch (m_State)
    {
    case NODDY_STATE::Damage: {
        if (m_CurInfo.HP <= 0.f)
        {
            GamePlayStatic::DestroyGameObject(GetOwner());
        }
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));
        Rigidbody()->SetAngularVelocity(Vec3(0.f, 0.f, 0.f));
    }
    break;
    case NODDY_STATE::Fall: {
    }
    break;
    case NODDY_STATE::FallSleep: {
    }
    break;
    case NODDY_STATE::Landing: {
    }
    break;
    case NODDY_STATE::LandingSleep: {
    }
    break;
    case NODDY_STATE::LookAround: {
    }
    break;
    case NODDY_STATE::Move: {
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));
    }
    break;
    case NODDY_STATE::Sleep: {
    }
    break;
    case NODDY_STATE::SleepStart: {
    }
    break;
    case NODDY_STATE::Wait: {
    }
    break;
    case NODDY_STATE::Wakeup: {
    }
    break;
    }
}

void CNoddyScript::Damage()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::LookAround);
    }
}

void CNoddyScript::Fall()
{
    if (IsGround())
    {
        ChangeState(NODDY_STATE::Landing);
    }
}

void CNoddyScript::FallSleep()
{
    if (IsGround())
    {
        ChangeState(NODDY_STATE::LandingSleep);
    }
}

void CNoddyScript::Landing()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::Wait);
    }
}

void CNoddyScript::LandingSleep()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::Wait);
    }
}

void CNoddyScript::LookAround()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::SleepStart);
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::Fall);
    }
}

void CNoddyScript::Move()
{
    RigidbodyMove();

    m_PassedTime += DT;
    if (m_PassedTime >= 2.f)
    {
        ChangeState(NODDY_STATE::Wait);
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::Fall);
    }
}

void CNoddyScript::Sleep()
{
    if (nullptr != GetTarget())
    {
        m_PassedTime += DT;

        if (m_PassedTime >= 5.f)
        {
            ChangeState(NODDY_STATE::Wakeup);
        }
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::FallSleep);
    }
}

void CNoddyScript::SleepStart()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::Sleep);
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::FallSleep);
    }
}

void CNoddyScript::Wait()
{
    m_PassedTime += DT;
    if (m_PassedTime >= 1.f)
    {
        ChangeState(NODDY_STATE::LookAround);
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::Fall);
    }
}

void CNoddyScript::Wakeup()
{
    if (Animator()->IsFinish())
    {
        ChangeState(NODDY_STATE::Move);
    }

    if (!IsGround())
    {
        ChangeState(NODDY_STATE::Fall);
    }
}

void CNoddyScript::OnCollisionEnter(CCollider* _OtherCollider)
{
}

void CNoddyScript::OnCollisionStay(CCollider* _OtherCollider)
{
}

void CNoddyScript::OnCollisionExit(CCollider* _OtherCollider)
{
}

UINT CNoddyScript::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CMonsterUnitScript::SaveToLevelFile(_File);

    return MemoryByte;
}

UINT CNoddyScript::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CMonsterUnitScript::LoadFromLevelFile(_File);

    return MemoryByte;
}
