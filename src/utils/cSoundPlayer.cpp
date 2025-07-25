#include "cSoundPlayer.h"

#include "definitions.h"
#include "utils/cLog.h"

#include "utils/DataPack.hpp"
#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL_mixer.h>
#include <algorithm> // for std::clamp


namespace {

constexpr int kAllegroMaxNrVoices = 256;
constexpr int kAllegroMaxVolume = 255;
constexpr int kAllegroPanCenter = 128;

constexpr int kFinishedPLaying = -1;
constexpr int kNoVoiceAvailable = -1;
constexpr int kNoVoice = -1;
constexpr int kNoLoop = 0;

constexpr int kMinNrVoices = 4;
constexpr int kMaxVolume = 220;
constexpr int MinVolume = 0;
constexpr int MaxVolume = MIX_MAX_VOLUME;
}

class cSoundData {
public:
    cSoundData()
    {
        auto logger = cLogger::getInstance();

        gfxaudio = std::make_unique<DataPack>("data/sdl_audio.dat");
        if (gfxaudio == nullptr) {
            static auto msg = "Could not hook/load datafile: sdl_audio.dat.";
            logger->log(LOG_ERROR, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
            throw std::runtime_error(msg);
        }
        else {
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", "Hooked datafile: sdl_audio.dat.", OUTC_SUCCESS);
        }
    }

    Mix_Chunk *getSample(int sampleId) const
    {
        return gfxaudio->getSample(sampleId);
    }

    Mix_Music *getMusic(int musicId) const
    {
        return gfxaudio->getMusic(musicId);
    }
private:
    std::unique_ptr<DataPack> gfxaudio;
};

cSoundPlayer::cSoundPlayer(const cPlatformLayerInit &init) : cSoundPlayer(init, kAllegroMaxNrVoices)
{
}

cSoundPlayer::cSoundPlayer(const cPlatformLayerInit &, int maxNrVoices)
    : soundData(std::make_unique<cSoundData>())
{
    // The platform layer init object is not used here, but since it needs to be passed, it tells
    // the caller that the initialization needs to be performed first.

    auto logger = cLogger::getInstance();

    if (maxNrVoices < kMinNrVoices) {
        logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Muting all sound.", OUTC_SUCCESS);
        return;
    }

    int nr_voices = maxNrVoices;
    while (true) {
        if (nr_voices < kMinNrVoices) {
            logger->log(LOG_WARN, COMP_SOUND, "Initialization", "Failed installing sound.", OUTC_FAILED);
            return;
        }

        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
            logger->log(LOG_ERROR, COMP_SOUND, "SDL2_mixer initialization", Mix_GetError(), OUTC_FAILED);
        }

        if (Mix_AllocateChannels(nr_voices) >= kMinNrVoices) {
            auto msg = fmt::format("Successfully installed sound. {} voices reserved", nr_voices);
            logger->log(LOG_INFO, COMP_SOUND, "Initialization", msg, OUTC_SUCCESS);

            voices.resize(nr_voices - 1, kNoVoice);
            break;
        }
        else {
            auto msg = fmt::format("Failed installing sound. {} voices reserved", nr_voices);
            logger->log(LOG_ERROR, COMP_SOUND, "Initialization", msg, OUTC_FAILED);
        }
    }
    musicVolume = MaxVolume/2;
    Mix_MasterVolume(MaxVolume/2);
    Mix_VolumeMusic(musicVolume);
}

cSoundPlayer::~cSoundPlayer()
{
}

int cSoundPlayer::getMaxVolume()
{
    // TODO: This will become configurable (so you can set your own max volume for sounds, etc)
    return MaxVolume;
}

void cSoundPlayer::think()
{}

void cSoundPlayer::playSound(int sampleId)
{
    playSound(sampleId, musicVolume);
}

void cSoundPlayer::playSound(int sampleId, int vol)
{
    if (vol <= 0) {
        return;
    }
   vol = std::clamp(vol, 0, kAllegroMaxVolume);

    Mix_Chunk *sample = soundData->getSample(sampleId);
    Mix_PlayChannel(-1, sample, 0); // -1 means play on any available channel
}

void cSoundPlayer::playVoice(int sampleId, int house)
{
    if (house == HARKONNEN) {
        sampleId += 1;
    }
    else if (house == ORDOS) {
        sampleId += 2;
    }

    playSound(sampleId);
}

void cSoundPlayer::playMusic(int sampleId)
{
    Mix_PlayMusic(soundData->getMusic(sampleId), kNoLoop);
}

bool cSoundPlayer::isMusicPlaying() const
{
    return Mix_PlayingMusic() > 0;
}

void cSoundPlayer::stopMusic()
{
    Mix_HaltMusic();
}

void cSoundPlayer::setMusicVolume(int vol)
{
    musicVolume = std::clamp(vol, 0, MaxVolume);
    Mix_VolumeMusic(musicVolume);
}

void cSoundPlayer::changeMusicVolume(int delta)
{
    musicVolume = std::clamp(musicVolume + delta, 0, MaxVolume);
    this ->setMusicVolume(musicVolume);
}
