#include "pch.h"
#include "CKirbyMoveController.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CPhysicsMgr.h>

CKirbyMoveController::CKirbyMoveController()
    : CScript(KIRBYMOVECONTROLLER)
    , m_Input{0.f, 0.f, 0.f}
    , m_CurDir{}
    , m_TowardDir{}
    , m_MoveDir{0.f, 0.f, 0.f}
    , m_GroundNormal{0.f, 1.f, 0.f}
    , m_ForceDirInfos{}
    , m_MoveVelocity{}
    , m_Speed(10.f)
    , m_RotSpeed(50.f)
    , m_JumpPower(1000.f)
    , m_RayCastDist(2.f)
    , m_Gravity(-50.f)
    , m_bMoveLock(false)
    , m_bDirLock(false)
    , m_bJumpLock(false)
    , m_bJump(false)
    , m_bGuard(false)
    , m_HoveringLimitHeight(7.5f)
    , m_HoveringMinSpeed(-5.f)
    , m_AddVelocity{0.f, 0.f, 0.f}
    , m_Friction(0.f)
    , m_bPurse(PurseType::NONE)
    , m_PurseAccType(PurseAccType::AccUp)
    , m_PurseAcc(0.f)
    , m_PurseAirTime(0.f)
    , m_PurseContTime(0.f)
    , m_PurseMinSpeed(0.f)
    , m_PurseSpeed(0.f)
    , m_PurseScale(0.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, "Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_RotSpeed, "Rotation Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_JumpPower, "JumpPower");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Gravity, "Gravity");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_HoveringLimitHeight, "HoveringLimit");
}

CKirbyMoveController::CKirbyMoveController(const CKirbyMoveController& _Origin)
    : CScript(_Origin)
    , m_CurDir{}
    , m_TowardDir{}
    , m_Input{0.f, 0.f, 0.f}
    , m_ForceDirInfos{}
    , m_MoveDir{0.f, 0.f, 0.f}
    , m_GroundNormal{0.f, 1.f, 0.f}
    , m_MoveVelocity{}
    , m_Speed(10.f)
    , m_RotSpeed(_Origin.m_RotSpeed)
    , m_JumpPower(1000.f)
    , m_RayCastDist(2.f)
    , m_Gravity(-50.f)
    , m_bMoveLock(false)
    , m_bDirLock(false)
    , m_bJumpLock(false)
    , m_bJump(false)
    , m_bGuard(false)
    , m_HoveringLimitHeight(_Origin.m_HoveringLimitHeight)
    , m_HoveringMinSpeed(_Origin.m_HoveringMinSpeed)
    , m_AddVelocity{0.f, 0.f, 0.f}
    , m_Friction(_Origin.m_Friction)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, "Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_RotSpeed, "Rotation Speed");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_JumpPower, "JumpPower");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Gravity, "Gravity");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_HoveringLimitHeight, "HoveringLimit");
}

CKirbyMoveController::~CKirbyMoveController()
{
}

void CKirbyMoveController::begin()
{
    // �÷��̾�� ĳ���� ��Ʈ�ѷ� ������Ʈ�� ������ �����ϵ��� �߰�
    if (nullptr == CharacterController())
    {
        GetOwner()->AddComponent(new CCharacterController);
    }

    m_CurDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    m_TowardDir = m_CurDir;

    m_Speed = 10.f;
    m_RotSpeed = 50.f;
    m_JumpPower = 10.f;
    m_Gravity = -20.f;
}

void CKirbyMoveController::tick()
{
    // Key �Է� Ȯ��
    Input();

    // ĳ������ ������� ���ϱ�
    SetDir();

    // �̵�
    Move();

    // ǥ�� ���� (����)
    // SurfaceAlignment();

    // Debug : ���� ǥ��
    GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::FRONT), 30.f, Vec3(0.f, 1.f, 0.f), true);
    // GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::RIGHT), 30.f, Vec3(1.f, 0.f, 0.f), true);
    // GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::UP), 30.f, Vec3(0.f, 0.f, 1.f), true);
}

void CKirbyMoveController::Input()
{
    // Ű �Է� ����
    m_Input = {0.f, 0.f, 0.f};
    // ������ ���� ����(World��ǥ��)
    m_MoveDir = {0.f, 0.f, 0.f};
    // Ŀ�� ���� ����
    m_CurDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);

    // �Է� �ޱ�
    // �� ��
    if (KEY_TAP(KEY::UP) || KEY_PRESSED(KEY::UP))
    {
        m_Input += Vec3(0.f, 0.f, 1.f);
    }
    if (KEY_TAP(KEY::DOWN) || KEY_PRESSED(KEY::DOWN))
    {
        m_Input += Vec3(0.f, 0.f, -1.f);
    }

    // �� ��
    if (KEY_TAP(KEY::RIGHT) || KEY_PRESSED(KEY::RIGHT))
    {
        m_Input += Vec3(1.f, 0.f, 0.f);
    }
    if (KEY_TAP(KEY::LEFT) || KEY_PRESSED(KEY::LEFT))
    {
        m_Input += Vec3(-1.f, 0.f, 0.f);
    }

    // m_MoveDir ���
    // Main Camera ��������
    CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();

    if (nullptr == pCam)
        return;

    Vec3 Front = pCam->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    Vec3 Right = pCam->Transform()->GetWorldDir(DIR_TYPE::RIGHT);

    // y�� �̵��� ������Ʈ�� ���ؼ� ó��
    Front.y = 0.f;
    Right.y = 0.f;

    if (!m_bMoveLock)
    {
        m_MoveDir = XMVectorAdd(XMVectorScale(Front, m_Input.z), XMVectorScale(Right, m_Input.x));
        m_MoveDir.Normalize();
    }
}

void CKirbyMoveController::SetDir()
{
    // ���� ���� ������ �ִٸ� �ش� ������ �ݿ�
    if (!m_ForceDirInfos.empty())
    {
        UINT Priority = (UINT)ForceDirType::END;
        Vec3 ForceDir = m_TowardDir;

        for (size_t i = 0; i < m_ForceDirInfos.size(); ++i)
        {
            if ((UINT)m_ForceDirInfos[i].Type < Priority)
            {

                Priority = (UINT)m_ForceDirInfos[i].Type;
                ForceDir = m_ForceDirInfos[i].Dir;
            }
        }

        ForceDir.y = 0.f;
        ForceDir.Normalize();

        // ���� �켱������ ���� ������ ����
        Transform()->SetDirection(ForceDir);
        m_TowardDir = ForceDir;

        m_ForceDirInfos.clear();
        return;
    }

    // Ű �Է½� �ٶ������ ������ ����
    if (m_Input.Length() > 0.f && m_bDirLock == false)
    {
        m_TowardDir = m_MoveDir;
    }

    // ���� ������ Lock �ɸ� ��� �ٶ������ ������ ���� �������� ����
    if (m_bDirLock)
    {
        m_TowardDir = m_CurDir;
    }

    // �ٶ������ ����� ���� ������ ���ݴ��� ��� ����ó��
    if (m_CurDir.Dot(m_TowardDir) - (m_CurDir.Length() * m_TowardDir.Length() * -1.f) < 0.0001f)
    {
        Vec3 Rot = Transform()->GetRelativeRotation();
        Rot.y += XM_PI / 180.f;
        Transform()->SetRelativeRotation(Rot);
    }

    // ���麸���� �̿��ؼ� ��ü�� ���ο� ������ ����
    Transform()->SetDirection(Vector3::SmoothStep(m_CurDir, m_TowardDir, DT * m_RotSpeed));

    // ���� ����
    // Transform()->SetDirection(m_TowardDir);
}

void CKirbyMoveController::Move()
{
    // ���ӵ� �ʱ�ȭ
    m_Accel = {0.f, 0.f, 0.f};

    bool bGrounded = CharacterController()->IsGrounded();

    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Transform()->GetWorldPos(), Vec3(0.f, -1.f, 0.f), 2.f, {L"Ground"});

    if (!bGrounded)
    {
        bGrounded = nullptr != Hit.pCollisionObj;
    }

    // =========================
    // Velocity ���
    // =========================

    // ���� ���� �̵��ӵ� ���
    // Guard�ÿ��� ������������ �̵��ӵ��� ���� ���ӽ�Ŵ
    if (m_bGuard)
    {
        m_Accel.x = -m_MoveVelocity.x * m_Friction;
        m_Accel.z = -m_MoveVelocity.z * m_Friction;

        m_MoveVelocity.x += m_Accel.x;
        m_MoveVelocity.z += m_Accel.z;
    }
    else
    {
        m_MoveVelocity.x = m_MoveDir.x * m_Speed;
        m_MoveVelocity.z = m_MoveDir.z * m_Speed;
    }

    // Jump
    if (m_bJump && !m_bJumpLock)
    {
        m_bJump = false;
        m_MoveVelocity.y = m_JumpPower;
    }

    // �߷� ����
    m_Accel.y += m_Gravity;

    // ���� ���� �̵��ӵ� ���
    m_MoveVelocity.y += m_Accel.y * DT;

    if (m_bPurse != PurseType::NONE)
    {
        CalcPurse();
    }

    // AddVelocity ����
    if (m_AddVelocity.Length() != 0.f)
    {
        m_MoveVelocity += m_AddVelocity;
        m_AddVelocity = {0.f, 0.f, 0.f};
    }

    // =========================
    // Velocity Min / Max Ȯ��
    // =========================
    // ���� ���� ���¸� Velocity Y�� �ʱ�ȭ
    if (bGrounded && m_MoveVelocity.y < 0)
    {
        m_MoveVelocity.y = 0.f;
    }

    if (PLAYERFSM->IsHovering())
    {
        // falling
        if (m_MoveVelocity.y <= m_HoveringMinSpeed)
        {
            m_MoveVelocity.y = m_HoveringMinSpeed;
        }

        // check limit height
        Hit = CPhysicsMgr::GetInst()->RayCast(Transform()->GetWorldPos(), Vec3(0.f, -1.f, 0.f), m_HoveringLimitHeight, {L"Layer 1"});
        
        if (Hit.pCollisionObj == nullptr && m_MoveVelocity.y > 0.f)
        {
            //@TODO ���̾��̸�
            m_MoveVelocity.y = 0.f;
        }
    }

    // =========================
    // ������ ����
    // =========================
    CharacterController()->Move(m_MoveVelocity * GetOwner()->Transform()->GetRelativeScale() * DT);
}

void CKirbyMoveController::SurfaceAlignment()
{
    bool bGrounded = CharacterController()->IsGrounded();
    float GravityVelue = CPhysicsMgr::GetInst()->GetGravity().y;

    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Transform()->GetWorldPos(), Vec3(0.f, -1.f, 0.f), 2.f, {L"Ground"});

    // Rotate Character
    if (bGrounded)
    {
        Vec3 SurfaceNormal = Hit.Normal;
        SurfaceNormal.Normalize();

        Vec3 ProjectedTowardDir = m_TowardDir - SurfaceNormal * (m_TowardDir.Dot(SurfaceNormal));
        ProjectedTowardDir.Normalize();

        Vec3 Right = SurfaceNormal.Cross(ProjectedTowardDir);
        Right.Normalize();

        Vec3 Front = Right.Cross(SurfaceNormal);
        Front.Normalize();

        // ȸ�� ��� ����
        XMMATRIX rotationMatrix =
            XMMATRIX(XMVectorSet(Right.x, Right.y, Right.z, 0.0f), XMVectorSet(SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z, 0.0f),
                     XMVectorSet(Front.x, Front.y, Front.z, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

        XMMATRIX rotationMatrix2 = DirectX::XMMatrixLookToLH(XMVectorZero(), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));

        float yaw, pitch, roll;
        pitch = asinf(-rotationMatrix.r[2].m128_f32[1]);
        yaw = atan2f(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
        roll = atan2f(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);

        Transform()->SetRelativeRotation({pitch, yaw, roll});
    }
    else
    {
        Transform()->SetDirection(m_TowardDir);

        Vec3 Movement = {0.f, -50.f, 0.f};

        CharacterController()->Move(Movement * DT);
    }
}

void CKirbyMoveController::PurseY(float _PurseSpeed, float _PurseAirTime, float _PurseContTime, float _PurseMinSpeed, float _PurseScale)
{
    m_bPurse = PurseType::Up;
    m_PurseAccType = PurseAccType::AccUp;
    float m_PurseAcc = 0.f;

    m_PurseSpeed = _PurseSpeed;
    m_PurseAirTime = _PurseAirTime;
    m_PurseContTime = _PurseContTime;
    m_PurseMinSpeed = _PurseMinSpeed;
    m_PurseScale = _PurseScale;
}

void CKirbyMoveController::CalcPurse()
{
    if (m_bPurse == PurseType::NONE)
    {
        return;
    }

    float m_PurseCurSpeed = 0.f;
    m_PurseAcc += DT;

    if (m_bPurse == PurseType::Zero)
    {
        m_MoveVelocity.y = 0;

        if (m_PurseAcc > m_PurseAirTime)
        {
            m_bPurse = PurseType::Down;
            m_PurseAcc -= m_PurseAirTime;
        }
    }
    else
    {
        // speed up
        if (m_PurseAccType == PurseAccType::AccUp)
        {
            m_PurseCurSpeed = pow(2, m_PurseAcc * m_PurseScale);

            if (m_PurseCurSpeed >= m_PurseSpeed)
            {
                m_PurseCurSpeed = m_PurseSpeed;
                m_PurseAccType = PurseAccType::AccZero;
                m_PurseAcc = 0.f;
            }
        }
        // const velocity
        else if (m_PurseAccType == PurseAccType::AccZero)
        {
            if (m_PurseAcc > m_PurseContTime)
            {
                m_PurseAccType = PurseAccType::AccDown;
                m_PurseAcc -= m_PurseContTime;
            }
        }
        // speed down
        else if (m_PurseAccType == PurseAccType::AccDown)
        {
            m_PurseCurSpeed = m_PurseSpeed - pow(2, m_PurseAcc * m_PurseScale);

            if (m_PurseCurSpeed <= m_PurseMinSpeed)
            {
                m_MoveVelocity.y = m_PurseMinSpeed;
                m_PurseAccType = PurseAccType::AccUp;
                m_PurseAcc = 0.f;

                if (m_bPurse == PurseType::Up)
                {
                    m_bPurse = PurseType::Zero;
                }
                else if (m_bPurse == PurseType::Down)
                {
                    m_bPurse = PurseType::NONE;
                }
            }
        }

        if (m_bPurse == PurseType::Up)
        {
            m_MoveVelocity.y = m_PurseCurSpeed;
        }
        else if (m_bPurse == PurseType::Down)
        {
            m_MoveVelocity.y = -m_PurseCurSpeed;
        }
    }
}

void CKirbyMoveController::OnControllerColliderHit(ControllerColliderHit Hit)
{
    // CPhysicsMgr::RayCast(Transform()->GetWorldPos(), Vec3(0.f,-1.f,0.f), 100.f, )
    int a = 0;
}

void CKirbyMoveController::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Speed, 1, sizeof(float), _File);
    fwrite(&m_Gravity, 1, sizeof(float), _File);
    fwrite(&m_JumpPower, 1, sizeof(float), _File);
}

void CKirbyMoveController::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Speed, 1, sizeof(float), _File);
    fread(&m_Gravity, 1, sizeof(float), _File);
    fread(&m_JumpPower, 1, sizeof(float), _File);
}
