#include "pch.h"

#include "CTaskMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "CComponent.h"

#include "CEngine.h"
#include "CDevice.h"
#include "CEditorMgr.h"
#include "CRenderMgr.h"
#include "CAssetMgr.h"
#include "CKeyMgr.h"

#include "CCamera.h"
#include "CTransform.h"
// stb
#define _CRT_SECURE_NO_WARNINGS // stb_image_write compile error fix
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

CTaskMgr::CTaskMgr()
{
}

CTaskMgr::~CTaskMgr()
{
}

void CTaskMgr::tick()
{
    {
        // ������ ������ üũ
        RECT rect;
        if (GetClientRect(CEngine::GetInst()->GetMainWind(), &rect))
        {
            Vec2 Resolution = CEngine::GetInst()->GetResolution();
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            if (Resolution.x != width || Resolution.y != height)
            {
                GamePlayStatic::WindowResize(width, height);
            }
        }

        // ��ũ����
        if (KEY_TAP(KEY::PRINT))
        {
            GamePlayStatic::ScreenShot();
        }

        // Mouse Picking
        if (KEY_TAP(KEY::LBTN))
        {
            //// Color Picking
            //{
            //    Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();

            //    // Editor ��忴�ٸ� Viewport ������ ���콺��ġ�� ����
            //    if (nullptr != CEditorMgr::GetInst()->GetCurEditor())
            //        MousePos = CEditorMgr::GetInst()->GetViewportMousePos();

            //    GamePlayStatic::MouseColorPicking(MousePos);
            //}

            // Ray Picking
            {
                Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();

                // Editor ��忴�ٸ� Viewport ������ ���콺��ġ�� ����
                if (nullptr != CEditorMgr::GetInst()->GetCurEditor())
                    MousePos = CEditorMgr::GetInst()->GetViewportMousePos();

                GamePlayStatic::MouseRayPicking(MousePos);
            }
        }
    }

    for (size_t i = 0; i < m_vecTask.size(); ++i)
    {
        switch (m_vecTask[i].Type)
        {
        case TASK_TYPE::CREATE_OBJECT:
            {
                int LayerIdx = (int)m_vecTask[i].Param_1;
                CGameObject* Object = (CGameObject*)m_vecTask[i].Param_2;

                CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
                pCurLevel->AddObject(Object, LayerIdx, true);

                /*if (LEVEL_STATE::PLAY == pCurLevel->GetState())
                {
                    Object->begin();
                }*/
            }
            break;
        case TASK_TYPE::DELETE_OBJECT:
            {
                CGameObject* pDeadObj = (CGameObject*)m_vecTask[i].Param_1;

                list<CGameObject*> queue;
                queue.push_back(pDeadObj);

                // ���̾ �ԷµǴ� ������Ʈ ����, �� �ؿ� �޸� �ڽĵ���� ��� Dead ó��
                while (!queue.empty())
                {
                    CGameObject* pObject = queue.front();
                    queue.pop_front();

                    pObject->m_bDead = true;

                    for (size_t i = 0; i < pObject->m_vecChild.size(); ++i)
                    {
                        queue.push_back(pObject->m_vecChild[i]);
                    }
                }
            }
            break;
        case TASK_TYPE::LEVEL_CHANGE:
            {
            }
            break;
        case TASK_TYPE::ADD_CHILD:
            break;
        case TASK_TYPE::DISCONNECT_PARENT:
            break;
        case TASK_TYPE::WINDOW_RESIZE:
            {
                UINT width = (UINT)m_vecTask[i].Param_1;
                UINT height = (UINT)m_vecTask[i].Param_2;
                if (width <= 0 || height <= 0)
                    break;

                Vec2 resolution = Vec2(width, height);
                CEngine::GetInst()->SetResolution(resolution);

                // ��� ����
                CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, L"RenderTargetTex");
                CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, L"DepthStencilTex");
                CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, L"RTCopyTex");
                CAssetMgr::GetInst()->DeleteAsset(ASSET_TYPE::TEXTURE, L"IDMap");

                CDevice::GetInst()->Resize(resolution);
                CRenderMgr::GetInst()->Resize(resolution);
            }
            break;
        case TASK_TYPE::DELETE_ASSET:
            {
                ASSET_TYPE type = (ASSET_TYPE)m_vecTask[i].Param_1;
                CAsset* pAsset = (CAsset*)m_vecTask[i].Param_2;
                CAssetMgr::GetInst()->DeleteAsset(type, pAsset->GetKey());
            }
            break;
        case TASK_TYPE::SCREENSHOT:
            {
                Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
                Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");

                //// MSAA ���� Texture2DMS �� Texture2D
                // Ptr<CTexture> tempTex = CAssetMgr::GetInst()->CreateTexture(
                //     L"tempTex", (UINT)Resolution.x, (UINT)Resolution.y, DXGI_FORMAT_R8G8B8A8_UNORM,
                //     D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
                //;
                // CONTEXT->ResolveSubresource(tempTex->GetTex2D().Get(), 0, pTex->GetTex2D().Get(), 0,
                //                             DXGI_FORMAT_R8G8B8A8_UNORM);

                // Texture2D �� StagingTexture
                D3D11_TEXTURE2D_DESC desc;
                pTex->GetTex2D()->GetDesc(&desc);
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU���� �б� ����
                desc.Usage = D3D11_USAGE_STAGING;            // GPU���� CPU�� ���� �����͸� �ӽ� ����

                ComPtr<ID3D11Texture2D> stagingTexture;
                DEVICE->CreateTexture2D(&desc, nullptr, stagingTexture.GetAddressOf());

                // ��ü ����
                CONTEXT->CopyResource(stagingTexture.Get(), pTex->GetTex2D().Get());

                // GPU �� CPU
                // R8G8B8A8 �̶�� ����
                std::vector<uint8_t> pixels(desc.Width * desc.Height * 4);

                D3D11_MAPPED_SUBRESOURCE ms;
                CONTEXT->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
                             &ms); // D3D11_MAP_READ ����

                // �ؽ��簡 ���� ��쿡��
                // ms.RowPitch�� width * sizeof(uint8_t) * 4���� Ŭ ���� �־
                // for������ ������ �ϳ��� ����
                uint8_t* pData = (uint8_t*)ms.pData;
                for (unsigned int h = 0; h < desc.Height; h++)
                {
                    memcpy(&pixels[h * desc.Width * 4], &pData[h * ms.RowPitch], desc.Width * sizeof(uint8_t) * 4);
                }

                CONTEXT->Unmap(stagingTexture.Get(), NULL);
                string filename = WstringTostring(CPathMgr::GetOutputPath());
                filename += "screenshot\\";

                // Check if the folder exists, if not, create it
                if (!std::filesystem::exists(filename))
                {
                    std::filesystem::create_directory(filename);
                }

                filename += currentDateTime();
                filename += ".png";
                if (stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, pixels.data(), desc.Width * 4))
                {
                    std::cout << "Screenshot! FileName : " << filename << std::endl;
                }
            }
            break;
        case TASK_TYPE::MOUSE_COLOR_PICKING:
            {
                int MouseX = (int)m_vecTask[i].Param_1;
                int MouseY = (int)m_vecTask[i].Param_2;
                Vec2 WindowSize = CDevice::GetInst()->GetRenderResolution();

                if (nullptr != CEditorMgr::GetInst()->GetCurEditor())
                {
                    Vec2 ViewportSize = CEditorMgr::GetInst()->GetViewportSize();
                    if (ViewportSize.x <= 0 || ViewportSize.y <= 0)
                        break;

                    // Viewport Screen �� 0 ~ 1
                    float NdcMouseX = MouseX / ViewportSize.x;
                    float NdcMouseY = MouseY / ViewportSize.y;

                    // 0 ~ 1 �� Window Screen
                    MouseX = int(NdcMouseX * WindowSize.x);
                    MouseY = int(NdcMouseY * WindowSize.y);
                }

                if (MouseX < 0 || MouseY < 0 || MouseX >= WindowSize.x || MouseY >= WindowSize.y)
                    break;

                Ptr<CTexture> pIDMapTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"IDMap");

                // Texture2D �� StagingTexture
                D3D11_TEXTURE2D_DESC desc;
                pIDMapTex->GetTex2D()->GetDesc(&desc);
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU���� �б� ����
                desc.Usage = D3D11_USAGE_STAGING;            // GPU���� CPU�� ���� �����͸� �ӽ� ����

                ComPtr<ID3D11Texture2D> stagingTexture;
                DEVICE->CreateTexture2D(&desc, nullptr, stagingTexture.GetAddressOf());

                // ���콺��ġ�� 1�ȼ��� ����
                D3D11_BOX box;
                box.left = MouseX;
                box.right = MouseX + 1;
                box.top = MouseY;
                box.bottom = MouseY + 1;
                box.front = 0;
                box.back = 1;
                CONTEXT->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0, pIDMapTex->GetTex2D().Get(), 0, &box);

                // R8G8B8A8
                D3D11_MAPPED_SUBRESOURCE ms;
                CONTEXT->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL, &ms);
                uint8_t* pData = (uint8_t*)ms.pData;
                uint8_t m_pickColor[4] = {};
                memcpy(m_pickColor, pData, sizeof(uint8_t) * 4);
                CONTEXT->Unmap(stagingTexture.Get(), NULL);

                CGameObject* pSelectedObj = nullptr;

                // ���� ������ ������Ʈ�� ID�� ��
                CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
                for (int i = 0; i < LAYER_MAX; ++i)
                {
                    CLayer* pLayer = pCurLevel->GetLayer(i);
                    const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
                    for (size_t i = 0; i < vecObjects.size(); ++i)
                    {
                        // ������Ʈ �̸����� HashID ����
                        std::hash<std::wstring> hasher;
                        int HashID = (int)hasher(vecObjects[i]->GetName());
                        Vec4 colorID = HashIDToColor(HashID);

                        // 0 ~ 1 �� 0 ~ 255 ����Ȯ��
                        uint8_t colorIDInt[4] = {
                            static_cast<uint8_t>(colorID[0] * 255.f), static_cast<uint8_t>(colorID[1] * 255.f),
                            static_cast<uint8_t>(colorID[2] * 255.f), static_cast<uint8_t>(colorID[3] * 255.f)};

                        // Picking Color ��
                        if (m_pickColor[0] == colorIDInt[0] && m_pickColor[1] == colorIDInt[1] &&
                            m_pickColor[2] == colorIDInt[2] && m_pickColor[3] == colorIDInt[3])
                        {
                            pSelectedObj = vecObjects[i];
                            break;
                        }
                    }

                    // ���õ� ������Ʈ�� �ִٸ� Ż��
                    if (nullptr != pSelectedObj)
                        break;
                }

                if (!ImGuizmo::IsUsing())
                    CLevelMgr::GetInst()->SetSelectObj(pSelectedObj);
            }
            break;
        case TASK_TYPE::MOUSE_RAY_PICKING:
            {
                int MouseX = (int)m_vecTask[i].Param_1;
                int MouseY = (int)m_vecTask[i].Param_2;

                float NdcMouseX = 0.0f;
                float NdcMouseY = 0.0f;

                // ���콺 Ŀ���� ��ġ�� NDC�� ��ȯ
                // ���콺 Ŀ���� ���� ��� (0, 0), ���� �ϴ�(width-1, height-1)
                // NDC�� ���� �ϴ��� (-1, -1), ���� ���(1, 1)
                if (nullptr != CEditorMgr::GetInst()->GetCurEditor())
                {
                    Vec2 ViewportSize = CEditorMgr::GetInst()->GetViewportSize();
                    if (ViewportSize.x <= 0 || ViewportSize.y <= 0)
                        break;

                    NdcMouseX = MouseX * 2.0f / ViewportSize.x - 1.0f;
                    NdcMouseY = -MouseY * 2.0f / ViewportSize.y + 1.0f;
                }
                else
                {
                    Vec2 WindowSize = CDevice::GetInst()->GetRenderResolution();
                    NdcMouseX = MouseX * 2.0f / WindowSize.x - 1.0f;
                    NdcMouseY = -MouseY * 2.0f / WindowSize.y + 1.0f;
                }

                if (NdcMouseX < -1.0 || NdcMouseY < -1.0 || NdcMouseX > 1.0 || NdcMouseY > 1.0)
                    break;

                Vector3 cursorNdcNear = Vector3(NdcMouseX, NdcMouseY, 0);
                Vector3 cursorNdcFar = Vector3(NdcMouseX, NdcMouseY, 1);

                // ���� ī�޶�
                CCamera* pCam = CRenderMgr::GetInst()->GetCamera(0);

                Matrix inverseProjView = (pCam->GetViewMat() * pCam->GetProjMat()).Invert();

                // ViewFrustum �ȿ��� PickingRay�� ���� ���ϱ�
                Vector3 NearWorld = Vector3::Transform(cursorNdcNear, inverseProjView);
                Vector3 FarWorld = Vector3::Transform(cursorNdcFar, inverseProjView);

                Vector3 dir = FarWorld - NearWorld;
                dir.Normalize();

                CGameObject* pSelectedObj = nullptr;

                // ������ ����� �浹 ����
                SimpleMath::Ray curRay = SimpleMath::Ray(NearWorld, dir);
                float dist = 0.0f;

                CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
                for (int i = 0; i < LAYER_MAX; ++i)
                {
                    CLayer* pLayer = pCurLevel->GetLayer(i);
                    const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
                    for (size_t i = 0; i < vecObjects.size(); ++i)
                    {
                        float dist = 0.0f;
                        bool bSelected = curRay.Intersects(vecObjects[i]->Transform()->GetBoundingSphere(), dist);

                        if (bSelected)
                        {
                            pSelectedObj = vecObjects[i];
                            break;
                        }
                    }

                    // ���õ� ������Ʈ�� �ִٸ� Ż��
                    if (nullptr != pSelectedObj)
                        break;
                }

                if (!ImGuizmo::IsUsing())
                    CLevelMgr::GetInst()->SetSelectObj(pSelectedObj);
            }
            break;
        }
    }

    m_vecTask.clear();
}