#include "pch.h"
#include "CEngine.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CTaskMgr.h"
#include "CDevice.h"
#include "CEditorMgr.h"
#include "CEditor.h"
#include "CLayer.h"
#include "CRenderMgr.h"

#include "CLevelSaveLoad.h"

#include "CTestLevel.h"
#include "CPBRLevel.h"

CLevelMgr::CLevelMgr()
    : m_CurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
    if (nullptr != m_CurLevel)
    {
        delete m_CurLevel;
        m_CurLevel = nullptr;
    }
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel)
{
    if (nullptr != m_CurLevel)
    {
        delete m_CurLevel;
        m_CurLevel = nullptr;
    }

    m_CurLevel = _NextLevel;
}

void CLevelMgr::init()
{
    // Level
    m_CurLevel = new CTestLevel;
    m_CurLevel->SetName(L"Test Level");
    CRenderMgr::GetInst()->SetHDRIRender(false);

    // PBR Level
    // m_CurLevel = new CPBRLevel;
    // m_CurLevel->SetName(L"PBR Level");
    // CRenderMgr::GetInst()->SetHDRIRender(true);

    m_CurLevel->begin();
}

void CLevelMgr::tick()
{
    if (nullptr == m_CurLevel)
        return;

    // ���� �����ӿ� ��ϵ� ������Ʈ�� clear
    m_CurLevel->clear();

    m_CurLevel->tick();
    m_CurLevel->finaltick();
}
