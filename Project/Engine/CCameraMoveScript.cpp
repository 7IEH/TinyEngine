#include "pch.h"
#include "CCameraMoveScript.h"
#include "CEditorMgr.h"
#include "CLevelEditor.h"
#include "CDevice.h"
#include "CLevelMgr.h"

CCameraMoveScript::CCameraMoveScript()
    : m_bFocus(false)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::tick()
{
    // Projection Change
    if (KEY_TAP(KEY::P))
    {
        if (Camera()->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC)
            Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
        else
            Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);

        Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
    }

    // Selected Obejct Focus
    if (KEY_TAP(KEY::F))
        m_bFocus = true;

    if (m_bFocus)
    {
        if (Camera()->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC)
            MoveFocusOrthographic();
        else
            MoveFocusPerspective();
    }

    // Level �������� ��� Viewportâ������ ī�޶� �̵� ����
    CLevelEditor* LevelDditor = dynamic_cast<CLevelEditor*>(CEditorMgr::GetInst()->GetCurEditor());
    if (nullptr != LevelDditor)
    {
        if (!LevelDditor->IsViewportHovered())
            return;
    }

    // Move
    if (Camera()->GetProjType() == PROJ_TYPE::ORTHOGRAPHIC)
        MoveOrthographic();
    else
        MovePerspective();

    // Wheel �ʱ�ȭ
    CKeyMgr::GetInst()->SetMouseWheel(0);
}

void CCameraMoveScript::MoveOrthographic()
{
    // Drag
    if (KEY_PRESSED(KEY::RBTN))
    {
        Vec2 vDrag = CKeyMgr::GetInst()->GetMouseDrag();
        float scale = Camera()->GetScale();

        Vec2 ViewportSize = CEditorMgr::GetInst()->GetViewportSize();
        Vec2 GetRenderResolution = CDevice::GetInst()->GetRenderResolution();

        float xRatio = 0.f;
        float yRatio = 0.f;

        if (ViewportSize.x == 0 || ViewportSize.y == 0 || nullptr == CEditorMgr::GetInst()->GetCurEditor())
        {
            xRatio = 1.f;
            yRatio = 1.f;
        }
        else
        {
            xRatio = GetRenderResolution.x / ViewportSize.x;
            yRatio = GetRenderResolution.y / ViewportSize.y;
        }

        // �巡���� �ȼ� ��ġ����ŭ �̵�
        // ����Ʈ ���� ����ؼ� ����
        Vec3 vPos = Transform()->GetRelativePos();
        vPos.x -= vDrag.x * scale * xRatio;
        vPos.y += vDrag.y * scale * yRatio;
        Transform()->SetRelativePos(vPos);
    }

    // Zoom
    short wheel = CKeyMgr::GetInst()->GetMouseWheel();
    if (wheel < 0)
        Camera()->SetScale(Camera()->GetScale() * 1.1f);
    else if (wheel > 0)
        Camera()->SetScale(Camera()->GetScale() * 0.9f);
}

void CCameraMoveScript::MovePerspective()
{
    float CamSpeed = GetOwner()->Camera()->GetCameraSpeed();

    if (KEY_PRESSED(KEY::RBTN))
    {
        // Move
        Vec3 vPos = Transform()->GetRelativePos();

        Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
        Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
        Vec3 vUp = Transform()->GetWorldDir(DIR_TYPE::UP);

        if (KEY_PRESSED(KEY::W))
            vPos += DT * CamSpeed * vFront;

        if (KEY_PRESSED(KEY::S))
            vPos += DT * CamSpeed * -vFront;

        if (KEY_PRESSED(KEY::A))
            vPos += DT * CamSpeed * -vRight;

        if (KEY_PRESSED(KEY::D))
            vPos += DT * CamSpeed * vRight;

        if (KEY_PRESSED(KEY::E))
            vPos += DT * CamSpeed * vUp;

        if (KEY_PRESSED(KEY::Q))
            vPos += DT * CamSpeed * -vUp;

        Transform()->SetRelativePos(vPos);

        // Drag
        Vec2 vDrag = CKeyMgr::GetInst()->GetMouseDrag();
        Vec3 vRot = Transform()->GetRelativeRotation();
        vRot.y += vDrag.x * XM_PI / 360.f;
        vRot.x += vDrag.y * XM_PI / 360.f;
        Transform()->SetRelativeRotation(vRot);

        // Camera speed
        short wheel = CKeyMgr::GetInst()->GetMouseWheel();
        float CamSpeedOffset = 10000.f;
        if (wheel < 0)
            Camera()->SetCameraSpeed(CamSpeed - DT * CamSpeedOffset);
        else if (wheel > 0)
            Camera()->SetCameraSpeed(CamSpeed + DT * CamSpeedOffset);
    }

    // Zoom
    float Zoffset = 1.f;
    if (KEY_PRESSED(KEY::Z))
        Camera()->SetFOV(Camera()->GetFOV() + DT * Zoffset);

    if (KEY_PRESSED(KEY::C))
        Camera()->SetFOV(Camera()->GetFOV() - DT * Zoffset);
}

void CCameraMoveScript::MoveFocusOrthographic()
{
    if (nullptr == CLevelMgr::GetInst()->GetSelectedObj())
    {
        m_bFocus = false;
        return;
    }

    Vec3 CamPos = Transform()->GetWorldPos();
    Vec3 ObjPos = CLevelMgr::GetInst()->GetSelectedObj()->Transform()->GetWorldPos();

    Vec2 dir = Vec2(ObjPos.x, ObjPos.y) - Vec2(CamPos.x, CamPos.y);

    // ī�޶� ������Ʈ�� ���õǾ��ų� ������ ��ġ��ó�� �����ߴٸ� Stop
    if (GetOwner() == CLevelMgr::GetInst()->GetSelectedObj() || (dir.Length() < 10.f))
    {
        m_bFocus = false;
    }

    if (m_bFocus)
    {
        Vec3 dir3 = Vec3(dir.x, dir.y, 0.f);
        float CamMoveSpeed = 5000.f;
        float scale = Camera()->GetScale();
        Transform()->SetRelativePos(Transform()->GetRelativePos() + DT * dir3.Normalize() * CamMoveSpeed * scale);
    }
}

void CCameraMoveScript::MoveFocusPerspective()
{
    if (nullptr == CLevelMgr::GetInst()->GetSelectedObj())
    {
        m_bFocus = false;
        return;
    }

    float focusDist = 200.f;
    Vec3 CamDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    Vec3 CamDistPos = Transform()->GetWorldPos() + CamDir * focusDist;

    Vec3 ObjPos = CLevelMgr::GetInst()->GetSelectedObj()->Transform()->GetWorldPos();
    Vec3 dir = ObjPos - CamDistPos;

    // ī�޶� ������Ʈ�� ���õǾ��ų� ������ ��ġ��ó�� �����ߴٸ� Stop
    if (GetOwner() == CLevelMgr::GetInst()->GetSelectedObj() || (dir.Length() < 10.f))
    {
        m_bFocus = false;
    }

    if (m_bFocus)
    {
        float CamMoveSpeed = 5000.f;
        Transform()->SetRelativePos(Transform()->GetRelativePos() + DT * dir.Normalize() * CamMoveSpeed);
    }
}
