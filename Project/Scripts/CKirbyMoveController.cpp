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
    , m_Speed(2.f)
    , m_RotSpeed(20.f)
    , m_JumpPower(1.f)
    , m_RayCastDist(2.f)
    , m_Gravity(-10.f)
    , m_bMoveLock(false)
    , m_bDirLock(false)
    , m_bJumpLock(false)
    , m_HoveringLimitHeight(15.f)
    , m_HoveringHeight(0.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, "Speed");
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
    , m_Speed(15.f)
    , m_RotSpeed(_Origin.m_RotSpeed)
    , m_JumpPower(1.f)
    , m_RayCastDist(2.f)
    , m_Gravity(-20.f)
    , m_bMoveLock(false)
    , m_bDirLock(false)
    , m_bJumpLock(false)
    , m_HoveringLimitHeight(_Origin.m_HoveringLimitHeight)
    , m_HoveringHeight(0.f)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, "Speed");
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

    m_MoveDir = XMVectorAdd(XMVectorScale(Front, m_Input.z), XMVectorScale(Right, m_Input.x));
    m_MoveDir.Normalize();
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

    // 180�� �� ��� ����ó��
    if (m_CurDir.Dot(m_TowardDir) == (-m_CurDir.Length() * m_TowardDir.Length()))
    {
        m_TowardDir.x += 0.1f;
        m_TowardDir.Normalize();
    }

    // ���麸���� �̿��ؼ� ��ü�� ���ο� ������ ����
    Transform()->SetDirection(Vector3::SmoothStep(m_CurDir, m_TowardDir, DT * m_RotSpeed));

    // ���� ����
    // Transform()->SetDirection(m_TowardDir);
}

void CKirbyMoveController::Move()
{
    bool bGrounded = CharacterController()->IsGrounded();

    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Transform()->GetWorldPos(), Vec3(0.f, -1.f, 0.f), 2.f, {L"Ground"});

    if (!bGrounded)
    {
        bGrounded = nullptr != Hit.pCollisionObj;
    }

    // ���� ���� ���¸� Velocity Y�� �ʱ�ȭ
    if (bGrounded && m_MoveVelocity.y < 0)
    {
        m_MoveVelocity.y = 0.f;
    }

    m_MoveVelocity.x = m_MoveDir.x * m_Speed;
    m_MoveVelocity.z = m_MoveDir.z * m_Speed;

    // ����
    if (KEY_TAP(KEY_JUMP) && bGrounded && m_bJumpLock == false)
    {
        m_MoveVelocity.y += std::sqrt(m_JumpPower * -3.f * m_Gravity);
    }

    // ������ �� ����
    m_MoveVelocity += m_ForceVelocity;
    m_ForceVelocity = Vec3();

    if (PLAYERFSM->IsHovering() && m_HoveringHeight > m_HoveringLimitHeight && m_MoveVelocity.y > 0.f)
    {
        m_MoveVelocity.y = 0.f;
    }

    // �߷� ����
    m_MoveVelocity.y += m_Gravity * DT;

    // �ִ� �ӵ� ����
    //if (PLAYERFSM->IsHovering() && m_MoveVelocity.y > 20.f)
    //{
    //    m_MoveVelocity.y = 20.f;
    //}

    // ������ ����
    Vec3 Diff;

    if (m_bMoveLock == false)
    {
        Diff = CharacterController()->Move(m_MoveVelocity * DT);
    }

    if (PLAYERFSM->IsHovering())
    {
        m_HoveringHeight += Diff.y;
    }
}

// USAGE: ĳ������ ���Ը� �����ϰ� ���� �ְ� ���� �� (�ӵ� ���� X)
// RESULT: ���� ���� ���� ���ӵ��� ����� �ӵ��� ����
// - ���ӵ� ���� �̿� �Լ����� ó���Ѵٰ� ����, �������� ���� ���ؼ��� ó��
void CKirbyMoveController::AddForce(Vec3 _Force, AddForceType _Type)
{
    switch (_Type)
    {
    case AddForceType::Acceleration: {
        // ���ӵ� ���
        Vec3 Accel = _Force / 1.f; // A = F/M;

        if (CharacterController()->IsGrounded())
        {
            Accel.y = 0.f;
        }

        // �ӵ� ���
        m_ForceVelocity += Accel * DT;

        // ������ ���
        // if (_Force == Vec3() && CharacterController()->IsGrounded() && m_MoveVelocity.x != 0.f)
        //{
        //    // ������ ũ��
        //    float fFriction = -m_MoveVelocity.x / fabs(m_MoveVelocity.x);
        //    fFriction *= m_fFrictionScale;

        //    // ���� ������ ũ��
        //    float fFrictionAccel = (fFriction / 1.f) * DT;
        //    if (fabs(m_MoveVelocity.x) < fabs(fFrictionAccel))
        //    {
        //        m_MoveVelocity.x = 0.f;
        //    }
        //    else
        //        m_MoveVelocity.x += fFrictionAccel;
        //}
    }
    break;
    case AddForceType::VelocityChange: {
        m_ForceVelocity += _Force;
    }
    break;
    }
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
