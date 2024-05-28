#include "pch.h"
#include "CButtonScript.h"
#include <Engine\\CKeyMgr.h>
#include <Engine\\CEditorMgr.h>
#include <Engine\\CRenderMgr.h>
#include <Engine\\CDevice.h>

CButtonScript::CButtonScript(SCRIPT_TYPE _Type)
    : CScript(_Type)
    , m_bClicked(false)
{
}

CButtonScript::~CButtonScript()
{
}

void CButtonScript::tick()
{
    m_bClicked = false; // �ʱ�ȭ

    if (KEY_TAP(LBTN))
    {
        if (nullptr != BoxCollider2D())
        {
            Vec2 MouseScreenPos = CKeyMgr::GetInst()->GetMousePos();
            if (CEditorMgr::GetInst()->IsEnable())
                MouseScreenPos = CEditorMgr::GetInst()->GetViewportMousePos();

            float NdcMouseX = 0.0f;
            float NdcMouseY = 0.0f;

            // Screen Coordinate �� NDC Coordinate
            // Screen Coordinate ���� ���콺 Ŀ���� ���� ��� (0, 0), ���� �ϴ�(width-1, height-1)
            // NDC�� ���� �ϴ��� (-1, -1), ���� ���(1, 1)
            if (CEditorMgr::GetInst()->IsEnable())
            {
                Vec2 ViewportSize = CEditorMgr::GetInst()->GetViewportSize();
                if (ViewportSize.x <= 0 || ViewportSize.y <= 0)
                    return;

                NdcMouseX = MouseScreenPos.x * 2.0f / ViewportSize.x - 1.0f;
                NdcMouseY = -MouseScreenPos.y * 2.0f / ViewportSize.y + 1.0f;
            }
            else
            {
                Vec2 WindowSize = CDevice::GetInst()->GetRenderResolution();
                NdcMouseX = MouseScreenPos.x * 2.0f / WindowSize.x - 1.0f;
                NdcMouseY = -MouseScreenPos.y * 2.0f / WindowSize.y + 1.0f;
            }

            // ȭ�� ���� Ŭ���� ��� ��ȿ
            if (NdcMouseX < -1.0 || NdcMouseY < -1.0 || NdcMouseX > 1.0 || NdcMouseY > 1.0)
                return;

            CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();

            // NDC Coordinate �� World Coordinate
            Vector3 cursorNdcNear = Vector3(NdcMouseX, NdcMouseY, 0);
            Matrix inverseProjView = (pCam->GetViewMat() * pCam->GetProjMat()).Invert();
            Vector3 NearWorld = Vector3::Transform(cursorNdcNear, inverseProjView);

            if (BoxCollider2D()->IsCollision(Vec2(NearWorld.x, NearWorld.y)))
            {
                m_bClicked = true;
            }
        }
    }
}
