#include "pch.h"
#include "CLevelSaveLoad.h"

#include "CPhysics2DMgr.h"
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

    // ���� �̸�
    _Level->SetName(_LevelFileName);
    SaveWStringToFile(_Level->GetName(), pFile);

    // ������ ���̾� ����
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        SaveLayer(_Level->GetLayer(i), pFile);
    }

    // �ݸ��� ���̾� ����
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        unsigned short ColLayer = CPhysics2DMgr::GetInst()->GetCollisionLayer(i);
        fwrite(&ColLayer, sizeof(unsigned short), 1, pFile);
    }

    fclose(pFile);
}

void CLevelSaveLoad::SaveLayer(CLayer* _Layer, FILE* _File)
{
    // Layer �� �̸� ����
    SaveWStringToFile(_Layer->GetName(), _File);

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
    SaveWStringToFile(_Obj->GetName(), _File);

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
        SaveWStringToFile(CScriptMgr::GetScriptName(vecScripts[i]), _File);
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
    _wfopen_s(&pFile, strLevelPath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        MessageBox(nullptr, L"���� ��ΰ� �ùٸ��� �ʰų�, ������ �������� ����", L"���� �ε� ����", MB_OK);
        return nullptr;
    }

    // ������ �̸� �ε�
    CLevel* pLevel = new CLevel;
    wstring strLevelName;
    LoadWStringFromFile(strLevelName, pFile);
    pLevel->SetName(strLevelName);

    // Layer �ε�
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        LoadLayer(pLevel->GetLayer(i), pFile);
    }

    // �ݸ��� ���̾� �ε�
    for (UINT i = 0; i < LAYER_MAX; ++i)
    {
        unsigned short ColLayer = 0;
        fread(&ColLayer, sizeof(unsigned short), 1, pFile);
        CPhysics2DMgr::GetInst()->SetCollisionLayer(i, ColLayer);
    }

    fclose(pFile);

    return pLevel;
}

void CLevelSaveLoad::LoadLayer(CLayer* _Layer, FILE* _File)
{
    // Layer �� �̸� �ε�
    wstring strLayerName;
    LoadWStringFromFile(strLayerName, _File);
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
    LoadWStringFromFile(strName, _File);
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
        case COMPONENT_TYPE::ANIMATOR2D:
            pComponent = new CAnimator2D;
            break;
        case COMPONENT_TYPE::ANIMATOR3D:
            break;
        case COMPONENT_TYPE::LIGHT2D:
            pComponent = new CLight2D;
            break;
        case COMPONENT_TYPE::LIGHT3D:
            pComponent = new CLight3D;
            break;
        case COMPONENT_TYPE::CAMERA:
            pComponent = new CCamera;
            break;
        case COMPONENT_TYPE::RIGIDBODY2D:
            pComponent = new CRigidbody2D;
            break;
        case COMPONENT_TYPE::BOXCOLLIDER2D:
            pComponent = new CBoxCollider2D;
            break;
        case COMPONENT_TYPE::CIRCLECOLLIDER2D:
            pComponent = new CCircleCollider2D;
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
        case COMPONENT_TYPE::DECAL:
            break;
        case COMPONENT_TYPE::LANDSCAPE:
            break;
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
        LoadWStringFromFile(strScriptName, _File);

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
