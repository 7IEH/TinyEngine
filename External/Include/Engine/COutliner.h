#pragma once
#include "CEntity.h"

class CGameObject;

class COutliner : public CEntity
{
private:
    CGameObject* m_SelectedObj;

private:
    void DrawNode(CGameObject* obj, UINT LayerNum);
    void DrawDetails(CGameObject* obj);

public:
    void begin();
    void render();

private:
    COutliner();
    ~COutliner();

    friend class CLevelEditor; // CLevelEditor ������ ���������ϵ��� ����
};
