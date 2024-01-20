#include "pch.h"
#include "CMaterialEditor.h"
#include "CEditorMgr.h"

CMaterialEditor::CMaterialEditor()
    : CEditor(EDITOR_TYPE::MATERIAL)
    , m_Mtrl()
{
}

CMaterialEditor::~CMaterialEditor()
{
}

void CMaterialEditor::render()
{
    if (nullptr == m_Mtrl.Get())
        return;

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    string name = ToString(m_Mtrl->GetName());
    strcpy_s(buffer, sizeof(buffer), name.c_str());
    ImGui::InputText(ImGuiLabelPrefix("Material Name").c_str(), buffer, sizeof(buffer));

    const tMtrlConst& MtrlConst = m_Mtrl->GetMtrlConst();

    Vec4 ambient = MtrlConst.mtrl.vAmb;
    Vec4 diffuse = MtrlConst.mtrl.vDiff;
    Vec4 specular = MtrlConst.mtrl.vSpec;
    Vec4 environment = MtrlConst.mtrl.vEmv;

    bool bDirty = false;

    if (ImGui::ColorEdit3(ImGuiLabelPrefix("Ambient").c_str(), &ambient.x))
        bDirty = true;

    if (ImGui::ColorEdit3(ImGuiLabelPrefix("diffuse").c_str(), &diffuse.x))
        bDirty = true;

    if (ImGui::ColorEdit3(ImGuiLabelPrefix("specular").c_str(), &specular.x))
        bDirty = true;

    if (ImGui::ColorEdit3(ImGuiLabelPrefix("environment").c_str(), &environment.x))
        bDirty = true;

    if (bDirty)
        m_Mtrl->SetMaterialCoefficient(ambient, diffuse, specular, environment);

    constexpr float IMAGE_BASE_SIZE = 250.0f;

    // Texture
    for (UINT i = TEX_PARAM::TEX_0; i <= TEX_PARAM::TEX_5; ++i)
    {
        Ptr<CTexture> pTex = m_Mtrl->GetTexParam((TEX_PARAM)i);
        ID3D11ShaderResourceView* pSRV = nullptr;

        if (nullptr != pTex.Get())
            pSRV = pTex->GetSRV().Get();
        else
            pSRV = CAssetMgr::GetInst()->FindAsset<CTexture>(L"missing_texture")->GetSRV().Get();

        if (i == TEX_PARAM::TEX_0)
            ImGui::Text("Texture 0");
        else if (i == TEX_PARAM::TEX_1)
            ImGui::Text("Texture 1");
        else if (i == TEX_PARAM::TEX_2)
            ImGui::Text("Texture 2");
        else if (i == TEX_PARAM::TEX_3)
            ImGui::Text("Texture 3");
        else if (i == TEX_PARAM::TEX_4)
            ImGui::Text("Texture 4");
        else if (i == TEX_PARAM::TEX_5)
            ImGui::Text("Texture 5");

        ImGui::Image((void*)pSRV, ImVec2(IMAGE_BASE_SIZE, IMAGE_BASE_SIZE));

        // Drag & Drop
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                string name = (char*)payload->Data;
                name.resize(payload->DataSize);
                m_Mtrl->SetTexParam((TEX_PARAM)i, CAssetMgr::GetInst()->FindAsset<CTexture>(ToWstring(name)));
            }

            ImGui::EndDragDropTarget();
        }
    }
}

void CMaterialEditor::render(bool* open)
{
    if (!ImGui::Begin("Material Editor", open))
    {
        ImGui::End();
        return;
    }

    render();

    ImGui::End();
}
