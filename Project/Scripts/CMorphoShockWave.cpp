#include "pch.h"
#include "CMorphoShockWave.h"
#include "CPlayerMgr.h"

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

    //@CAMERA ȸ�� (30s)
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