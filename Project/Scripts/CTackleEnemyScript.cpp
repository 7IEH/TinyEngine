#include "pch.h"
#include "CTackleEnemyScript.h"

#include "CPlayerMgr.h"
#include "CUIDashEffectScript.h"

CTackleEnemyScript::CTackleEnemyScript()
    : CMonsterUnitScript(TACKLEENEMYSCRIPT)
    , m_eState(TackleEnemyState::Idle)
    , m_fSpeed(10.f)
    , m_fMaxSpeed(10.f)
    , m_fRushLerp(0.9f)
    , m_fRushSpeedLerp(0.5f)
    , m_fAccTime(0.f)
    , m_fWaitTime(2.5f)
    , m_fThreshHoldRushLerp(0.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fMaxSpeed, "Rush Max Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Rush Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fRushLerp, "Rush Lerp");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fRushSpeedLerp, "Rush Speed Lerp");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fThreshHoldRushLerp, "Threshold Rush Lerp");
}

CTackleEnemyScript::CTackleEnemyScript(const CTackleEnemyScript& Origin)
    : CMonsterUnitScript(Origin)
    , m_eState(TackleEnemyState::Idle)
    , m_fSpeed(Origin.m_fSpeed)
    , m_fMaxSpeed(Origin.m_fMaxSpeed)
    , m_fRushLerp(Origin.m_fRushLerp)
    , m_fRushSpeedLerp(Origin.m_fRushSpeedLerp)
    , m_fThreshHoldRushLerp(Origin.m_fThreshHoldRushLerp)
    , m_fAccTime(0.f)
    , m_fWaitTime(1.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fMaxSpeed, "Rush Max Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Rush Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fRushLerp, "Rush Lerp");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fRushSpeedLerp, "Rush Speed Lerp");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fThreshHoldRushLerp, "Threshold Rush Lerp");
}

CTackleEnemyScript::~CTackleEnemyScript()
{
}

void CTackleEnemyScript::begin()
{
    CMonsterUnitScript::begin();

    GetOwner()->MeshRender()->GetDynamicMaterial(0);

    ChangeState(TackleEnemyState::Idle);

    SetInfo(UnitInfo{67.f, 67.f, 70.f, 8.f, 1.f, 5.f});
    m_fMaxSpeed = m_fSpeed = 21.5f;
    m_fRushLerp = 0.8f;
    m_fRushSpeedLerp = 0.2f;
    m_fThreshHoldRushLerp = 0.1f;

    if (nullptr == GetOwner()->GetChildObject(L"Effect_DashEffect"))
    {
        m_pDashEffect = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Effect_DashEffect.pref", L"prefab\\Effect_DashEffect.pref")->Instantiate();
        m_pDashEffect->Transform()->SetAbsolute(false);
        m_pDashEffect->Transform()->SetLocalScale(Vec3(2.2f, 2.2f, 2.2f));
        m_pDashEffect->Transform()->SetLocalPos(Vec3(0.f, 2.f, 1.f));
        m_pDashEffect->Transform()->SetLocalRotation(Vec3(XM_PIDIV2, 0.f, 0.f));
        m_pDashEffect->GetScript<CUIDashEffectScript>()->SetDashTime(1.f);
        m_pDashEffect->GetScript<CUIDashEffectScript>()->SetRatioSpeed(0.01f);
        m_pDashEffect->GetScript<CUIDashEffectScript>()->SetTextureSpeed(4.5f);
        m_pDashEffect->GetScript<CUIDashEffectScript>()->SetBackGroundAlpha(0.15f);

        m_pDashEffect->GetScript<CUIDashEffectScript>()->ChangeState(DashEffectState::Stop);

        GamePlayStatic::AddChildObject(GetOwner(), m_pDashEffect);
    }

    SetRayCast(50.f);

    SetResistTime(2.f);
}

void CTackleEnemyScript::tick()
{
    CMonsterUnitScript::tick();

    FSM();

    CheckDamage();

    if (TackleEnemyState::Eaten != m_eState && GetResistState())
    {
        ChangeState(TackleEnemyState::Eaten);
    }
}

UINT CTackleEnemyScript::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CMonsterUnitScript::SaveToLevelFile(_File);
    fwrite(&m_fMaxSpeed, sizeof(float), 1, _File);
    fwrite(&m_fSpeed, sizeof(float), 1, _File);
    fwrite(&m_fRushLerp, sizeof(float), 1, _File);
    fwrite(&m_fRushSpeedLerp, sizeof(float), 1, _File);
    fwrite(&m_fThreshHoldRushLerp, sizeof(float), 1, _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);

    return MemoryByte;
}

UINT CTackleEnemyScript::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CMonsterUnitScript::LoadFromLevelFile(_File);
    fread(&m_fMaxSpeed, sizeof(float), 1, _File);
    fread(&m_fSpeed, sizeof(float), 1, _File);
    fread(&m_fRushLerp, sizeof(float), 1, _File);
    fread(&m_fRushSpeedLerp, sizeof(float), 1, _File);
    fread(&m_fThreshHoldRushLerp, sizeof(float), 1, _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);

    return MemoryByte;
}

void CTackleEnemyScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (TackleEnemyState::Eaten == m_eState)
        return;

    CGameObject* pObj = _OtherCollider->GetOwner();

    Vec3 vDir = PLAYER->Transform()->GetWorldPos() - Transform()->GetWorldPos();
    UnitHit hitInfo = {DAMAGE_TYPE::NORMAL, vDir.Normalize(), GetCurInfo().ATK, 0.f, 0.f};
    UINT Layer = _OtherCollider->GetOwner()->GetLayerIdx();

    if (Layer == LAYER_PLAYER_TRIGGER && L"Body Collider" == pObj->GetName())
    {
        pObj->GetParent()->GetScript<CUnitScript>()->GetDamage(hitInfo);
        BodyAttackSound();
    }
}

/////////////////////////// FUNC //////////////////////////////////
// 1. EnterState
// 2. FSM
// 3. ExitState
// 4. ChangeState
// 5. CheckDamage
// 6. TrackDir
// 7. ApplyDir

void CTackleEnemyScript::EnterState(TackleEnemyState _state)
{
    switch (_state)
    {
    case TackleEnemyState::Idle: {
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png"));

        Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, true);
        Animator()->Play(ANIMPREFIX("Wait"), true, false, 1.5f);
    }
    break;
    case TackleEnemyState::Find: {
        FindSound();
        Animator()->Play(ANIMPREFIX("Find"), false, false, 1.5f);
    }
    break;
    case TackleEnemyState::AttackPrev: {
        DashEffectOn();
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.01.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.01.png"));

        Animator()->Play(ANIMPREFIX("RunStart"), false, false, 1.5f);
    }
    break;
    case TackleEnemyState::Attack: {
        Animator()->Play(ANIMPREFIX("Run"), true, false, 1.5f);
    }
    break;
    case TackleEnemyState::AttackAfter: {
        DashEffectOff();
        Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, true);
        Animator()->Play(ANIMPREFIX("Brake"), false, false, 1.5f);
    }
    break;
    case TackleEnemyState::AttackAfter2: {
        DashEffectOff();
        Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, true);
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));
        Animator()->Play(ANIMPREFIX("Brake"), false);
    }
    break;
    case TackleEnemyState::Fall: {
        DashEffectOff();
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png"));

        Animator()->Play(ANIMPREFIX("Fall"));
    }
    break;
    case TackleEnemyState::Landing: {
        DashEffectOff();
        LandingSmokeEffect(Vec3(0.f, -45.f, 0.f));
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png"));
        Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, true);
        Animator()->Play(ANIMPREFIX("Landing"), false);
    }
    break;
    case TackleEnemyState::Wait: {
        DashEffectOff();
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.00.png"));
        Animator()->Play(ANIMPREFIX("Wait"), true, false, 1.5f);
    }
    break;
    case TackleEnemyState::Damage: {
        DashEffectOff();
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png"));
        Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, false);

        SetSparkle(true);

        Vec3 vFollowDir = (PLAYER->Transform()->GetWorldPos() - Transform()->GetWorldPos()).Normalize();
        vFollowDir.y = 0.f;

        Transform()->SetDirection(vFollowDir);

        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));

        Vec3 vHitDir = GetOwner()->GetScript<CUnitScript>()->GetHitDir();
        float fForceX = 0.f;
        float fForceY = 0.f;
        if (GetCurInfo().HP <= 0.1f)
        {
            fForceX = 110.f;
            fForceY = 500.f;
            vHitDir.y = 1.5f;
        }
        else
        {
            fForceX = 100.f;
            fForceY = 200.f;
            vHitDir.y = 1.f;
        }

        Rigidbody()->AddForce(vHitDir.Normalize() * Vec2(fForceX, fForceY), ForceMode::Impulse);

        Animator()->Play(ANIMPREFIX("Damage"), false, false, 2.f);
    }
    break;
    case TackleEnemyState::Eaten: {
        DashEffectOff();
        GetOwner()->MeshRender()->GetMaterial(0)->SetTexParam(
            TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png",
                                                        L"fbx\\Characters\\Monster\\TackleEnemy\\TackleEnemyEye.02.png"));
        Animator()->Play(ANIMPREFIX("Damage"));
    }
    break;
    case TackleEnemyState::Death:
        DashEffectOff();
        SpawnDeadSmokeEffect();
        break;
    default:
        break;
    }
}

void CTackleEnemyScript::FSM()
{
    switch (m_eState)
    {
    case TackleEnemyState::Idle: {
        Idle();
    }
    break;

    case TackleEnemyState::Find: {
        Find();
    }
    break;
    case TackleEnemyState::AttackPrev: {
        AttackPrev();
    }
    break;
    case TackleEnemyState::Attack: {
        Attack();
    }
    break;
    case TackleEnemyState::AttackAfter: {
        AttackAfter();
    }
    break;
    case TackleEnemyState::AttackAfter2: {
        AttackAfter2();
    }
    break;
    case TackleEnemyState::Wait: {
        Wait();
    }
    break;
    case TackleEnemyState::Fall: {
        Fall();
    }
    break;
    case TackleEnemyState::Landing: {
        Landing();
    }
    break;
    case TackleEnemyState::Damage: {
        Damage();
    }
    break;
    case TackleEnemyState::Eaten: {
        Eaten();
    }
    break;
    case TackleEnemyState::Death: {
        Death();
    }
    break;
    default:
        break;
    }
}

void CTackleEnemyScript::ExitState(TackleEnemyState _state)
{
    switch (_state)
    {
    case TackleEnemyState::Idle: {
    }
    break;
    case TackleEnemyState::Find: {
    }
    break;
    case TackleEnemyState::AttackPrev: {
    }
    break;
    case TackleEnemyState::Attack: {
    }
    break;
    case TackleEnemyState::AttackAfter: {
        m_fSpeed = m_fMaxSpeed;
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));
    }
    break;
    case TackleEnemyState::AttackAfter2: {
        m_fSpeed = m_fMaxSpeed;
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));
    }
    break;
    case TackleEnemyState::Damage: {
    }
    break;
    case TackleEnemyState::Death:
        break;
    case TackleEnemyState::Wait: {
        m_fAccTime = 0.f;
    }
    break;
    default:
        break;
    }
}

void CTackleEnemyScript::ChangeState(TackleEnemyState _state)
{
    if (TackleEnemyState::Death == m_eState)
        return;

    ExitState(m_eState);
    m_eState = _state;
    EnterState(m_eState);
}

void CTackleEnemyScript::CheckDamage()
{
    if (IsGetDamage())
    {
        ChangeState(TackleEnemyState::Damage);
    }
}

void CTackleEnemyScript::ApplyDir(Vec3 _vFront, bool _flag)
{
    Vec3 _vPos = Transform()->GetLocalPos();

    // Caculate Quaternion Tracking Dir
    Quat _vOriginQuat = Transform()->GetWorldQuaternion();
    Vec3 _vTrackingDir = TrackDir(_vPos);
    _vTrackingDir.y = 0.f;

    // Quternion Fix
    Vec3 _vUp = Vec3(0.f, 0.f, -1.f) == _vFront ? Vec3(0.f, -1.f, 0.f) : Vec3(0.f, 1.f, 0.f);

    Quat _vTrackQuat = Quat::LookRotation(-_vTrackingDir, _vUp);

    float vRadian = Transform()->GetWorldDir(DIR_TYPE::FRONT).Dot((PLAYER->Transform()->GetWorldPos() - Transform()->GetWorldPos()).Normalize());

    // 90도 이상 틀어질 경우 lerp가 확도는걸 감안함
    if (vRadian <= 0.2f)
    {
        ChangeState(TackleEnemyState::AttackAfter2);
        return;
    }

    if (_flag)
        _vTrackQuat = Quat::Slerp(_vOriginQuat, _vTrackQuat, m_fRushLerp * DT);

    Transform()->SetWorldRotation(_vTrackQuat);
}

void CTackleEnemyScript::DashEffectOn()
{
    if (nullptr != m_pDashEffect)
    {
        m_pDashEffect->GetScript<CUIDashEffectScript>()->ChangeState(DashEffectState::Start);
    }
}

void CTackleEnemyScript::DashEffectOff()
{
    if (nullptr != m_pDashEffect)
    {
        m_pDashEffect->GetScript<CUIDashEffectScript>()->ChangeState(DashEffectState::Stop);
    }
}

Vec3 CTackleEnemyScript::TrackDir(Vec3 _vPos)
{
    return (PLAYER->GetComponent<CTransform>()->GetLocalPos() - _vPos).Normalize();
}

///////////////////////////// FIND FSM ///////////////////////////////////////
// -> FSM STATES ->
// 1. Idle
// 2. Find
// 3. AttackPrev
// 4. Attack
// 5. AttackAfter
// 6. AttackAfter2
// 7. Fall
// 8. Ladning
// 9. Damage
// 10. Eaten
// 11. Wait
// 12. Death

#pragma region IDLE
void CTackleEnemyScript::Idle()
{
    if (IsGround())
    {
        if (nullptr != GetTarget())
        {
            Rigidbody()->SetFreezeRotation(AXIS_TYPE::Y, false);
            ChangeState(TackleEnemyState::Find);
        }
    }
    else
    {
        ChangeState(TackleEnemyState::Fall);
    }
}
#pragma endregion

#pragma region FIND
void CTackleEnemyScript::Find()
{
    Animator()->IsFinish() ? ChangeState(TackleEnemyState::AttackPrev) : void();
}
#pragma endregion

#pragma region ATTACKPREV
void CTackleEnemyScript::AttackPrev()
{
    RotatingToTarget();
    Animator()->IsFinish() ? ChangeState(TackleEnemyState::Attack) : void();
}
#pragma endregion

#pragma region ATTACK
void CTackleEnemyScript::Attack()
{
    Vec3 vDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    vDir.y = 0.f;
    ApplyDir(vDir, true);

    m_fSpeed = Lerp(m_fSpeed, 0.f, m_fRushSpeedLerp * DT);
    Rigidbody()->SetVelocity(vDir * m_fSpeed + Vec3(0.f, -9.8f, 0.f));

    if (m_fSpeed <= 6.f)
    {
        ChangeState(TackleEnemyState::AttackAfter);
    }
}
#pragma endregion

#pragma region ATTACKAFTER
void CTackleEnemyScript::AttackAfter()
{
    if (Animator()->IsFinish())
    {
        ChangeState(TackleEnemyState::Wait);
    }
}
#pragma endregion

#pragma region ATTACKAFTER2
void CTackleEnemyScript::AttackAfter2()
{
    Vec3 vDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    vDir.y = 0.f;

    m_fSpeed = Lerp(m_fSpeed, 0.f, 6.f * DT);

    Rigidbody()->AddForce(Transform()->GetWorldDir(DIR_TYPE::FRONT) * m_fSpeed * DT);

    if (m_fSpeed <= 0.3f)
    {
        ChangeState(TackleEnemyState::Wait);
    }
}
#pragma endregion

#pragma region FALL
void CTackleEnemyScript::Fall()
{
    if (IsGround())
    {
        ChangeState(TackleEnemyState::Landing);
    }
}
#pragma endregion

#pragma region LADNING
void CTackleEnemyScript::Landing()
{
    if (Animator()->IsFinish())
    {
        ChangeState(TackleEnemyState::Idle);
    }
}
#pragma endregion

#pragma region DAMAGE
void CTackleEnemyScript::Damage()
{
    if (GetCurInfo().HP <= 0.1f)
    {
        ChangeState(TackleEnemyState::Death);
    }
    else
    {
        Animator()->IsFinish() ? ChangeState(TackleEnemyState::Fall) : void();
    }
}
#pragma endregion

#pragma region EATEN
void CTackleEnemyScript::Eaten()
{
    if (!GetResistState())
    {
        ChangeState(TackleEnemyState::Fall);
    }
}
#pragma endregion

#pragma region WAIT
void CTackleEnemyScript::Wait()
{
    m_fAccTime += DT;
    if (m_fAccTime >= m_fWaitTime)
    {
        ChangeState(TackleEnemyState::Idle);
    }
}
#pragma endregion

#pragma region DEATH
void CTackleEnemyScript::Death()
{
    if (Animator()->IsFinish())
    {
        SpawnDeadEffect(0);
        GamePlayStatic::DestroyGameObject(GetOwner());
    }
}
#pragma endregion