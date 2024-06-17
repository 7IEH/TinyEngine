#include "pch.h"
#include "CKirbyMoveController.h"

#include <Engine/CRenderMgr.h>

CKirbyMoveController::CKirbyMoveController()
    : CScript(KIRBYMOVECONTROLLER)
    , m_Dir{}
    , m_LookDir{}
    , m_Move(false)
{
}

CKirbyMoveController::CKirbyMoveController(const CKirbyMoveController& _Origin)
    : CScript(_Origin)
    , m_Dir{}
    , m_LookDir{}
    , m_Move(false)
{
}

CKirbyMoveController::~CKirbyMoveController()
{
}

void CKirbyMoveController::begin()
{
}

void CKirbyMoveController::tick()
{
    SetDir();

    if (m_Move)
    {
        Vec3 Dir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Vec3 Pos = Transform()->GetRelativePos();

        Transform()->SetRelativePos(Pos + Dir * DT * 20);
    }
}

void CKirbyMoveController::SetDir()
{
    // Main Camera Front ��������
    CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();

    if (nullptr == pCam)
        return;

    Vec3 CamRotation = pCam->Transform()->GetRelativeRotation();

    Vec3 CamDir = pCam->Transform()->GetLocalDir(DIR_TYPE::FRONT);
    CamDir.z = 0.f;
    CamDir.Normalize();

    // Ű �Է� Ȯ��
    Vec3 InputDir = Vec3(0.f, 0.f, 0.f);

    // �� ��
    if (KEY_TAP(KEY::W) || KEY_PRESSED(KEY::W))
    {
        InputDir += Vec3(0.f, 0.f, 1.f);
    }
    if (KEY_TAP(KEY::S) || KEY_PRESSED(KEY::S))
    {
        InputDir += Vec3(0.f, 0.f, -1.f);
    }

    // �� ��
    if (KEY_TAP(KEY::D) || KEY_PRESSED(KEY::D))
    {
        InputDir += Vec3(1.f, 0.f, 0.f);
    }
    if (KEY_TAP(KEY::A) || KEY_PRESSED(KEY::A))
    {
        InputDir += Vec3(-1.f, 0.f, 0.f);
    }

    // Ű �Է��� �ִٸ� �ٶ������ ������ �ٽ� �����Ѵ�
    if (InputDir.Length() != 0.f)
    {
        InputDir.Normalize();

        // Camera Y�� ȸ�� ��� �����
        Matrix matRotY = XMMatrixIdentity();
        matRotY = XMMatrixRotationY(CamRotation.y);

        // �Է� ���⿡ ī�޶� y�� ȸ���� ���� World ���� �������� ��ȯ
        m_LookDir = XMVector3TransformNormal(InputDir, matRotY);

        m_Move = true;
    }
    else
    {
        m_Move = false;
    }

    // ���麸���� �̿��ؼ� ��ü�� ���ο� ������ ����
    Transform()->SetDirection(Vector3::SmoothStep(Transform()->GetWorldDir(DIR_TYPE::FRONT), m_LookDir, DT * 50.f));

    GamePlayStatic::DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldDir(DIR_TYPE::FRONT), 50.f, Vec3(0.f, 1.f, 0.f), false);
}

void CKirbyMoveController::SaveToLevelFile(FILE* _File)
{
}

void CKirbyMoveController::LoadFromLevelFile(FILE* _File)
{
}
