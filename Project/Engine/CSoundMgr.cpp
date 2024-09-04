#include "pch.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"

CSoundMgr::CSoundMgr()
    : m_ListSoundEvent{}
{
}

CSoundMgr::~CSoundMgr()
{
}

void CSoundMgr::tick()
{
    std::list<tSoundEvent>::iterator iter = m_ListSoundEvent.begin();

    for (; iter != m_ListSoundEvent.end();)
    {
        tSoundEvent& Event = *iter;
        Event.Acc += DT_ENGINE;

        if (Event.Acc >= Event.Duration)
        {
            // ��� �ð��� ���� ���� Stop
            if (!Event.bLoopAfterDuration)
            {
                Event.pSound->Stop();
            }

            iter = m_ListSoundEvent.erase(iter);
            continue;
        }

        float fVolume = Lerp(Event.StartVolume, Event.EndVolume, Event.Acc / Event.Duration);

        // �ʱ� ���� ���
        if (!Event.pSound->IsPlaying())
        {
            if (Event.b3DSound)
            {
                Event.pSound->Play3D(Event.WorldPos, 0, fVolume, Event.bOverlap, Event.Mindistance, Event.Maxdistance);
            }
            else
            {
                Event.pSound->Play2D(0, fVolume, Event.bOverlap);
            }
        }
        // ������� ����� Volume �� ����
        else
        {
            Event.pSound->SetVolume(fVolume);
        }

        ++iter;
    }
}

void CSoundMgr::FadeSound(const wstring& _SoundPath, float _StartVolume, float _EndVolume, float _Duration, bool _bLoopAfterDuration, bool _bOverlap,
                          bool _b3DSound, Vec3 _WorldPos, float _Mindistance, float _Maxdistance)
{
    Ptr<CSound> pSound = CAssetMgr::GetInst()->Load<CSound>(_SoundPath, _SoundPath);

    // �������� �ʴ� ����
    if (nullptr == pSound)
        return;

    std::list<tSoundEvent>::iterator iter = m_ListSoundEvent.begin();

    for (; iter != m_ListSoundEvent.end(); iter++)
    {
        // SoundMgr�� �̹� ��ϵ� ������ ���
        if (iter->pSound == pSound)
        {
            return;
        }
    }

    tSoundEvent SoundEvent;
    SoundEvent.pSound = pSound;
    SoundEvent.StartVolume = _StartVolume;
    SoundEvent.EndVolume = _EndVolume;
    SoundEvent.Acc = 0.f;
    SoundEvent.Duration = _Duration;
    SoundEvent.bLoopAfterDuration = _bLoopAfterDuration;
    SoundEvent.bOverlap = _bOverlap;
    SoundEvent.b3DSound = _b3DSound;
    SoundEvent.WorldPos = _WorldPos;
    SoundEvent.Mindistance = _Mindistance;
    SoundEvent.Maxdistance = _Maxdistance;

    m_ListSoundEvent.push_back(SoundEvent);
}
