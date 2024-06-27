#include "pch.h"
#include "CModelEditor.h"

#include "CRenderMgr.h"
#include <Scripts\\CScriptMgr.h>

#include "CGameObjectEx.h"

#include "components.h"
#include "CScript.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CMRT.h"

CModelEditor::CModelEditor()
    : CEditor(EDITOR_TYPE::MODEL)
    , m_ModelObj(nullptr)
    , m_SelectedBoneIdx(-1)
    , m_FinalBoneMat{}
    , m_ViewportRTTex(nullptr)
    , m_ViewportDSTex(nullptr)
    , m_ViewportCam(nullptr)
    , m_LightObj(nullptr)
    , m_SkyBoxObj(nullptr)
    , m_FloorObj(nullptr)
    , m_LightBuffer(nullptr)
{
}

CModelEditor::~CModelEditor()
{
    if (nullptr != m_ModelObj)
    {
        delete m_ModelObj;
        m_ModelObj = nullptr;
    }

    if (nullptr != m_ViewportCam)
    {
        delete m_ViewportCam->GetOwner();
        m_ViewportCam = nullptr;
    }

    if (nullptr != m_LightObj)
    {
        delete m_LightObj;
        m_LightObj = nullptr;
    }

    if (nullptr != m_SkyBoxObj)
    {
        delete m_SkyBoxObj;
        m_SkyBoxObj = nullptr;
    }

    if (nullptr != m_FloorObj)
    {
        delete m_FloorObj;
        m_FloorObj = nullptr;
    }

    if (nullptr != m_LightBuffer)
    {
        delete m_LightBuffer;
        m_LightBuffer = nullptr;
    }
}

void CModelEditor::init()
{
    // RenderTarget
    m_ViewportRTTex = CAssetMgr::GetInst()->CreateTexture(L"ModelEditorViewportRTTex", 1280, 1280, DXGI_FORMAT_R16G16B16A16_FLOAT,
                                                          D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);
    m_ViewportDSTex = CAssetMgr::GetInst()->CreateTexture(L"ModelEditorViewportDSTex", 1280, 1280, DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                          D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

    // Camera
    CGameObjectEx* pCam = new CGameObjectEx;

    pCam->SetName(L"Editor Camera");
    pCam->AddComponent(new CTransform);
    pCam->AddComponent(new CCamera);
    pCam->AddComponent(CScriptMgr::GetScript(MODELEDITORCAMERAMOVESCRIPT));

    pCam->Transform()->SetRelativePos(Vec3(0.f, 250.f, -250.f));

    m_ViewportCam = pCam->Camera();

    m_ViewportCam->LayerMaskAll();
    m_ViewportCam->SetProjType(PROJ_TYPE::PERSPECTIVE);
    m_ViewportCam->SetFOV(XM_PI / 2.f);
    m_ViewportCam->SetFar(10000.f);
    m_ViewportCam->SetHDRI(true);

    // Light
    m_LightObj = new CGameObjectEx;
    m_LightObj->SetName(L"Directional Light");
    m_LightObj->AddComponent(new CTransform);
    m_LightObj->AddComponent(new CLight);

    m_LightObj->Transform()->SetRelativePos(Vec3(-500.f, 1000.f, -500.f));
    m_LightObj->Transform()->SetDirection(Vec3(1.f, -1.f, 1.f));
    m_LightObj->Light()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
    // m_LightObj->Light()->SetLightRadiance(Vec3(0.5f, 0.5f, 0.5f));
    m_LightObj->Light()->SetLightRadiance(Vec3(1.f, 0.f, 0.f));
    m_LightObj->Light()->SetRadius(10.f);

    // SkyBox
    m_SkyBoxObj = new CGameObjectEx;
    m_SkyBoxObj->SetName(L"SkyBox");
    m_SkyBoxObj->AddComponent(new CTransform);
    m_SkyBoxObj->AddComponent(new CSkyBox);

    m_SkyBoxObj->Transform()->SetRelativePos(Vec3(0.f, 5000.f, 0.f));

    m_SkyBoxObj->SkyBox()->SetEnvTex(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaEnvHDR.dds",
                                                                          L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaEnvHDR.dds"));

    m_SkyBoxObj->SkyBox()->SetBrdfTex(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaBrdf.dds",
                                                                           L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaBrdf.dds"));

    m_SkyBoxObj->SkyBox()->SetDiffuseTex(CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaDiffuseHDR.dds",
                                                                              L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaDiffuseHDR.dds"));

    m_SkyBoxObj->SkyBox()->SetSpecularTex(
        CAssetMgr::GetInst()->Load<CTexture>(L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaSpecularHDR.dds",
                                             L"Texture\\skybox\\EpicQuadPanorama\\EpicQuadPanoramaSpecularHDR.dds"));

    // Floor
    m_FloorObj = new CGameObjectEx;
    m_FloorObj->SetName(L"Floor");
    m_FloorObj->AddComponent(new CTransform);
    m_FloorObj->AddComponent(new CMeshRender);

    m_FloorObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
    m_FloorObj->Transform()->SetRelativeScale(Vec3(500.f, 10.f, 500.f));

    m_FloorObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"BoxMesh"));
    m_FloorObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"UnrealPBRDeferredMtrl"), 0);
    m_FloorObj->MeshRender()->GetMaterial(0)->SetMaterialCoefficient(Vec4(1.f, 1.f, 1.f, 1.f));
    m_FloorObj->MeshRender()->SetFrustumCheck(false);

    // Light Buffer
    m_LightBuffer = new CStructuredBuffer;
    m_LightBuffer->Create(sizeof(tLightInfo), 1, SB_TYPE::READ_ONLY, true);
}

void CModelEditor::tick()
{
    if (nullptr != m_ModelObj)
    {
        m_ModelObj->tick();
    }

    m_ViewportCam->GetOwner()->tick();
    m_LightObj->Transform()->tick();
    m_SkyBoxObj->tick();
    m_FloorObj->tick();
}

void CModelEditor::finaltick()
{
    if (nullptr != m_ModelObj)
    {
        for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
        {
            CComponent* pComp = m_ModelObj->GetComponent((COMPONENT_TYPE)i);
            if (nullptr != pComp)
            {
                if (i == (UINT)COMPONENT_TYPE::ANIMATOR)
                {
                    m_ModelObj->Animator()->finaltick_ModelEditor();
                }
                else
                {
                    pComp->finaltick();
                }
            }
        }

        UINT BoneCount = m_ModelObj->Animator()->GetBoneCount();

        if (m_FinalBoneMat.size() != BoneCount)
        {
            m_FinalBoneMat.resize(BoneCount);
        }

        // ���� Bone ����� �޾ƿ´�.
        m_ModelObj->Animator()->GetFinalBoneMat()->GetData(m_FinalBoneMat.data(), BoneCount);
    }

    m_ViewportCam->GetOwner()->finaltick();
    m_LightObj->Transform()->finaltick();
    m_SkyBoxObj->finaltick();
    m_FloorObj->finaltick();
}

void CModelEditor::render(bool* open)
{
    // =====================================
    // DockSpace
    // =====================================
    ImGuiWindowClass window_class;
    window_class.ClassId = ImGui::GetMainViewport()->ID;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingSplitOther;
    window_class.DockingAllowUnclassed = true;
    ImGui::SetNextWindowClass(&window_class);

    if (!ImGui::Begin(ToString(GetName()).c_str(), open))
    {
        ImGui::End();
        return;
    }

    ImGuiID dockSpace = ImGui::GetID("Model Editor DockSpace");
    ImGui::DockSpace(dockSpace);

    // =====================================
    // Model Editor Render
    // =====================================
    render();

    ImGui::End();
}

void CModelEditor::render()
{
    ImGui_SetWindowClass(GetEditorType());
    DrawViewport();

    ImGui_SetWindowClass(GetEditorType());
    DrawDetails();

    ImGui_SetWindowClass(GetEditorType());
    DrawSkeletonTree();

    ImGui_SetWindowClass(GetEditorType());
    DrawAnimation();
}

void CModelEditor::DrawViewport()
{
    ImGui::Begin("Viewport##ModelEditor");

    // ����Ÿ�� ����
    CONTEXT->ClearRenderTargetView(m_ViewportRTTex->GetRTV().Get(), Vec4(0.f, 0.f, 0.f, 1.f));
    CONTEXT->ClearDepthStencilView(m_ViewportDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    CONTEXT->OMSetRenderTargets(1, m_ViewportRTTex->GetRTV().GetAddressOf(), m_ViewportDSTex->GetDSV().Get());
    CDevice::GetInst()->SetViewport(1280.f, 1280.f);

    // ī�޶� ���� ����
    g_Transform.matView = m_ViewportCam->GetViewMat();
    g_Transform.matViewInv = m_ViewportCam->GetViewInvMat();
    g_Transform.matProj = m_ViewportCam->GetProjMat();
    g_Transform.matProjInv = m_ViewportCam->GetProjInvMat();

    // ���� ���� ����
    g_Global.g_LightCount = 1;

    // ���� ī�޶� ��ġ ���
    g_Global.g_eyeWorld = m_ViewportCam->Transform()->GetWorldPos();

    // ���� ��� ������ ���ε�
    CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL_DATA);
    pGlobalBuffer->SetData(&g_Global);
    pGlobalBuffer->UpdateData();
    pGlobalBuffer->UpdateData_CS();

    // Light ���� ����
    const tLightInfo& info = m_LightObj->Light()->GetLightInfo();

    tLightInfo CopyInfo = {};
    CopyInfo = info;
    CopyInfo.vWorldPos = m_LightObj->Transform()->GetWorldPos();
    CopyInfo.vWorldDir = m_LightObj->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    CopyInfo.viewMat = Matrix();
    CopyInfo.projMat = Matrix();
    CopyInfo.invProj = Matrix();

    m_LightBuffer->SetData(&CopyInfo, 1);
    m_LightBuffer->UpdateData(14);

    // ������
    m_SkyBoxObj->render();
    m_FloorObj->render();
    if (nullptr != m_ModelObj)
    {
        m_ModelObj->render();
    }

    ImGui::Image((void*)m_ViewportRTTex->GetSRV().Get(), ImGui::GetContentRegionAvail());

    // ����Ÿ�� ���󺹱�
    CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
    for (UINT i = 0; i < TEX_PARAM::TEX_END; i++)
    {
        CTexture::Clear(i);
    }

    ImGui::End();
}

void CModelEditor::DrawDetails()
{
    ImGui::Begin("Details##ModelEditor");

    static ImGuiTreeNodeFlags DefaultTreeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
                                                    ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    // ==========================
    // Mesh Data
    // ==========================
    if (ImGui::TreeNodeEx("Mesh Data##ModelEditorDetails", DefaultTreeNodeFlag))
    {
        string ModelName = string();
        if (nullptr != m_ModelObj)
        {
            ModelName = ToString(m_ModelObj->GetName());
        }

        const map<wstring, Ptr<CAsset>>& mapMeshData = CAssetMgr::GetInst()->GetMapAsset(ASSET_TYPE::MESHDATA);

        if (ImGui_ComboUI(ImGui_LabelPrefix("Mesh Data").c_str(), ModelName, mapMeshData))
        {
            Ptr<CMeshData> pMeshData = CAssetMgr::GetInst()->FindAsset<CMeshData>(ToWstring(ModelName));

            if (nullptr != pMeshData)
            {
                SetModel(pMeshData);
            }
        }

        if (ImGui_AlignButton("Load Model", 1.f))
        {
            std::filesystem::path filePath = OpenFileDialog(L"fbx\\", TEXT("FBX Files\0*.fbx\0��� ����(*.*)\0*.*\0"));

            // .fbx ������ �ƴ� ���
            if (L".fbx" != filePath.extension())
            {
                MessageBox(nullptr, L"fbx ���� ������ �ƴմϴ�.", L"�� �ε� ����", MB_OK);
            }
            // ��ο� Content ������ ���Ե��� ���� ���
            else if (string::npos == wstring(filePath).find(CPathMgr::GetContentPath()))
            {
                MessageBox(nullptr, L"Content ������ �����ϴ� ���� �ƴմϴ�.", L"�� �ε� ����", MB_OK);
            }
            // ���, �ݱ� ��ư üũ�� Ŭ������ ���� ���
            else if (!filePath.empty())
            {
                Ptr<CMeshData> pMeshData = CAssetMgr::GetInst()->LoadFBX(filePath.lexically_relative(CPathMgr::GetContentPath()));

                if (nullptr != pMeshData)
                {
                    SetModel(pMeshData);
                }
            }
        }

        ImGui::TreePop();
    }

    // ==========================
    // Bone
    // ==========================
    if (ImGui::TreeNodeEx("Bone##ModelEditorDetails", DefaultTreeNodeFlag))
    {
        if (nullptr != m_ModelObj && -1 < m_SelectedBoneIdx)
        {
            const tMTBone& CurBone = m_ModelObj->MeshRender()->GetMesh()->GetBones()->at(m_SelectedBoneIdx);
            ImGui_InputText("Bone Name", ToString(CurBone.strBoneName).c_str());
        }

        ImGui::TreePop();
    }

    // ==========================
    // Transforms
    // ==========================
    if (ImGui::TreeNodeEx("Transforms##ModelEditorDetails", DefaultTreeNodeFlag))
    {
        if (nullptr != m_ModelObj && -1 < m_SelectedBoneIdx)
        {
            const tMTBone& CurBone = m_ModelObj->MeshRender()->GetMesh()->GetBones()->at(m_SelectedBoneIdx);

            // Bone
            if (ImGui::TreeNodeEx("Bone##ModelEditorDetailsTransforms", DefaultTreeNodeFlag))
            {
                Vec3 pos = Vec3();
                Vec3 rot = Vec3();
                Vec3 scale = Vec3();
                ImGuizmo::DecomposeMatrixToComponents(*m_FinalBoneMat[m_SelectedBoneIdx].m, pos, rot, scale);

                ImGui_DrawVec3Control("Location", pos, 10.f);
                ImGui_DrawVec3Control("Rotation", rot, 1.f);
                ImGui_DrawVec3Control("Scale", scale, 1.f, 1.f, D3D11_FLOAT32_MAX, 1.f);

                ImGui::TreePop();
            }

            // Offset
            if (ImGui::TreeNodeEx("Offset##ModelEditorDetailsTransforms", DefaultTreeNodeFlag))
            {
                const tMTBone& CurBone = m_ModelObj->MeshRender()->GetMesh()->GetBones()->at(m_SelectedBoneIdx);

                Vec3 pos = Vec3();
                Vec3 rot = Vec3();
                Vec3 scale = Vec3();
                ImGuizmo::DecomposeMatrixToComponents(*CurBone.matOffset.m, pos, rot, scale);

                ImGui_DrawVec3Control("Location", pos, 10.f);
                ImGui_DrawVec3Control("Rotation", rot, 1.f);
                ImGui_DrawVec3Control("Scale", scale, 1.f, 1.f, D3D11_FLOAT32_MAX, 1.f);

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void CModelEditor::DrawSkeletonTree()
{
    ImGui::Begin("Skeleton Tree##ModelEditor");

    if (nullptr != m_ModelObj)
    {
        int NodeOpenFlag = 0;
        if (ImGui::Button("Expand All"))
        {
            NodeOpenFlag = 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Collapse All"))
        {
            NodeOpenFlag = -1;
        }

        ImGui::Separator();

        SkeletonRe(*m_ModelObj->MeshRender()->GetMesh()->GetBones(), 0, NodeOpenFlag);
    }

    ImGui::End();
}

void CModelEditor::SkeletonRe(const vector<tMTBone>& _vecBone, int _BoneIdx, int _NodeOpenFlag)
{
    if (-1 == _NodeOpenFlag)
    {
        ImGui::SetNextItemOpen(false);
    }
    else if (1 == _NodeOpenFlag)
    {
        ImGui::SetNextItemOpen(true);
    }

    static ImGuiTreeNodeFlags DefaultTreeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;

    bool opened = ImGui::TreeNodeEx(ToString(_vecBone[_BoneIdx].strBoneName).c_str(),
                                    m_SelectedBoneIdx == _BoneIdx ? ImGuiTreeNodeFlags_Selected : 0 | DefaultTreeNodeFlag);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        m_SelectedBoneIdx = _BoneIdx;
    }

    if (opened)
    {
        for (UINT i = 0; i < (UINT)_vecBone.size(); ++i)
        {
            // ������ BoneIdx�� �θ��� BoneIdx�� ���� ���
            if (i == _BoneIdx)
                continue;

            if (_BoneIdx == _vecBone[i].iParentIndx)
            {
                SkeletonRe(_vecBone, i, _NodeOpenFlag);
            }
        }

        ImGui::TreePop();
    }
}

void CModelEditor::DrawAnimation()
{
    ImGui::Begin("Animation##ModelEditor");
    if (nullptr != m_ModelObj && m_ModelObj->Animator())
    {
        static ImGuiTreeNodeFlags DefaultTreeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                                        ImGuiTreeNodeFlags_FramePadding;

        CAnimator* pAnimator = m_ModelObj->Animator();
        Ptr<CMesh> pSkeletalMesh = m_ModelObj->Animator()->GetSkeletalMesh();

        // Skeletal Mesh
        if (ImGui::TreeNodeEx("Skeletal Mesh##ModelEditor Animation", DefaultTreeNodeFlag))
        {
            string name;
            if (nullptr != pSkeletalMesh)
                name = ToString(pSkeletalMesh->GetName());

            ImGui_InputText("Skeletal Mesh", name);

            ImGui::TreePop();
        }

        // Animation
        if (nullptr != pSkeletalMesh)
        {
            if (ImGui::TreeNodeEx("Animation##ModelEditor Animation", DefaultTreeNodeFlag))
            {
                // =====================
                // Animations
                // =====================
                const vector<tMTAnimClip>* vecAnimClip = pSkeletalMesh->GetAnimClip();

                int CurClipIdx = pAnimator->GetCurClipIdx();
                const tMTAnimClip& CurClip = vecAnimClip->at(CurClipIdx);

                string CurClipName = ToString(CurClip.strAnimName);

                int ChangedClipIdx = -1;
                ImGui::Text("Animations");
                if (ImGui::BeginCombo("##Anim", CurClipName.c_str()))
                {
                    for (int i = 0; i < vecAnimClip->size(); i++)
                    {
                        string ClipName = ToString(vecAnimClip->at(i).strAnimName);
                        bool is_selected = (CurClipName == ClipName);
                        if (ImGui::Selectable(ClipName.c_str(), is_selected))
                        {
                            CurClipName = ClipName;
                            ChangedClipIdx = i;
                        }

                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ChangedClipIdx >= 0)
                {
                    pAnimator->SetCurClipIdx(ChangedClipIdx);
                }

                // Frame Index
                int FrameIdx = pAnimator->GetCurFrameIdx();
                if (ImGui::SliderInt(ImGui_LabelPrefix("Frame Index").c_str(), &FrameIdx, CurClip.iStartFrame, CurClip.iEndFrame))
                {
                    pAnimator->SetFrameIdx(FrameIdx);
                }

                bool bPlaying = pAnimator->IsPlaying();
                if (ImGui::Checkbox(ImGui_LabelPrefix("Play").c_str(), &bPlaying))
                    pAnimator->SetPlay(bPlaying);

                bool bRepeat = pAnimator->IsRepeat();
                if (ImGui::Checkbox(ImGui_LabelPrefix("Repeat").c_str(), &bRepeat))
                    pAnimator->SetRepeat(bRepeat);

                float PlaySpeed = pAnimator->GetPlaySpeed();
                if (ImGui::DragFloat(ImGui_LabelPrefix("Play Speed").c_str(), &PlaySpeed, 0.01f, 0.f, 100.f))
                    pAnimator->SetPlaySpeed(PlaySpeed);

                // =====================
                // Animation Clip Info
                // =====================
                ImGui::Text("Clip Infomation");
                static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                               ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedSame;
                if (ImGui::BeginTable("##AnimClip", 3, flags, ImVec2(600.f, 150.f)))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Animation Name");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", ToString(CurClip.strAnimName).c_str());

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Clip Index");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", CurClipIdx);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Frame Rate");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", (int)pAnimator->GetFrameRate());

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Frame Range");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", CurClip.iStartFrame);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", CurClip.iEndFrame);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Frame Length");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", CurClip.iFrameLength);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Time Range");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.3f", CurClip.dStartTime);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%.3f", CurClip.dEndTime);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Time Length");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.3f", CurClip.dTimeLength);

                    ImGui::EndTable();
                }

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void CModelEditor::SetModel(Ptr<CMeshData> _MeshData)
{
    if (nullptr != m_ModelObj)
    {
        delete m_ModelObj;
        m_ModelObj = nullptr;
    }

    // Bone ������ �ʱ�ȭ
    m_SelectedBoneIdx = -1;
    m_FinalBoneMat.clear();

    m_ModelObj = _MeshData->InstantiateEx();

    m_ModelObj->Transform()->SetRelativePos(Vec3(0.f, 100.f, 0.f));
    m_ModelObj->Transform()->SetRelativeRotation(Vec3(-XM_PIDIV2, 0.f, 0.f));
    m_ModelObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));

    m_ModelObj->MeshRender()->SetFrustumCheck(false);

    m_ModelObj->Animator()->SetPlay(false);
}
