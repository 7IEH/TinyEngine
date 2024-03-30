#include "pch.h"
#include "CGameManagerScript.h"

#include <Engine\\CLevelMgr.h>
#include <Engine\\CLevel.h>

CGameManagerScript* CGameManagerScript::m_Inst = nullptr;

CGameManagerScript::CGameManagerScript()
    : CScript(GAMEMANAGERSCRIPT)
    , m_Player(nullptr)
{
    CGameObject* pPlayer = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");

    // ������ �÷��̾ ���������ʴ� ��� �����տ��� �ε�
    if (nullptr == pPlayer)
    {
        Ptr<CPrefab> pPlayerPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Player.pref", L"prefab\\Player.pref");
        m_Player = pPlayerPref->Instantiate();
    }
    else
    {
        m_Player = pPlayer;
    }
}

CGameManagerScript::~CGameManagerScript()
{
    m_Inst = nullptr;

    CGameObject* pObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Player");
    if (nullptr == pObj)
    {
        if (nullptr != m_Player)
        {
            delete m_Player;
            m_Player = nullptr;
        }
    }
}

void CGameManagerScript::begin()
{
    vector<CGameObject*> pGMObjs = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectsOfType<CGameManagerScript>();
    if (1 != pGMObjs.size())
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
        return;
    }
}

void CGameManagerScript::tick()
{
}

CGameManagerScript* CGameManagerScript::GetInset()
{
    if (nullptr == m_Inst)
    {
        CGameObject* pGMObj = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectOfType<CGameManagerScript>();

        if (nullptr != pGMObj)
        {
            CGameManagerScript* ScriptInst = pGMObj->GetScript<CGameManagerScript>();

            // GM������Ʈ�� �����ϴµ� ��ũ��Ʈ�� ���������ʴ� ���
            if (nullptr == ScriptInst)
            {
                ScriptInst = new CGameManagerScript;
            }

            m_Inst = ScriptInst;
        }
        else // GM������Ʈ�� ���������ʴ� ���
        {
            CGameObject* pNewGMObj = new CGameObject;
            pNewGMObj->SetName(L"GameManager");
            pNewGMObj->AddComponent(new CTransform);

            CGameManagerScript* ScriptInst = new CGameManagerScript;
            pNewGMObj->AddComponent(ScriptInst);
            m_Inst = ScriptInst;

            int GMLayerIdx = 0;
            CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

            for (int i = 0; i < LAYER_MAX; i++)
            {
                if (L"GameManager" == pCurLevel->GetLayer(i)->GetName())
                {
                    GMLayerIdx = i;
                    break;
                }
            }

            GamePlayStatic::SpawnGameObject(pNewGMObj, GMLayerIdx);
        }
    }

    return m_Inst;
}
