#pragma once
#pragma once
#include "CEntity.h"

class CGameObject;

class COutliner : public CEntity
{
private:
    void DrawNode(CGameObject* obj);
    void DrawDetails(CGameObject* obj);

public:
    void init();
    void render();

private:
    COutliner();
    ~COutliner();

    friend class CLevelEditor; // CLevelEditor ������ ���������ϵ��� ����
};
