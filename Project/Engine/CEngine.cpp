#include "pch.h"
#include "CEngine.h"
#include "CDevice.h"

#include "CPathMgr.h"
#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "CLevelMgr.h"
#include "CEditorMgr.h"
#include "CTaskMgr.h"

CEngine::CEngine()
    : m_hMainWnd(nullptr)
    , m_vResolution()
    , m_ClearColor{}
{
    m_ClearColor = Vec4(0.3f, 0.8f, 0.3f, 1.f);
}

CEngine::~CEngine()
{
}

int CEngine::init(HWND _hWnd, Vec2 _vResolution)
{
    m_hMainWnd = _hWnd;
    m_vResolution = _vResolution;

    RECT rt = {0, 0, (int)m_vResolution.x, (int)m_vResolution.y};
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, SetMenu(m_hMainWnd, nullptr)); // �޴��� Off
    SetWindowPos(m_hMainWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);

    if (FAILED(CDevice::GetInst()->init(m_hMainWnd, m_vResolution)))
    {
        MessageBox(nullptr, L"Device �ʱ�ȭ ����", L"�ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // Manager �ʱ�ȭ
    CPathMgr::init();
    CTimeMgr::GetInst()->init();
    CKeyMgr::GetInst()->init();
    CAssetMgr::GetInst()->init();
    CLevelMgr::GetInst()->init();
    CEditorMgr::GetInst()->init();

    return S_OK;
}

void CEngine::progress()
{
    // ===========================
    // tick
    // ===========================

    // Manager Update
    CTimeMgr::GetInst()->tick();
    CKeyMgr::GetInst()->tick();

    // Level Update
    CEditorMgr::GetInst()->tick(); // Editor tick ���� ȣ���ؾ� Level���� Imguiȣ�Ⱑ��
    CLevelMgr::GetInst()->tick();

    // ===========================
    // Rendering
    // ===========================

    // Clear
    CDevice::GetInst()->ClearRenderTarget(m_ClearColor);

    // Level Render
    CLevelMgr::GetInst()->render();
    CEditorMgr::GetInst()->render(); // Level Render ���� ȣ��

    // Present
    CDevice::GetInst()->Present();

    // task Execute
    CTaskMgr::GetInst()->tick();
}
