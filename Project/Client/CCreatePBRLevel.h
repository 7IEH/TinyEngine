#pragma once

class CLevel;

class CCreatePBRLevel
{
public:
    static CLevel* CreatePBRLevel();

private:
    static void AddSample(CLevel* _CurLevel);
    static void AddModels(CLevel* _CurLevel);
};
