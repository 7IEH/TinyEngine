#include "pch.h"
#include "CKirbyMoveController.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyUnitScript.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CPhysicsMgr.h>

#include "CCameraController.h"

CKirbyMoveController::CKirbyMoveController()
    : CScript(KIRBYMOVECONTROLLER)
    , m_Input{0.f, 0.f, 0.f}
    , m_CurDir{}
    , m_TowardDir{}
    , m_MoveDir{0.f, 0.f, 0.f}
    , m_ForceDirInfos{}
    , m_MoveVelocity{}
    , m_Speed(10.f)
    , m_MaxSpeed(12.f)
    , m_RotSpeed(10.f)
    , m_JumpPower(10.f)
    , m_Gravity(-20.f)
    , m_bMoveLock(false)
    , m_bInputLock(false)
    , m_bDirLock(false)
    , m_bJumpLock(false)
    , m_bJump(false)
    , m_bActiveFriction(false)
    , m_bForwardMode(false)
    , m_bLimitFallSpeed(false)
    , m_HoveringLimitHeight(500.f)
    , m_HoveringHeight(0.f)
    , m_AddVelocity{0.f, 0.f, 0.f}
    , m_Friction(0.f)
    , m_HoveringMinSpeed(-5.f)
    , m_RayHit{}
    , m_MaxFallSpeed(-15.f)
{
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
    m_Speed = PLAYERUNIT->GetInitInfo().Speed;
    m_RotSpeed = PLAYERUNIT->GetInitInfo().RotationSpeed;
    m_JumpPower = PLAYERUNIT->GetInitInfo().JumpPower;
    m_Gravity = -20.f;
    m_bGround = false;
    m_bLimitFallSpeed = false;
    m_CheckPointPos = Transform()->GetWorldPos();
}

void CKirbyMoveController::tick()
{ 
    // @Test
    if (KEY_TAP(KEY::G))
    {
        CAMERACTRL->FixedView(false);
    }




    // Key �Է� Ȯ��
    Input();

    // ĳ������ ������� ���ϱ�
    SetDir();

    // RayCast
    RayGround();

    // �̵�
    Move();

    // @Debug : ���� ǥ��
    //GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::FRONT), 100.f, Vec3(1.f, 0.f, 0.f), true);
    //GamePlayStatic::DrawDebugLine(CharacterController()->GetFootPos(), Transform()->GetWorldDir(DIR_TYPE::RIGHT), 100.f, Vec3(0.f, 0.f, 1.f), true);
    // GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::UP), 100.f, Vec3(0.f, 0.f, 1.f), true);
}

void CKirbyMoveController::Input()
{
    // Ű �Է� ����
    m_Input = {0.f, 0.f, 0.f};

    m_InputWorld = {0.f, 0.f, 0.f};
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

    if (m_bInputLock)
    {
        m_Input = Vector3::Zero;
    }

    m_InputWorld = XMVectorAdd(XMVectorScale(Front, m_Input.z), XMVectorScale(Right, m_Input.x));
    m_InputWorld.Normalize();

    if (!m_bMoveLock)
    {
        m_MoveDir = m_InputWorld;
    }
}

void CKirbyMoveController::RayGround()
{
    Vec3 RayStart = Transform()->GetWorldPos() + Vec3(0.f, 1.f, 0.f);
    static vector<wstring> vecCollision{L"World Static", L"World Dynamic"};
    m_RayHit = CPhysicsMgr::GetInst()->RayCast(RayStart, Vec3(0.f, -1.f, 0.f), m_HoveringLimitHeight, vecCollision);

    // Grund ����
    m_bGround = CharacterController()->IsGrounded();

    if (m_bGround)
    {
        if (m_RayHit.pCollisionObj == nullptr)
        {
            m_bGround = false;
        }
        else if (m_RayHit.Distance > 5.f)
        {
            m_bGround = false;
        }
    }
}

void CKirbyMoveController::SetDir()
{
    // ���� ���� ������ �ִٸ� �ش� ������ �ݿ�
    if (!m_ForceDirInfos.empty())
    {
        UINT Priority = (UINT)ForceDirType::END;
        Vec3 ForceDir = m_TowardDir;
        bool Immediate;

        for (size_t i = 0; i < m_ForceDirInfos.size(); ++i)
        {
            if ((UINT)m_ForceDirInfos[i].Type < Priority)
            {

                Priority = (UINT)m_ForceDirInfos[i].Type;
                ForceDir = m_ForceDirInfos[i].Dir;
                Immediate = m_ForceDirInfos[i].Immediate;
            }
        }

        ForceDir.y = 0.f;
        ForceDir.Normalize();

        // ���� �켱������ ���� ������ ����
        m_TowardDir = ForceDir;

        // ������ ��� �����ؾ� �Ѵٸ�
        if (Immediate)
            Transform()->SetDirection(m_TowardDir);

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

    // ������ ���� ���� ���� ����
    Transform()->Slerp(m_TowardDir, DT * m_RotSpeed);
}

void CKirbyMoveController::Move()
{
    // ���ӵ� �ʱ�ȭ
    m_Accel = {0.f, 0.f, 0.f};

    // =========================
    // Velocity ���
    // =========================

    // ���� ���� �̵��ӵ� ���
    // Guard�ÿ��� ������������ �̵��ӵ��� ���� ���ӽ�Ŵ

    if (m_bActiveFriction)
    {
        m_Accel.x = -m_MoveVelocity.x * m_Friction;
        m_Accel.z = -m_MoveVelocity.z * m_Friction;

        m_MoveVelocity.x += m_Accel.x * DT;
        m_MoveVelocity.z += m_Accel.z * DT;
    }
    else if (m_bForwardMode)
    {
        m_CurDir.y = 0.f;
        m_CurDir.Normalize();

        m_MoveVelocity.x = m_CurDir.x * m_Speed;
        m_MoveVelocity.z = m_CurDir.z * m_Speed;
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

    // ���� ���� ���¸� Velocity Y�� �ʱ�ȭ
    if (m_bGround && m_MoveVelocity.y < 0)
    {
        m_MoveVelocity.y = 0.f;
    }

    if (PLAYERFSM->IsHovering() && m_HoveringHeight > m_HoveringLimitHeight && m_MoveVelocity.y > 0.f)
    {
        m_MoveVelocity.y = 0.f;
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
    if (m_bGround && m_MoveVelocity.y < 0)
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
        if (m_RayHit.pCollisionObj == nullptr && m_MoveVelocity.y > 0.f)
        {
            m_MoveVelocity.y = 0.f;
        }
    }

    // ���� MaxSpeed ����
    Vec3 HorizontalVel = {m_MoveVelocity.x, 0.f, m_MoveVelocity.z};
    if (HorizontalVel.Length() > m_MaxSpeed)
    {
        HorizontalVel = HorizontalVel.Normalize() * m_MaxSpeed;
        m_MoveVelocity.x = HorizontalVel.x;
        m_MoveVelocity.z = HorizontalVel.z;
    }

    // MaxFallSpeed ����
    if (m_MoveVelocity.y < m_MaxFallSpeed  && m_bLimitFallSpeed) 
    {
        m_MoveVelocity.y = m_MaxFallSpeed;
    }

    // =========================
    // ������ ����
    // =========================
    CharacterController()->Move(m_MoveVelocity * DT);
}




UINT CKirbyMoveController::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CKirbyMoveController::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}