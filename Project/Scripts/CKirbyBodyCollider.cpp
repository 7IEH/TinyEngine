#include "pch.h"
#include "CKirbyBodyCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyVacuumCollider.h"
#include "CKirbyUnitScript.h"
#include "CState.h"


#include "CCameraController.h"
#include <Engine/CTimeMgr.h>
#include "CState.h"

CKirbyBodyCollider::CKirbyBodyCollider()
    : CScript(KIRBYBODYCOLLIDER)
{
}

CKirbyBodyCollider::~CKirbyBodyCollider()
{
}

void CKirbyBodyCollider::begin()
{
    // Level�� ���۵Ǹ� BodyCollider�� ũ�⸦ �����Ѵ�.

    Transform()->SetAbsolute(false);
    Transform()->SetLocalScale(Vec3(1.2f, 1.2f, 1.2f));
    CapsuleCollider()->SetTrigger(true);
    CapsuleCollider()->SetCenter(Vec3(0.f, 0.65f, 0.f));
    CapsuleCollider()->SetHeight(1.51f);
    CapsuleCollider()->SetRadius(0.51f);
}

void CKirbyBodyCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    int LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    // monster : ������ ����
    if (LayerIdx == LAYER_MONSTER)
    {
        // Player�� Dodge ���̶�� ���Ϳ��� �������� ���� �ʴ´�.
        if (PLAYERFSM->GetCurState()->GetName() == L"DODGE_START" || PLAYERFSM->GetCurState()->GetName() == L"DODGE1" ||
            PLAYERFSM->GetCurState()->GetName() == L"DODGE2" || PLAYERFSM->GetCurState()->GetName() == L"BACKJUMP")
            return;

        // ���� ȿ���� �ִ� ������Ʈ
        if (PLAYERFSM->GetCurState()->GetName() == L"BURNING" || PLAYERFSM->GetCurState()->GetName() == L"BURNING_START")
        {
            CTimeMgr::GetInst()->SetTimeScale(0.1f, 0.f);
        }

        Vec3 HitDir = (_OtherCollider->Transform()->GetWorldPos() - Transform()->GetWorldPos()).Normalize();
        float HitDamage = FindDamage();
        UnitHit HitInfo = {DAMAGE_TYPE::NORMAL, HitDir, HitDamage, 0.f, 0.f};
        CUnitScript* pMonster = _OtherCollider->GetOwner()->GetScript<CUnitScript>();

        if (!pMonster)
            return;
        pMonster->GetDamage(HitInfo);
    }
}
float CKirbyBodyCollider::FindDamage()
{
    float Damage = 5.f;
    wstring StateName = PLAYERFSM->GetCurState()->GetName();

    if (PLAYERFSM->GetCurObject())
    {
        switch (PLAYERFSM->GetCurObjectIdx())
        {
        case ObjectCopyType::CONE: {
            if (StateName == L"ATTACK" || StateName == L"ATTACK_END" || StateName == L"ATTACK_COMBO1")
            {
                Damage = 100.f;
            }
        }
        break;
        }
    }

    return Damage;
}

UINT CKirbyBodyCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CKirbyBodyCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}
