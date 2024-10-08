#pragma once
#include "CState.h"

class CKirbyStuffed : public CState
{
public:
    virtual void tick();
    virtual void Enter();
    virtual void Exit();

public:
    CLONE(CKirbyStuffed)
    CKirbyStuffed();
    virtual ~CKirbyStuffed();
};
