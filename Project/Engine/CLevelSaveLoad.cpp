#include "pch.h"
#include "CLevelSaveLoad.h"

#include "CCollisionMgr.h"
#include <Scripts\\CScriptMgr.h>

#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "components.h"
#include "CScript.h"

wstring CLevelSaveLoad::Level_extension = L".tLevel";

void CLevelSaveLoad::SaveLevel(CLevel* _Level, const wstring& _LevelFileName)
{
    assert(_Level);

    if (_Level->GetState() != LEVEL_STATE::STOP)
        return;

    // Save �� ������ Ȯ���ڷ� �����ϵ��� ����
    wstring strLevelPath = CPathMgr::GetContentPath();
    strLevelPath += L"Levels\\";
    strLevelPath += _LevelFileName;

    std::filesystem::path filePath = strLevelPath;

    // Ȯ���ڰ� �Էµ��� ���� ���
    if ("" == filePath.extension())
    {
        strLevelPath += Level_extension;
    }
    // Ȯ���ڰ� �߸� �Էµ� ���
    else if (Level_extension != filePath.extension())
    {
        strLevelPath = filePath.replace_extension(Level_extension);
    }

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strLevelPath.c_str(), L"wb");

    if (nullptr == pFile)
        return;

    // ������ �̸�
    SaveWString(_Level->GetName(), pFile);

    // ������ ���̾� ����
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        SaveLayer(_Level->GetLayer(i), pFile);
    }

    // Collision Layer
    for (UINT i = 0; i < LAYER_MAX; i++)
    {
        UINT row = CCollisionMgr::GetInst()->GetCollisionLayer(i);
        fwrite(&row, sizeof(UINT), 1, pFile);
    }

    fclose(pFile);
}

void CLevelSaveLoad::SaveLayer(CLayer* _Layer, FILE* _File)
{
    // Layer �� �̸� ����
    SaveWString(_Layer->GetName(), _File);

    // Layer �� �����ϰ� �ִ� GameObject ���� ����
    const vector<CGameObject*>& vecObject = _Layer->GetParentObjects();

    size_t ObjCount = vecObject.size();
    fwrite(&ObjCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < vecObject.size(); ++i)
    {
        SaveGameObject(vecObject[i], _File);
    }
}

void CLevelSaveLoad::SaveGameObject(CGameObject* _Obj, FILE* _File)
{
    // GameObject �� �̸��� ����
    SaveWString(_Obj->GetName(), _File);

    // ���̾� �ε��� ����
    int layerIdx = _Obj->GetLayerIdx();
    fwrite(&layerIdx, sizeof(int), 1, _File);

    // ������Ʈ ������ ����
    for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; ++i)
    {
        if (i == (UINT)COMPONENT_TYPE::END)
        {
            // ������Ʈ Ÿ�� ����
            fwrite(&i, sizeof(UINT), 1, _File);
            break;
        }

        CComponent* pCom = _Obj->GetComponent((COMPONENT_TYPE)i);
        if (nullptr == pCom)
            continue;

        // ������Ʈ Ÿ�� ���� ����
        fwrite(&i, sizeof(UINT), 1, _File);

        // �ش� ������Ʈ�� ������ ������ ����
        pCom->SaveToLevelFile(_File);
    }

    // ��ũ��Ʈ ���� ����
    const vector<CScript*>& vecScripts = _Obj->GetScripts();
    size_t ScriptCount = vecScripts.size();

    // ��ũ��Ʈ ���� ����
    fwrite(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < vecScripts.size(); ++i)
    {
        SaveWString(CScriptMgr::GetScriptName(vecScripts[i]), _File);
        vecScripts[i]->SaveToLevelFile(_File);
    }

    // �ڽ� ������Ʈ�� ������ �ڽ� ������Ʈ ���� ����
    const vector<CGameObject*>& vecChild = _Obj->GetChildObject();
    size_t childcount = vecChild.size();
    fwrite(&childcount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < childcount; ++i)
    {
        SaveGameObject(vecChild[i], _File);
    }
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _LevelFileName)
{
    // Level �� �ҷ��� ��� ����
    wstring strLevelPath = CPathMgr::GetContentPath();
    strLevelPath += L"Levels\\";
    strLevelPath += _LevelFileName;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strLevelPath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        MessageBox(nullptr, L"���� ��ΰ� �ùٸ��� �ʰų�, ������ �������� ����", L"���� �ε� ����", MB_OK);
        return nullptr;
    }

    // ������ �̸��� �д´�.
    CLevel* pLevel = new CLevel;
    wstring strLevelName;
    LoadWString(strLevelName, pFile);
    pLevel->SetName(strLevelName);

    // Layer �ε�
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        LoadLayer(pLevel->GetLayer(i), pFile);
    }

    // Collision Layer
    for (UINT i = 0; i < LAYER_MAX; i++)
    {
        UINT row = 0;
        fread(&row, sizeof(UINT), 1, pFile);
        CCollisionMgr::GetInst()->SetCollisionLayer(i, row);
    }

    fclose(pFile);

    return pLevel;
}

void CLevelSaveLoad::LoadLayer(CLayer* _Layer, FILE* _File)
{
    // Layer �� �̸� ����
    wstring strLayerName;
    LoadWString(strLayerName, _File);
    _Layer->SetName(strLayerName);

    // Layer �� �����ϴ� GameObject ���� �ҷ��´�.
    size_t ObjCount = 0;
    fread(&ObjCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < ObjCount; ++i)
    {
        _Layer->AddObject(LoadGameObject(_File), false);
    }
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File)
{
    CGameObject* pObject = new CGameObject;

    // GameObject �� �̸��� �ε�
    wstring strName;
    LoadWString(strName, _File);
    pObject->SetName(strName);

    // ���̾� �ε��� ����
    int layerIdx = 0;
    fread(&layerIdx, sizeof(int), 1, _File);
    pObject->m_iLayerIdx = layerIdx;

    // ������Ʈ ������ �ҷ�����
    COMPONENT_TYPE type = COMPONENT_TYPE::END;
    while (true)
    {
        fread(&type, sizeof(UINT), 1, _File);
        if (COMPONENT_TYPE::END == type)
            break;

        CComponent* pComponent = nullptr;

        switch (type)
        {
        case COMPONENT_TYPE::TRANSFORM:
            pComponent = new CTransform;
            break;
        case COMPONENT_TYPE::COLLIDER2D:
            pComponent = new CCollider2D;
            break;
        // case COMPONENT_TYPE::COLLIDER3D:
        //     pComponent = new CCollider3D;
        //     break;
        case COMPONENT_TYPE::ANIMATOR2D:
            pComponent = new CAnimator2D;
            break;
        // case COMPONENT_TYPE::ANIMATOR3D:
        //     pComponent = new CAnimator3D;
        //     break;
        case COMPONENT_TYPE::LIGHT2D:
            pComponent = new CLight2D;
            break;
        case COMPONENT_TYPE::LIGHT3D:
            pComponent = new CLight3D;
            break;
        case COMPONENT_TYPE::CAMERA:
            pComponent = new CCamera;
            break;
        case COMPONENT_TYPE::MESHRENDER:
            pComponent = new CMeshRender;
            break;
        case COMPONENT_TYPE::TILEMAP:
            pComponent = new CTileMap;
            break;
        case COMPONENT_TYPE::PARTICLESYSTEM:
            pComponent = new CParticleSystem;
            break;
        case COMPONENT_TYPE::SKYBOX:
            pComponent = new CSkyBox;
            break;
            // case COMPONENT_TYPE::DECAL:
            //     pComponent = new CDecal;
            //     break;
            // case COMPONENT_TYPE::LANDSCAPE:
            //     pComponent = new CLandScape;
            //     break;
        }

        // �ش� ������Ʈ�� ������ �����͸� �ε�
        pObject->AddComponent(pComponent);
        pComponent->LoadFromLevelFile(_File);
    }

    // ��ũ��Ʈ ���� �б�
    size_t ScriptCount = 0;
    fread(&ScriptCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < ScriptCount; ++i)
    {
        wstring strScriptName;
        LoadWString(strScriptName, _File);

        CScript* pScript = CScriptMgr::GetScript(strScriptName);
        pObject->AddComponent(pScript);
        pScript->LoadFromLevelFile(_File);
    }

    // �ڽ� ������Ʈ�� ������ �ڽ� ������Ʈ ���� ����
    size_t childcount = 0;
    fread(&childcount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < childcount; ++i)
    {
        pObject->AddChild(LoadGameObject(_File));
    }

    return pObject;
}
