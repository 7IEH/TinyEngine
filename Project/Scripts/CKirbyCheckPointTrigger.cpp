#include "pch.h"
#include "CKirbyCheckPointTrigger.h"

#include "CPlayerMgr.h"
#include "CKirbyMoveController.h"

CKirbyCheckPointTrigger::CKirbyCheckPointTrigger()
    : CScript(KIRBYCHECKPOINTTRIGGER)
{
    AddScriptParam(SCRIPT_PARAM::VEC3, &m_Offset, "Offset");
}

CKirbyCheckPointTrigger::CKirbyCheckPointTrigger(const CKirbyCheckPointTrigger& _Origin)
    : CScript(KIRBYCHECKPOINTTRIGGER)
{
    AddScriptParam(SCRIPT_PARAM::VEC3, &m_Offset, "Offset");
}

CKirbyCheckPointTrigger::~CKirbyCheckPointTrigger()
{
}


void CKirbyCheckPointTrigger::tick()
{
#ifndef DISTRIBUTE

    Vec3 Pos = Transform()->GetWorldPos();
    Pos += m_Offset;

    GamePlayStatic::DrawDebugLine(Pos, Vec3(0.f, -1.f, 0.f), Transform()->GetWorldScale().y * 2.f, Vec3(1.f, 0.f, 0.f), true);
#endif
}

void CKirbyCheckPointTrigger::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner()->GetLayerIdx() == LAYER_PLAYER)
    {
        Vec3 Pos = Transform()->GetWorldPos();
        Pos += m_Offset;

        // Collider�� �߾����� ���� �ٴ����� ���̸� ��� �� ������ ������ Pos�� �Ѵ�.
        static vector<wstring> vecCollision{L"World Static", L"World Dynamic"};
        RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Pos, Vec3(0.f, -1.f, 0.f), Transform()->GetWorldScale().y * 2.f, vecCollision);

        if (Hit.pCollisionObj == nullptr)
            return;

        Vec3 RespawnPoint = Hit.Point;
        RespawnPoint.y += 1.f;

        PLAYERCTRL->SetCheckPoint(RespawnPoint);

        // �� �̻� �������� �ʵ��� DeActive�Ѵ�.
        GetOwner()->SetActive(false);
    }
}

UINT CKirbyCheckPointTrigger::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CKirbyCheckPointTrigger::LoadFromLevelFile(FILE* _File)
{
    return 0;
}
