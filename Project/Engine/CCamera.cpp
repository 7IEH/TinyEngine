#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"
#include "CTransform.h"

#include "CRenderMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CMeshRender.h"
#include "CMaterial.h"
#include "CAssetMgr.h"

CCamera::CCamera()
    : CComponent(COMPONENT_TYPE::CAMERA)
    , m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
    , m_FOV(XM_PI / 2.f)
    , m_Width(0.f)
    , m_Scale(1.f)
    , m_AspectRatio(1.f)
    , m_Near(1.f)
    , m_Far(10000.f)
    , m_CamSpeed(250.f)
    , m_LayerCheck(0)
{
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_AspectRatio = vResol.x / vResol.y;
}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
    // �������� ȭ��� ���
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    if (!(vResol.x <= 0.f || vResol.y <= 0.f)) // â �ּ�ȭ ����ó��
    {
        m_Width = vResol.x;
        m_AspectRatio = vResol.x / vResol.y;
    }

    // ī�޶� �ӵ� ����
    if (m_CamSpeed < 0.f)
        m_CamSpeed = 0.f;

    // �� ����� ����Ѵ�.
    // ī�޶� �������� �̵���Ű�� �̵� ���
    Vec3 vCamPos = Transform()->GetRelativePos();
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    //// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
    // Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
    // Vec3 vUp = Transform()->GetWorldDir(DIR_TYPE::UP);
    // Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);

    // Matrix matRotate = XMMatrixIdentity();
    // matRotate._11 = vRight.x;
    // matRotate._12 = vUp.x;
    // matRotate._13 = vFront.x;

    // matRotate._21 = vRight.y;
    // matRotate._22 = vUp.y;
    // matRotate._23 = vFront.y;

    // matRotate._31 = vRight.z;
    // matRotate._32 = vUp.z;
    // matRotate._33 = vFront.z;

    Vec3 vCamRot = Transform()->GetRelativeRotation();
    Matrix matRotate =
        Matrix::CreateRotationX(vCamRot.x) * Matrix::CreateRotationY(vCamRot.y) * Matrix::CreateRotationZ(vCamRot.z);
    matRotate = matRotate.Transpose(); // ��������� ������� ��ġ���

    // �̵� x ȸ�� = view ���
    m_matView = matTrans * matRotate;

    // ���� ��Ŀ� ���� ���� ����� ����Ѵ�.
    m_matProj = XMMatrixIdentity();

    if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
    {
        // �ּ� ������ ����
        if (m_Scale <= 0.f)
            m_Scale = 0.001f;

        // ��������
        m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, m_Near, m_Far);
    }
    else
    {
        // ��������
        m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);
    }
}

void CCamera::SetCameraPriority(int _Priority)
{
    CRenderMgr::GetInst()->RegisterCamera(this, _Priority);
}

void CCamera::LayerCheck(UINT _LayerIdx, bool _bCheck)
{
    if (_bCheck)
    {
        m_LayerCheck |= (1 << _LayerIdx);
    }
    else
    {
        m_LayerCheck &= ~(1 << _LayerIdx);
    }
}

void CCamera::LayerCheck(const wstring& _strLayerName, bool _bCheck)
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    CLayer* pLayer = pCurLevel->GetLayer(_strLayerName);

    if (nullptr == pLayer)
        return;

    int idx = pLayer->GetLayerIdx();
    LayerCheck(idx, _bCheck);
}

void CCamera::render()
{
    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matView = m_matView;
    g_Transform.matProj = m_matProj;

    // eyePos ���
    g_Global.eyeWorld = Transform()->GetRelativePos();

    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (int i = 0; i < LAYER_MAX; ++i)
    {
        // ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
        if (false == (m_LayerCheck & (1 << i)))
            continue;

        CLayer* pLayer = pCurLevel->GetLayer(i);
        const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
        for (size_t i = 0; i < vecObjects.size(); ++i)
        {
            wstring LayerName = pLayer->GetName();

            // Render Pass
            vecObjects[i]->render();

            CMeshRender* meshRender = vecObjects[i]->MeshRender();
            if (nullptr != meshRender)
            {
                Ptr<CMaterial> mtrl = meshRender->GetMaterial();
                if (nullptr != mtrl)
                {
                    Ptr<CGraphicsShader> shader = mtrl->GetShader();

                    // Normal Line Pass
                    if (meshRender->IsDrawNormalLine())
                    {
                        mtrl->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"NormalLine"));
                        vecObjects[i]->render();
                    }

                    // outline pass
                    // ���̾� �������϶��� Off
                    // SkyBox �϶��� Off
                    if (CLevelMgr::GetInst()->GetSelectedObj() == vecObjects[i] && !g_Global.DrawAsWireFrame &&
                        LayerName != L"SkyBox")
                    {
                        mtrl->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"OutLine"));
                        vecObjects[i]->render();
                    }

                    // IDMap
                    if (LayerName == L"SkyBox" || LayerName == L"Mesh")
                    {
                        Ptr<CTexture> pIDMapTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"IDMapTex");
                        Ptr<CTexture> pIDMapDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"IDMapDSTex");

                        CONTEXT->OMSetRenderTargets(1, pIDMapTex->GetRTV().GetAddressOf(), pIDMapDSTex->GetDSV().Get());

                        Ptr<CGraphicsShader> IDShader = CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"IDMap");

                        if (LayerName == L"SkyBox")
                            IDShader = CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"SkyBox_IDMap");

                        mtrl->SetShader(IDShader);
                        vecObjects[i]->render();

                        Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
                        Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

                        CONTEXT->OMSetRenderTargets(1, pTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());
                    }

                    // ���� ���̴��� ����
                    mtrl->SetShader(shader);
                }
            }
        }
    }
}
