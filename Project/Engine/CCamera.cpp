#include "pch.h"
#include "CCamera.h"

#include "CAssetMgr.h"
#include "CEditorMgr.h"

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
#include "CConstBuffer.h"
#include "CMRT.h"
#include "CLight3D.h"

CCamera::CCamera()
    : CComponent(COMPONENT_TYPE::CAMERA)
    , m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
    , m_FOV(XM_PI / 2.f)
    , m_Width(0.f)
    , m_Scale(1.f)
    , m_AspectRatio(1.f)
    , m_Near(1.f)
    , m_Far(10000.f)
    , m_LayerMask(0)
    , m_iCamPriority(-1)
    , m_bHDRI(false)
    , m_matView()
    , m_matProj()
    , m_vecDeferred{}
    , m_vecOpaque{}
    , m_vecMaked{}
    , m_vecTransparent{}
    , m_vecPostProcess{}
{
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_Width = vResol.x;
    m_AspectRatio = vResol.x / vResol.y;

    LayerMaskAll();
}

CCamera::~CCamera()
{
}

void CCamera::begin()
{
    CRenderMgr::GetInst()->RegisterCamera(this, m_iCamPriority);
}

void CCamera::finaltick()
{
    // =====================
    // �� ��� ���
    // =====================
    // ī�޶� �������� �̵���Ű�� �̵� ���
    Vec3 vCamPos = Transform()->GetRelativePos();
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    //// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
    Vec3 vCamRot = Transform()->GetRelativeRotation();
    Matrix matRotate = Matrix::CreateRotationX(vCamRot.x) * Matrix::CreateRotationY(vCamRot.y) * Matrix::CreateRotationZ(vCamRot.z);
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

void CCamera::LayerMask(UINT _LayerIdx, bool _bMask)
{
    if (_bMask)
    {
        m_LayerMask |= (1 << _LayerIdx);
    }
    else
    {
        m_LayerMask &= ~(1 << _LayerIdx);
    }
}

void CCamera::LayerMask(CLevel* _CurLevel, const wstring& _strLayerName, bool _bMask)
{
    CLayer* pLayer = _CurLevel->GetLayer(_strLayerName);

    if (nullptr == pLayer)
        return;

    int idx = pLayer->GetLayerIdx();
    LayerMask(idx, _bMask);
}

void CCamera::SortObject()
{
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (int i = 0; i < LAYER_MAX; ++i)
    {
        // ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
        if (false == (m_LayerMask & (1 << i)))
            continue;

        CLayer* pLayer = pCurLevel->GetLayer(i);
        const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
        for (size_t j = 0; j < vecObjects.size(); ++j)
        {
            // �޽�, ����, ���̴� Ȯ��
            if (!(vecObjects[j]->GetRenderComponent() && vecObjects[j]->GetRenderComponent()->GetMesh().Get() &&
                  vecObjects[j]->GetRenderComponent()->GetMaterial().Get() && vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader().Get()))
            {
                continue;
            }

            SHADER_DOMAIN domain = vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader()->GetDomain();

            switch (domain)
            {
            case SHADER_DOMAIN::DOMAIN_DEFERRED:
                m_vecDeferred.push_back(vecObjects[j]);
                break;
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
            }
        }
    }
}

void CCamera::render()
{
    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matView.Invert();
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProj.Invert();

    // Deferred Render Pass
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
    render(m_vecDeferred);

    // SSAO Pass
    render_SSAO();

    // Light Pass
    render_Light();

    // RenderTarget ����
    if (m_bHDRI)
    {
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDRI)->OMSet();
    }
    else
    {
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
    }

    // Merge Pass
    render_Merge();

#ifndef DISTRIBUTE
    // OutLine Pass
    render_OutLine(m_vecDeferred);
    render_OutLine(m_vecOpaque);
    render_OutLine(m_vecMaked);
    render_OutLine(m_vecTransparent);
#endif // DISTRIBUTE

    // Main Render Pass
    render(m_vecOpaque);
    render(m_vecMaked);
    render(m_vecTransparent);

#ifndef DISTRIBUTE
    // IDMap Pass
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::IDMAP)->OMSet();
    render_IDMap(m_vecDeferred);
    render_IDMap(m_vecOpaque);
    render_IDMap(m_vecMaked);
    render_IDMap(m_vecTransparent);
#endif // DISTRIBUTE

    // ��ó��
    if (m_bHDRI)
    {
        CRenderMgr::GetInst()->render_postprocess_HDRI();
    }
    else
    {
        CRenderMgr::GetInst()->render_postprocess_LDRI();
    }

    render_Postprocess();

    // Clear
    render_Clear();
}

void CCamera::render_SSAO()
{
    CTexture::Clear(26);
    CMRT* pSSAOMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SSAO);

    pSSAOMRT->OMSet();

    static Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
    static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SSAOMtrl");

    pMtrl->UpdateData();
    pMesh->render();

    CONTEXT->OMSetRenderTargets(0, NULL, NULL);
    pSSAOMRT->GetRenderTargetTex(0)->UpdateData(26);
}

void CCamera::render_Light()
{
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();

    const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetvecLight3D();
    for (UINT Idx = 0; Idx < vecLight3D.size(); ++Idx)
    {
        vecLight3D[Idx]->render_Deferred(Idx);
    }
}

void CCamera::render_Merge()
{
    static Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
    static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UnrealPBRDeferredMergeMtrl");
    // static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Merge_DeferredMtrl"); // Phong

    pMtrl->UpdateData();
    pMesh->render();
}

void CCamera::render_DepthOnly(Ptr<CTexture> _DepthMapTex)
{
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matView.Invert();
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProj.Invert();

    CONTEXT->ClearDepthStencilView(_DepthMapTex->GetDSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    CONTEXT->OMSetRenderTargets(0, NULL, _DepthMapTex->GetDSV().Get());
    CDevice::GetInst()->SetViewport((float)_DepthMapTex->GetWidth(), (float)_DepthMapTex->GetHeight());

    render_DepthOnly(m_vecDeferred);
    render_DepthOnly(m_vecOpaque);
    render_DepthOnly(m_vecMaked);
    render_DepthOnly(m_vecTransparent);

    // Clear
    render_Clear();

    CONTEXT->OMSetRenderTargets(0, NULL, NULL);
}

void CCamera::render_Clear()
{
    m_vecDeferred.clear();
    m_vecOpaque.clear();
    m_vecMaked.clear();
    m_vecTransparent.clear();
    m_vecPostProcess.clear();
}

void CCamera::render(vector<CGameObject*>& _vecObj)
{
    for (size_t i = 0; i < _vecObj.size(); ++i)
    {
#ifdef DISTRIBUTE
        _vecObj[i]->render();
#else
        // Render Pass
        if (g_Global.g_DrawAsWireFrame)
        {
            RS_TYPE originRSType = _vecObj[i]->GetRenderComponent()->GetMaterial()->GetShader()->GetRSType();
            _vecObj[i]->GetRenderComponent()->GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
            _vecObj[i]->render();
            _vecObj[i]->GetRenderComponent()->GetMaterial()->GetShader()->SetRSType(originRSType);
        }
        else
        {
            _vecObj[i]->render();
        }
#endif // DISTRIBUTE
    }
}

void CCamera::render_OutLine(vector<CGameObject*>& _vecObj)
{
    CGameObject* pSelectedObj = CEditorMgr::GetInst()->GetSelectedObject();

    for (size_t i = 0; i < _vecObj.size(); ++i)
    {
        if (pSelectedObj != _vecObj[i] || this != CRenderMgr::GetInst()->GetMainCamera() || g_Global.g_DrawAsWireFrame)
            continue;

        // OutLine Pass
        if (g_Global.g_RenderOutline)
        {
            if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
            {
                pSelectedObj->render(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"2D_OutLineMtrl"));
            }
            else
            {
                pSelectedObj->render(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"3D_OutLineMtrl"));
            }
        }

        // NormalLine Pass
        Ptr<CMaterial> NormalLineMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"NormalLineMtrl");
        if (NormalLineMtrl->GetMtrlConst().arrInt[0])
        {
            pSelectedObj->render(NormalLineMtrl);
        }
    }
}

void CCamera::render_DepthOnly(vector<CGameObject*>& _vecObj)
{
    for (size_t i = 0; i < _vecObj.size(); i++)
    {
        _vecObj[i]->render(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DepthOnlyMtrl"));
    }
}

void CCamera::render_IDMap(vector<CGameObject*>& _vecObj)
{
    for (size_t i = 0; i < _vecObj.size(); i++)
    {
        wstring LayerName = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(_vecObj[i]->GetLayerIdx())->GetName();
        // Ư�� ���̾�� IDMap ���� ����
        if (LayerName != L"UI" && LayerName != L"Camera" && LayerName != L"SkyBox")
        {
            // ������Ʈ �̸� + ID������ HashID Find
            hash<wstring> hasher;
            int HashID = (int)hasher(_vecObj[i]->GetName()) + _vecObj[i]->GetID();

            Ptr<CMaterial> IDMapMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"IDMapMtrl");
            IDMapMtrl->SetScalarParam(VEC4_0, HashIDToColor(HashID));
            _vecObj[i]->render(IDMapMtrl);
        }
    }
}

void CCamera::render_Postprocess()
{
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

    for (int i = 0; i < m_vecPostProcess.size(); ++i)
    {
        // ���� ������ �̹����� ��ó�� Ÿ�ٿ� ����
        CRenderMgr::GetInst()->CopyToPostProcessTex_LDRI();

        // ������� ��ó�� �ؽ��ĸ� t15 �������Ϳ� ���ε�
        CRenderMgr::GetInst()->GetPostProcessTex_LDRI()->UpdateData(15);

        // ��ó�� ������Ʈ ������
        m_vecPostProcess[i]->render();
    }

    CTexture::Clear(14);
}

void CCamera::Resize(Vec2 Resolution)
{
    if (Resolution.x <= 0.f || Resolution.y <= 0.f) // â �ּ�ȭ ����ó��
        return;

    m_Width = Resolution.x;
    m_AspectRatio = Resolution.x / Resolution.y;
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
    fwrite(&m_LayerMask, sizeof(UINT), 1, _File);
    fwrite(&m_iCamPriority, sizeof(int), 1, _File);
    fwrite(&m_bHDRI, sizeof(bool), 1, _File);
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
    fread(&m_LayerMask, sizeof(UINT), 1, _File);
    fread(&m_iCamPriority, sizeof(int), 1, _File);
    fread(&m_bHDRI, sizeof(bool), 1, _File);
}
