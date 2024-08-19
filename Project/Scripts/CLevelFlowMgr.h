#pragma once

#include <Engine/CScript.h>

class CGameObject;
class CFadeEffectScript;
class CUIFlowScript;

class CLevelFlowMgr : public CScript
{
private:
    bool m_bIsChangedLevel;

    bool m_bFadeEffect;
    float m_FadeEffectAcc;
    float m_FadeEffectDuration;


    bool m_bRadialBlurEffect;
    float m_RadialBlurAcc;
    float m_RadialBlurDuration;

    wstring m_CurLevelPath;
    string m_NextLevelPath;

    CGameObject* m_DimensionFadeEffect;
    CGameObject* m_RadialBlurEffect;

    CFadeEffectScript* m_FadeEffectScript;

    CUIFlowScript* m_UIFlowScript;
    Ptr<CMaterial> m_ToneMappingMtrl;

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void TriggerEvent(UINT _Idx){};

public:
    CUIFlowScript* GetFlowScript() { return m_UIFlowScript; }
    void SetFlowScript(CUIFlowScript* _pScript) { m_UIFlowScript = _pScript; }

    void SetNextLevel(const string _string) { m_NextLevelPath = _string; }

public:
    void OnDimensionFade();
    void OffDimensionFade();
    void OnRadialBlurEffect(float _Duration, float _Radius = 1.f, float _BlurPower = 1.1f);
    void OffRadialBlurEffect();
    void SetFadeEffectColor(Vec3 _Color);
    void SetFadeEffect(Vec3 _Color, bool _bReverse, float _Duration, float _Speed, bool _CenterMode);
    void SetToneMappingParam(bool _bBloomEnable = false, bool _bBlendMode = false, float _BloomStrength = 0.5f, float _Threshold = 0.f,
                             float _FilterRadius = 1.f, float _Exposure = 1.f, float _Gamma = 2.2f);

public:
    virtual void LevelStart();
    virtual void LevelEnd();
    virtual void LevelRestart();

private:
    virtual void LevelExit();
    virtual void MtrlParamUpdate();

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    virtual CLevelFlowMgr* Clone() = 0;
    CLevelFlowMgr(UINT _Type);
    CLevelFlowMgr(const CLevelFlowMgr& _Origin);
    virtual ~CLevelFlowMgr();
};
