#include "pch.h"
#include "CMorphoShockWave.h"
#include "CPlayerMgr.h"
#include "CCameraController.h"

CMorphoShockWave::CMorphoShockWave()
    : CScript(MORPHOSHOCKWAVE)
{
}

CMorphoShockWave::~CMorphoShockWave()
{
}

void CMorphoShockWave::tick()
{
}

void CMorphoShockWave::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner() != PLAYER)
        return;

    // Camera : ȸ��
    CAMERACTRL->Shake(0.3f, 30.f, 30.f);
    CAMERACTRL->Tilt(10.f, 2.f);

    //@EFFECT Ŀ�� ����� ȿ��
}

UINT CMorphoShockWave::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CMorphoShockWave::LoadFromLevelFile(FILE* _File)
{
    return 0;
}