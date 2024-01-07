#pragma once
#include "CEditor.h"
#include "COutliner.h"
#include "CContentBrowser.h"
#include "Ptr.h"
#include "CTexture.h"

class CLevelEditor : public CEditor
{
private:
    bool m_ViewportFocused;
    bool m_ViewportHovered;

    bool m_bShowIDMap;

    COutliner m_Outliner;

    ImGuizmo::OPERATION m_GizmoType;

    CContentBrowser m_ContentBrowser;

public:
    virtual void init() override;
    virtual void tick() override;
    virtual void finaltick() override;
    virtual void render() override;

private:
    void SetDarkThemeColors();

public:
    bool IsViewportFocused() const { return m_ViewportFocused; }
    bool IsViewportHovered() const { return m_ViewportHovered; }

private:
    // UI Panels
    void MenuBar();
    void UI_Toolbar();
    void ImGuizmoRender();
    void CollisionResponses();

public:
    CLevelEditor();
    virtual ~CLevelEditor();
};
