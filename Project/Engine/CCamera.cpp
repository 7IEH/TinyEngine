#include "pch.h"
#include "CCamera.h"

#include "CAssetMgr.h"
#include "CEditorMgr.h"
#include "CKeyMgr.h"
#include "CInstancingBuffer.h"

#include "CDevice.h"
#include "CTransform.h"

#include "CRenderMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CMeshRender.h"
#include "CSkyBox.h"
#include "CMaterial.h"

#include "CRenderComponent.h"
#include "CConstBuffer.h"
#include "CMRT.h"
#include "CLight.h"
#include "CAnimator.h"

CCamera::CCamera()
    : CComponent(COMPONENT_TYPE::CAMERA)
    , m_Frustum(this)
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
    , m_matPrevView()
    , m_matView()
    , m_matViewInv()
    , m_matProj()
    , m_matProjInv()
    , m_mapInstGroup_D{}
    , m_mapInstGroup_F{}
    , m_mapSingleObj{}
    , m_vecSkybox{}
    , m_vecDecal{}
    , m_vecTransparent{}
    , m_vecPostProcess{}
    , m_vecShadow{}
{
    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();
    m_Width = vResol.x;
    m_AspectRatio = vResol.x / vResol.y;
}

CCamera::CCamera(const CCamera& origin)
    : CComponent(origin)
    , m_Frustum(this)
    , m_ProjType(origin.m_ProjType)
    , m_FOV(origin.m_FOV)
    , m_Width(origin.m_Width)
    , m_Scale(origin.m_Scale)
    , m_AspectRatio(origin.m_AspectRatio)
    , m_Near(origin.m_Near)
    , m_Far(origin.m_Far)
    , m_LayerMask(origin.m_LayerMask)
    , m_iCamPriority(-1)
    , m_bHDRI(origin.m_bHDRI)
    , m_matPrevView()
    , m_matView()
    , m_matViewInv()
    , m_matProj()
    , m_matProjInv()
    , m_mapInstGroup_D{}
    , m_mapInstGroup_F{}
    , m_mapSingleObj{}
    , m_vecSkybox{}
    , m_vecDecal{}
    , m_vecTransparent{}
    , m_vecPostProcess{}
    , m_vecShadow{}
{
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
    Vec3 vCamPos = Transform()->GetWorldPos();
    Matrix matTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

    //// ī�޶��� �� ��, ��, �� �� ������ �������̶� ��ġ��Ű���� ȸ���ϴ� ȸ�����
    Vec3 vCamRot = Transform()->GetWorldRotation();
    Matrix matRotate = Matrix::CreateRotationX(vCamRot.x) * Matrix::CreateRotationY(vCamRot.y) * Matrix::CreateRotationZ(vCamRot.z);
    matRotate = matRotate.Transpose(); // ��������� ������� ��ġ���

    // �̵� x ȸ�� = view ���
    m_matPrevView = m_matView;
    m_matView = matTrans * matRotate;
    m_matViewInv = m_matView.Invert();

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

    m_matProjInv = m_matProj.Invert();

    // Frustum ���
    m_Frustum.finaltick();
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
    // ���� ������ �з����� ����
    render_Clear();

    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        // ī�޶� �ﵵ�� ������ Layer �� �ƴϸ� ����
        if (false == (m_LayerMask & (1 << i)))
            continue;

        CLayer* pLayer = pCurLevel->GetLayer(i);
        const vector<CGameObject*>& vecObjects = pLayer->GetLayerObjects();
        for (size_t j = 0; j < vecObjects.size(); ++j)
        {
            // Ȱ��ȭ ���� üũ
            if (!vecObjects[j]->IsActive())
                continue;

            CRenderComponent* pRenderCom = vecObjects[j]->GetRenderComponent();

            // ������ ����� ���� ������Ʈ�� ����
            if (nullptr == pRenderCom || nullptr == pRenderCom->GetMesh())
                continue;

            // Enable Check
            if (!pRenderCom->IsEnabled())
            {
                continue;
            }

            // FrustumCheck
            if (pRenderCom->IsFrustumCheck())
            {
                if (!m_Frustum.FrustumCheckBySphere(vecObjects[j]->Transform()->GetWorldPos(),
                                                    vecObjects[j]->Transform()->GetWorldRatio() *
                                                        vecObjects[j]->GetRenderComponent()->GetBoundingRadius()))
                    continue;
            }

            // ���׸��� ������ŭ �ݺ�
            UINT iMtrlCount = pRenderCom->GetMtrlCount();
            for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
            {
                // ������ ���ų�, ������ ���̴��� ������ �ȵ� ���
                if (nullptr == pRenderCom->GetMaterial(iMtrl) || nullptr == pRenderCom->GetMaterial(iMtrl)->GetShader())
                {
                    continue;
                }

                // ���̴� �����ο� ���� �з�
                Ptr<CGraphicsShader> pShader = pRenderCom->GetMaterial(iMtrl)->GetShader();
                SHADER_DOMAIN eDomain = pShader->GetDomain();

                switch (eDomain)
                {
                case SHADER_DOMAIN::DOMAIN_SKYBOX:
                    m_vecSkybox.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_DEFERRED:
                case SHADER_DOMAIN::DOMAIN_OPAQUE:
                case SHADER_DOMAIN::DOMAIN_MASKED: {
                    // Shader �� DOMAIN �� ���� �ν��Ͻ� �׷��� �з��Ѵ�.
                    map<ULONG64, vector<tInstObj>>* pMap = NULL;

                    if (eDomain == SHADER_DOMAIN::DOMAIN_DEFERRED)
                    {
                        pMap = &m_mapInstGroup_D;
                    }
                    else if (eDomain == SHADER_DOMAIN::DOMAIN_OPAQUE || eDomain == SHADER_DOMAIN::DOMAIN_MASKED)
                    {
                        pMap = &m_mapInstGroup_F;
                    }
                    else
                    {
                        assert(nullptr);
                        continue;
                    }

                    uInstID uID = {};
                    uID.llID = pRenderCom->GetInstID(iMtrl);

                    // ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
                    if (0 == uID.llID)
                        continue;

                    map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
                    if (iter == pMap->end())
                    {
                        pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{vecObjects[j], iMtrl}}));
                    }
                    else
                    {
                        iter->second.push_back(tInstObj{vecObjects[j], iMtrl});
                    }
                }
                break;
                case SHADER_DOMAIN::DOMAIN_DECAL:
                    m_vecDecal.push_back(vecObjects[j]);
                    break;
                case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
                case SHADER_DOMAIN::DOMAIN_TRANSPARENT_BACKGROUND:
                    m_vecTransparent.push_back(tInstObj{vecObjects[j], iMtrl});
                    break;
                case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
                    m_vecPostProcess.push_back(vecObjects[j]);
                    break;
                }
            }
        }
    }
}

void CCamera::SortShadowMapObject(UINT _MobilityType)
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
            // Ȱ��ȭ ���� üũ
            if (!vecObjects[j]->IsActive())
                continue;

            CRenderComponent* pRenderCom = vecObjects[j]->GetRenderComponent();

            // ������ ����� ���� ������Ʈ�� ����
            if (nullptr == pRenderCom || nullptr == pRenderCom->GetMesh())
                continue;

            // Enable, CastShadow Check
            if (!pRenderCom->IsEnabled() || !vecObjects[j]->GetRenderComponent()->IsCastShadow())
            {
                continue;
            }

            // _MobilityType�� 0 �ΰ�� ������ �߰�
            // �� ���� ��쿡�� RenderComponent�� �ɼ� Ȯ��
            if (0 == _MobilityType || (UINT)vecObjects[j]->Transform()->GetMobilityType() & _MobilityType)
            {
                m_vecShadow.push_back(vecObjects[j]);
            }
        }
    }
}

void CCamera::render_Deferred()
{
    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matPrevView = m_matPrevView;
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matViewInv;
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProjInv;

    // SkyBox Bind
    for (size_t i = 0; i < m_vecSkybox.size(); ++i)
    {
        m_vecSkybox[i]->SkyBox()->UpdateData();
    }

    // Deferred Render Pass
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
    render_Inst(m_mapInstGroup_D);

    // Decal Pass
    render_Decal();

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
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LDRI)->OMSet();
    }

    // Merge Pass
    render_Merge();
}

void CCamera::render_Forward()
{
    // ����� view ��İ� proj ����� ���������� ��Ƶд�.
    g_Transform.matPrevView = m_matPrevView;
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matViewInv;
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProjInv;

    // RenderTarget ����
    if (m_bHDRI)
    {
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDRI)->OMSet();
    }
    else
    {
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LDRI)->OMSet();
    }

    // SkyBox Bind
    for (size_t i = 0; i < m_vecSkybox.size(); ++i)
    {
        m_vecSkybox[i]->SkyBox()->UpdateData();
    }

#ifndef DISTRIBUTE
    // OutLine Pass
    render_OutLine();
#endif // DISTRIBUTE

    // Main Render Pass
    render(m_vecSkybox);
    render_Inst(m_mapInstGroup_F);

    // Transparent
    render_Transparent();

#ifndef DISTRIBUTE
    // IDMap Pass
    if (0 == g_Global.g_MousePickingMode)
    {
        CRenderMgr::GetInst()->GetMRT(MRT_TYPE::IDMAP)->OMSet();
        render_IDMap();
    }
#endif // DISTRIBUTE

    // SkyBox Clear
    for (size_t i = 0; i < m_vecSkybox.size(); ++i)
    {
        m_vecSkybox[i]->SkyBox()->ClearData();
    }

    // Clear
    render_Clear();
}

void CCamera::render_Inst(const map<ULONG64, vector<tInstObj>>& _Group)
{
    m_mapSingleObj.clear();

    // Deferred object render
    tInstancingData tInstData = {};

    for (auto& pair : _Group)
    {
        // �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
        if (pair.second.empty())
            continue;

        // instancing ���� ���� �̸��̰ų�
        // Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
        // Mesh Render �� �ν��Ͻ� ����
        // Shader �� Instancing �� �������� �ʴ°��
        constexpr UINT InstanceCount = 5;
        if (pair.second.size() <= InstanceCount || pair.second[0].pObj->Animator2D() ||
            pair.second[0].pObj->GetRenderComponent() != pair.second[0].pObj->MeshRender() ||
            nullptr == pair.second[0].pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst())
        {
            // �ش� ��ü���� ���� ���������� ��ȯ
            for (UINT i = 0; i < pair.second.size(); ++i)
            {
                map<INT_PTR, vector<tInstObj>>::iterator iter = m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

                if (iter != m_mapSingleObj.end())
                {
                    iter->second.push_back(pair.second[i]);
                }
                else
                {
                    m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
                }
            }
            continue;
        }

        CGameObject* pObj = pair.second[0].pObj;
        Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
        Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

        // Instancing ���� Ŭ����
        CInstancingBuffer::GetInst()->Clear();

        int iRowIdx = 0;
        bool bHasAnim3D = false;
        for (UINT i = 0; i < pair.second.size(); ++i)
        {
            tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
            tInstData.matWorldInvTranspose = tInstData.matWorld;
            tInstData.matWorldInvTranspose.Translation(Vec3(0.0f));
            tInstData.matWorldInvTranspose = tInstData.matWorldInvTranspose.Transpose().Invert();
            tInstData.matViewProj = m_matView * m_matProj;
            tInstData.matPrevTransform = pair.second[i].pObj->Transform()->GetPrevWorldMat() * m_matPrevView * m_matProj;

            if (pair.second[i].pObj->Animator() && pair.second[i].pObj->Animator()->IsValid())
            {
                pair.second[i].pObj->Animator()->UpdateData();
                tInstData.iRowIdx = iRowIdx++;
                CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator()->GetFinalBoneMatBuffer(),
                                                                   pair.second[i].pObj->Animator()->GetPrevFinalBoneMatBuffer());
                bHasAnim3D = true;
            }
            else
            {
                tInstData.iRowIdx = -1;
            }

            if (pair.second[i].pObj->MeshRender() && pair.second[i].pObj->MeshRender()->IsUseMotionBlur())
            {
                tInstData.iMotionBlur = true;
            }
            else
            {
                tInstData.iMotionBlur = false;
            }

            CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
        }

        // �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
        CInstancingBuffer::GetInst()->SetData();

        if (bHasAnim3D)
        {
            pMtrl->SetAnim3D(true); // Animation Mesh �˸���
            pMtrl->SetBoneCount(pMesh->GetBoneCount());
        }

#ifdef DISTRIBUTE
        pMtrl->UpdateData_Inst();
        pMesh->render_instancing(pair.second[0].iMtrlIdx);
#else
        if (g_Global.g_DrawAsWireFrame)
        {
            RS_TYPE originRSType = pMtrl->GetShader()->GetRSType();
            pMtrl->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
            pMtrl->UpdateData_Inst();
            pMesh->render_instancing(pair.second[0].iMtrlIdx);
            pMtrl->GetShader()->SetRSType(originRSType);
        }
        else
        {
            pMtrl->UpdateData_Inst();
            pMesh->render_instancing(pair.second[0].iMtrlIdx);
        }
#endif // DISTRIBUTE

        // ����
        if (bHasAnim3D)
        {
            pMtrl->SetAnim3D(false); // Animation Mesh �˸���
            pMtrl->SetBoneCount(0);
        }
    }

    // ���� ������
    for (auto& pair : m_mapSingleObj)
    {
        if (pair.second.empty())
            continue;

        for (auto& instObj : pair.second)
        {
#ifdef DISTRIBUTE
            instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
#else
            if (g_Global.g_DrawAsWireFrame)
            {
                Ptr<CGraphicsShader> pSingleObjShader = instObj.pObj->GetRenderComponent()->GetMaterial(instObj.iMtrlIdx)->GetShader();
                RS_TYPE originRSType = pSingleObjShader->GetRSType();
                pSingleObjShader->SetRSType(RS_TYPE::WIRE_FRAME);
                instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
                pSingleObjShader->SetRSType(originRSType);
            }
            else
            {
                instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
            }
#endif
        }
    }
}

void CCamera::render_Decal()
{
    CMRT* pDeferredPBRMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED);

    static Ptr<CMaterial> pDecalMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DecalMtrl");

    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();

    for (size_t i = 0; i < m_vecDecal.size(); ++i)
    {
        m_vecDecal[i]->render();
    }
}

void CCamera::render_SSAO()
{
    CMRT* pSSAOMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SSAO);
    Ptr<CTexture> pSSAOTex = pSSAOMRT->GetRenderTargetTex(0);

    if (!g_Global.g_EnableSSAO)
    {
        pSSAOTex->UpdateData(30);
        return;
    }

    CTexture::Clear(30);

    pSSAOMRT->OMSet();

    // SSAO
    static Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
    static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SSAOMtrl");
    pMtrl->UpdateData();
    pMesh->render(0);

    // Blur
    CRenderMgr::GetInst()->BlurTexture(pSSAOTex, 3);

    // Bind
    CONTEXT->OMSetRenderTargets(0, NULL, NULL);
    pSSAOTex->UpdateData(30);
}

void CCamera::render_Light()
{
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();

    const vector<CLight*>& vecLight = CRenderMgr::GetInst()->GetvecLight();
    for (UINT Idx = 0; Idx < vecLight.size(); ++Idx)
    {
        vecLight[Idx]->render_Deferred(Idx);
    }
}

void CCamera::render_Merge()
{
    static Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
    static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UnrealPBRDeferredMergeMtrl");
    // static Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Merge_DeferredMtrl"); // Phong

    pMtrl->UpdateData();
    pMesh->render(0);
}

bool AlphaSortingComp(const tInstObj& _ObjA, const tInstObj& _ObjB)
{
    Vec4 ObjAViewPos = Vec4(0.f, 0.f, 0.f, 1.f);
    Vec4 ObjBViewPos = Vec4(0.f, 0.f, 0.f, 1.f);

    ObjAViewPos = Vec4::Transform(ObjAViewPos, _ObjA.pObj->Transform()->GetWorldMat() * g_Transform.matView);
    ObjBViewPos = Vec4::Transform(ObjBViewPos, _ObjB.pObj->Transform()->GetWorldMat() * g_Transform.matView);

    SHADER_DOMAIN ObjAShaderDomain = _ObjA.pObj->GetRenderComponent()->GetMaterial(_ObjA.iMtrlIdx)->GetShader()->GetDomain();
    SHADER_DOMAIN ObjBShaderDomain = _ObjB.pObj->GetRenderComponent()->GetMaterial(_ObjB.iMtrlIdx)->GetShader()->GetDomain();
    // �� Shader�� Transparent Background �� ��� �������� ����
    if (!(ObjAShaderDomain == SHADER_DOMAIN::DOMAIN_TRANSPARENT_BACKGROUND && ObjBShaderDomain == SHADER_DOMAIN::DOMAIN_TRANSPARENT_BACKGROUND))
    {
        // �� Shader�� �ϳ��� BackGround �� ���
        if (ObjAShaderDomain == SHADER_DOMAIN::DOMAIN_TRANSPARENT_BACKGROUND)
        {
            return true;
        }
        else if (ObjBShaderDomain == SHADER_DOMAIN::DOMAIN_TRANSPARENT_BACKGROUND)
        {
            return false;
        }
    }

    // ��������
    return ObjAViewPos.z > ObjBViewPos.z;
}

void CCamera::render_Transparent()
{
    // Alpha Sorting
    sort(m_vecTransparent.begin(), m_vecTransparent.end(), AlphaSortingComp);

    // Render
    for (tInstObj& instObj : m_vecTransparent)
    {
#ifdef DISTRIBUTE
        instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
#else
        if (g_Global.g_DrawAsWireFrame)
        {
            Ptr<CGraphicsShader> pSingleObjShader = instObj.pObj->GetRenderComponent()->GetMaterial(instObj.iMtrlIdx)->GetShader();
            RS_TYPE originRSType = pSingleObjShader->GetRSType();
            pSingleObjShader->SetRSType(RS_TYPE::WIRE_FRAME);
            instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
            pSingleObjShader->SetRSType(originRSType);
        }
        else
        {
            instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
        }
#endif
    }
}

void CCamera::render_DepthOnly(Ptr<CTexture> _DepthMapTex)
{
    g_Transform.matView = m_matView;
    g_Transform.matViewInv = m_matViewInv;
    g_Transform.matProj = m_matProj;
    g_Transform.matProjInv = m_matProjInv;

    CONTEXT->ClearDepthStencilView(_DepthMapTex->GetDSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    CONTEXT->OMSetRenderTargets(0, NULL, _DepthMapTex->GetDSV().Get());
    CDevice::GetInst()->SetViewport((float)_DepthMapTex->GetWidth(), (float)_DepthMapTex->GetHeight());

    render_DepthOnly(m_vecShadow);

    m_vecShadow.clear();

    CONTEXT->OMSetRenderTargets(0, NULL, NULL);
}

void CCamera::render_Clear()
{
    m_mapInstGroup_D.clear();
    m_mapInstGroup_F.clear();
    m_mapSingleObj.clear();

    m_vecSkybox.clear();
    m_vecDecal.clear();
    m_vecTransparent.clear();
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
            RS_TYPE originRSType = _vecObj[i]->GetRenderComponent()->GetMaterial(0)->GetShader()->GetRSType();
            _vecObj[i]->GetRenderComponent()->GetMaterial(0)->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
            _vecObj[i]->render();
            _vecObj[i]->GetRenderComponent()->GetMaterial(0)->GetShader()->SetRSType(originRSType);
        }
        else
        {
            _vecObj[i]->render();
        }
#endif // DISTRIBUTE
    }
}

void CCamera::render_OutLine()
{
    CGameObject* pSelectedObj = CEditorMgr::GetInst()->GetSelectedObject();
    if (nullptr == pSelectedObj || !pSelectedObj->IsActive())
        return;

    CRenderComponent* pRenderCom = pSelectedObj->GetRenderComponent();

    // ������ ����� ���� ������Ʈ�� ����
    if (nullptr == pRenderCom || nullptr == pRenderCom->GetMesh() || g_Global.g_DrawAsWireFrame)
        return;

    // OutLine Pass
    static Ptr<CMaterial> OutLineMtrl2D = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"2D_OutLineMtrl");
    static Ptr<CMaterial> OutLineMtrl3D = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"3D_OutLineMtrl");
    if (g_Global.g_RenderOutline)
    {
        pSelectedObj->render(PROJ_TYPE::ORTHOGRAPHIC == m_ProjType ? OutLineMtrl2D : OutLineMtrl3D);
    }

    // NormalLine Pass
    static Ptr<CMaterial> NormalLineMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"NormalLineMtrl");
    if (NormalLineMtrl->GetMtrlConst().arrInt[0])
    {
        pSelectedObj->render(NormalLineMtrl);
    }
}

void CCamera::render_DepthOnly(vector<CGameObject*>& _vecObj)
{
    static Ptr<CMaterial> DepthOnlyMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DepthOnlyMtrl");
    for (size_t i = 0; i < _vecObj.size(); i++)
    {
        _vecObj[i]->render(DepthOnlyMtrl);
    }
}

void CCamera::render_IDMap()
{
    static Ptr<CMaterial> IDMapMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"IDMapMtrl");
    static vector<CGameObject*> vecIDMapObj = {};
    vecIDMapObj.clear();

    for (const auto& pair : m_mapInstGroup_D)
    {
        for (UINT i = 0; i < pair.second.size(); ++i)
        {
            vecIDMapObj.push_back(pair.second[i].pObj);
        }
    }

    for (const auto& pair : m_mapInstGroup_F)
    {
        for (UINT i = 0; i < pair.second.size(); ++i)
        {
            vecIDMapObj.push_back(pair.second[i].pObj);
        }
    }

    // Transparent
    for (tInstObj& instObj : m_vecTransparent)
    {
        vecIDMapObj.push_back(instObj.pObj);
    }

    // Render
    for (const auto& iter : vecIDMapObj)
    {
        if (iter->Camera() || iter->SkyBox())
            continue;

        // ������Ʈ �̸� + ID������ HashID Find
        hash<wstring> hasher;
        int HashID = (int)hasher(iter->GetName()) + iter->GetID();

        IDMapMtrl->SetScalarParam(VEC4_0, HashIDToColor(HashID));
        iter->render(IDMapMtrl);
    }
}

void CCamera::render_Postprocess()
{
    // ��ó��
    if (m_bHDRI)
    {
        CRenderMgr::GetInst()->render_postprocess_HDRI();
    }
    else
    {
        CRenderMgr::GetInst()->render_postprocess_LDRI();
    }

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

    CTexture::Clear(15);

    m_vecPostProcess.clear();
}

void CCamera::Resize(Vec2 Resolution)
{
    if (Resolution.x <= 0.f || Resolution.y <= 0.f) // â �ּ�ȭ ����ó��
        return;

    m_Width = Resolution.x;
    m_AspectRatio = Resolution.x / Resolution.y;
}

tRay CCamera::GetRay()
{
    // ���� ���콺 ��ǥ
    Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

    float NdcMouseX = 0.0f;
    float NdcMouseY = 0.0f;

    // Mouse Pos �� NDC
    if (CEditorMgr::GetInst()->IsEnabled())
    {
        vMousePos = CEditorMgr::GetInst()->GetViewportMousePos();
        Vec2 ViewportSize = CEditorMgr::GetInst()->GetViewportSize();

        NdcMouseX = vMousePos.x * 2.0f / ViewportSize.x - 1.0f;
        NdcMouseY = -vMousePos.y * 2.0f / ViewportSize.y + 1.0f;
    }
    else
    {
        Vec2 WindowSize = CDevice::GetInst()->GetRenderResolution();
        NdcMouseX = vMousePos.x * 2.0f / WindowSize.x - 1.0f;
        NdcMouseY = -vMousePos.y * 2.0f / WindowSize.y + 1.0f;
    }

    // NDC �� World
    Vector3 cursorNdcNear = Vector3(NdcMouseX, NdcMouseY, 0);
    Vector3 cursorNdcFar = Vector3(NdcMouseX, NdcMouseY, 1);
    Matrix inverseProjView = m_matProjInv * m_matViewInv;
    Vector3 NearWorld = Vector3::Transform(cursorNdcNear, inverseProjView);
    Vector3 FarWorld = Vector3::Transform(cursorNdcFar, inverseProjView);

    // World ��ǥ Ray
    tRay ray = {};
    ray.vStart = Transform()->GetWorldPos();
    ray.vDir = (NearWorld - FarWorld).Normalize();

    return ray;
}

UINT CCamera::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

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

    MemoryByte += sizeof(PROJ_TYPE);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(int);
    MemoryByte += sizeof(bool);

    return MemoryByte;
}

UINT CCamera::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

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

    MemoryByte += sizeof(PROJ_TYPE);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(UINT);
    MemoryByte += sizeof(int);
    MemoryByte += sizeof(bool);

    return MemoryByte;
}
