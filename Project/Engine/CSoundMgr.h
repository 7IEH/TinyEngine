#pragma once

struct tSoundEvent
{
    Ptr<CSound> pSound;
    float StartVolume;
    float EndVolume;
    float Acc;               // ���� �ð�
    float Duration;          // StartVolume ~ EndVolume ������ ���� �ð�
    bool bLoopAfterDuration; // ���� �ð��� ���� �� Stop or Loop ����
    bool bOverlap;

    // 3DSound Param
    bool b3DSound;
    Vec3 WorldPos;
    float Mindistance;
    float Maxdistance;
};

class CSoundMgr : public CSingleton<CSoundMgr>
{
    SINGLE(CSoundMgr);

private:
    std::list<tSoundEvent> m_ListSoundEvent;

public:
    void tick();
    void FadeSound(const wstring& _SoundPath, float _StartVolume, float _EndVolume, float _Duration, bool _bLoopAfterDuration, bool _bOverlap = true,
                   bool _b3DSound = false, Vec3 _WorldPos = Vec3(), float _Mindistance = 1.f, float _Maxdistance = 500.f);
    void ClearSoundEvent();
};
