#include "pch.h"
#include "CCamera.h"

#include "CAssetMgr.h"
#include "CLevelMgr.h"

#include "CDevice.h"
#include "CTransform.h"

#include "CRenderMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CMeshRender.h"
#include "CMaterial.h"

#include "CRenderComponent.h"

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
    , m_iCamPriority(-1)
{
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_AspectRatio = vResol.x / vResol.y;
}

CCamera::~CCamera()
{
}

void CCamera::begin()
{
    if (-1 != m_iCamPriority)
    {
        CRenderMgr::GetInst()->RegisterCamera(this, m_iCamPriority);
    }
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

    // =====================
    // �� ��� ���
    // =====================
    // ī�޶� �������� �̵���Ű�� �̵� ���
    Vec3 vCamPos = Transform()->GetRelativePos();
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    //// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
    Vec3 vCamRot = Transform()->GetRelativeRotation();
    Matrix matRotate =
        Matrix::CreateRotationX(vCamRot.x) * Matrix::CreateRotationY(vCamRot.y) * Matrix::CreateRotationZ(vCamRot.z);
    matRotate = matRotate.Transpose(); // ��������� ������� ��ġ���

    // �̵� x ȸ�� = view ���
    m_matView = matTrans * matRotate;

    // ===============================
    // ���� ��Ŀ� ���� ���� ��� ���
    // ===============================
    m_matProj = XMMatrixIdentity();

    if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
    {
        // �ּ� ������ ����
        if (m_Scale <= 0.f)
            m_Scale = 1e-5f;

        // ��������
        m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale, m_Near, m_Far);
    }
    else
    {
        // FOV ����
        float offset = XM_1DIV2PI; // 1/2�� 1�� ����
        if (m_FOV < XM_1DIV2PI)
            m_FOV = offset;
        else if (m_FOV > XM_PI - offset)
            m_FOV = XM_PI - offset;

        // ��������
        m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);
    }
}

void CCamera::SetCameraPriority(int _Priority)
{
    m_iCamPriority = _Priority;
    CRenderMgr::GetInst()->RegisterCamera(this, m_iCamPriority);
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

void CCamera::SortObject()
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (int i = 0; i < LAYER_MAX; ++i)
    {
        // ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
        if (false == (m_LayerCheck & (1 << i)))
            continue;

        CLayer* pLayer = pCurLevel->GetLayer(i);
        const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
        for (size_t j = 0; j < vecObjects.size(); ++j)
        {
            // �޽�, ����, ���̴� Ȯ��
            if (!(vecObjects[j]->GetRenderComopnent() && vecObjects[j]->GetRenderComopnent()->GetMesh().Get() &&
                  vecObjects[j]->GetRenderComopnent()->GetMaterial().Get() &&
                  vecObjects[j]->GetRenderComopnent()->GetMaterial()->GetShader().Get()))
            {
                continue;
            }

            SHADER_DOMAIN domain = vecObjects[j]->GetRenderComopnent()->GetMaterial()->GetShader()->GetDomain();

            switch (domain)
            {
            case SHADER_DOMAIN::DOMAIN_OPAQUE:
                m_vecOpaque.push_back(vecObjects[j]);
                break;
            case SHADER_DOMAIN::DOMAIN_MASKED:
                m_vecMaked.push_back(vecObjects[j]);
                break;
            case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
                m_vecTransparent.push_back(vecObjects[j]);
                break;
            case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
                m_vecPostProcess.push_back(vecObjects[j]);
                break;
            case SHADER_DOMAIN::DOMAIN_DEBUG:
                break;
            }
        }
    }
}

void CCamera::render()
{
    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matView = m_matView;
    g_Transform.matProj = m_matProj;

    // eyePos ���
    g_Global.eyeWorld = Transform()->GetWorldPos();

    // Domain ������� ������
    render(m_vecOpaque);
    render(m_vecMaked);
    render(m_vecTransparent);
    render(m_vecPostProcess);
}

void CCamera::render(vector<CGameObject*>& _vecObj)
{
    for (size_t i = 0; i < _vecObj.size(); ++i)
    {
        wstring LayerName = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(_vecObj[i]->GetLayerIdx())->GetName();

        // Render Pass
        _vecObj[i]->render();

        CMeshRender* meshRender = _vecObj[i]->MeshRender();
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
                    _vecObj[i]->render();
                }

                // outline pass
                // ���̾� �������϶��� Off
                // SkyBox �϶��� Off
                if (CLevelMgr::GetInst()->GetSelectedObject() == _vecObj[i] && !g_Global.DrawAsWireFrame &&
                    LayerName != L"SkyBox")
                {
                    mtrl->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"OutLine"));
                    _vecObj[i]->render();
                }

                // IDMap
                if (LayerName != L"UI" && LayerName != L"Light" && LayerName != L"Camera")
                {
                    Ptr<CTexture> pIDMapTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"IDMapTex");
                    Ptr<CTexture> pIDMapDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"IDMapDSTex");

                    CONTEXT->OMSetRenderTargets(1, pIDMapTex->GetRTV().GetAddressOf(), pIDMapDSTex->GetDSV().Get());

                    Ptr<CGraphicsShader> IDShader = CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"IDMap");

                    if (LayerName == L"SkyBox")
                        IDShader = CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"SkyBox_IDMap");

                    mtrl->SetShader(IDShader);
                    _vecObj[i]->render();

                    Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
                    Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");

                    CONTEXT->OMSetRenderTargets(1, pTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());
                }

                // ���� ���̴��� ����
                mtrl->SetShader(shader);
            }
        }
    }

    _vecObj.clear();
}

void CCamera::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
    fwrite(&m_FOV, sizeof(float), 1, _File);
    fwrite(&m_Width, sizeof(float), 1, _File);
    fwrite(&m_Scale, sizeof(float), 1, _File);
    fwrite(&m_AspectRatio, sizeof(float), 1, _File);
    fwrite(&m_Near, sizeof(float), 1, _File);
    fwrite(&m_Far, sizeof(float), 1, _File);
    fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
    fwrite(&m_CamSpeed, sizeof(float), 1, _File);
    fwrite(&m_iCamPriority, sizeof(int), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
    fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
    fread(&m_FOV, sizeof(float), 1, _File);
    fread(&m_Width, sizeof(float), 1, _File);
    fread(&m_Scale, sizeof(float), 1, _File);
    fread(&m_AspectRatio, sizeof(float), 1, _File);
    fread(&m_Near, sizeof(float), 1, _File);
    fread(&m_Far, sizeof(float), 1, _File);
    fread(&m_LayerCheck, sizeof(UINT), 1, _File);
    fread(&m_CamSpeed, sizeof(float), 1, _File);
    fread(&m_iCamPriority, sizeof(int), 1, _File);
}
