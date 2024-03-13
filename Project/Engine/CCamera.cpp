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

    // RenderTarget ����
    if (m_bHDRI)
        CDevice::GetInst()->SetFloatRenderTarget();
    else
        CDevice::GetInst()->SetRenderTarget();

    CDevice::GetInst()->SetViewport();

    // Main Render Pass
    render(m_vecOpaque);
    render(m_vecMaked);
    render(m_vecTransparent);

    // Depth Only Pass
    CONTEXT->OMSetRenderTargets(0, NULL, CRenderMgr::GetInst()->GetDepthOnlyTex()->GetDSV().Get());
    render_DepthOnly(m_vecOpaque);
    render_DepthOnly(m_vecMaked);
    render_DepthOnly(m_vecTransparent);

    // IDMap Pass
    CONTEXT->OMSetRenderTargets(1, CRenderMgr::GetInst()->GetIDMapTex()->GetRTV().GetAddressOf(),
                                CRenderMgr::GetInst()->GetIDMapDSTex()->GetDSV().Get());
    render_IDMap(m_vecOpaque);
    render_IDMap(m_vecMaked);
    render_IDMap(m_vecTransparent);

    if (m_bHDRI)
    {
        // ��ó��
        CDevice::GetInst()->SetFloatRenderTarget();
        render_postprocess_HDRI();
        CRenderMgr::GetInst()->render_postprocess();
    }
    else
    {
        CDevice::GetInst()->SetRenderTarget();
        render_postprocess_LDRI();
    }

    // Clear
    m_vecOpaque.clear();
    m_vecMaked.clear();
    m_vecTransparent.clear();
    m_vecPostProcess.clear();
}

void CCamera::render_LightDepth(Ptr<CTexture> _DepthMapTex)
{
    // ���� ���� ������
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matView.Invert();
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProj.Invert();

    CONTEXT->ClearDepthStencilView(_DepthMapTex->GetDSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    CONTEXT->OMSetRenderTargets(0, NULL, _DepthMapTex->GetDSV().Get());
    CDevice::GetInst()->SetViewport((float)_DepthMapTex->GetWidth(), (float)_DepthMapTex->GetHeight());

    render_DepthOnly(m_vecOpaque);
    render_DepthOnly(m_vecMaked);
    render_DepthOnly(m_vecTransparent);

    m_vecOpaque.clear();
    m_vecMaked.clear();
    m_vecTransparent.clear();
}

void CCamera::render(vector<CGameObject*>& _vecObj)
{
    CGameObject* pSelectedObj = CEditorMgr::GetInst()->GetSelectedObject();

    for (size_t i = 0; i < _vecObj.size(); ++i)
    {
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

        // NormalLine & OutLine Pass
        if (pSelectedObj == _vecObj[i] && this == CRenderMgr::GetInst()->GetMainCamera() && !g_Global.g_DrawAsWireFrame)
        {
            // NormalLine Pass
            Ptr<CMaterial> NormalLineMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"NormalLineMtrl");
            if (NormalLineMtrl->GetMtrlConst().arrInt[0])
                pSelectedObj->render(NormalLineMtrl);

            // OutLine Pass
            if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
                pSelectedObj->render(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"2D_OutLineMtrl"));
            else
                pSelectedObj->render(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"3D_OutLineMtrl"));
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

void CCamera::render_postprocess_LDRI()
{
    for (int i = 0; i < m_vecPostProcess.size(); ++i)
    {
        // ���� ������ �̹����� ��ó�� Ÿ�ٿ� ����
        CRenderMgr::GetInst()->CopyToPostProcessTex_LDRI();

        // ������� ��ó�� �ؽ��ĸ� t14 �������Ϳ� ���ε�
        CRenderMgr::GetInst()->GetPostProcessTex_LDRI()->UpdateData(14);

        // ��ó�� ������Ʈ ������
        m_vecPostProcess[i]->render();
    }

    CTexture::Clear(14);
}

void CCamera::render_postprocess_HDRI()
{
    for (int i = 0; i < m_vecPostProcess.size(); ++i)
    {
        // ���� ������ �̹����� ��ó�� Ÿ�ٿ� ����
        CRenderMgr::GetInst()->CopyToPostProcessTex_HDRI();

        // ������� ��ó�� �ؽ��ĸ� t14 �������Ϳ� ���ε�
        CRenderMgr::GetInst()->GetPostProcessTex_HDRI()->UpdateData(14);

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
