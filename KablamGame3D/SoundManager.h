#pragma once

#include <string>
#include <map>
#include <irrKlang.h>

//#include "KablamEngine.h"
class KablamEngine; // forward dec

//#pragma comment(lib, "irrKlang.lib")

class SoundManager {
private:
    // kablamEngine
    KablamEngine* graphicsEngine;
    // Sound engine!
    irrklang::ISoundEngine* soundEngine;

    // Map to store sound file paths associated with their names
    std::map<std::wstring, std::wstring> soundMap;

    // Map to store currently playing sounds
    std::map<std::wstring, irrklang::ISound*> playingSounds;

    // Flag to indicate if the sound engine is operational
    bool isOperational;

    std::wstring inGameMusicName;
    std::wstring inGameMusicFilePath;
    bool inGameMusicPlaying;
    bool inGameMusicAdded;
    int inGameMusicVolume;
    int sfxVolume;

public:
    SoundManager(KablamEngine* graphicsEngine);
    ~SoundManager();

    // Add in-game music to the manager
    void AddInGameMusic(const std::wstring& name, const std::wstring& fileExtension);

    void PlayInGameMusic();

    void StopInGameMusic();

    // Add a sound file to the manager
    void AddSound(const std::wstring& name, const std::wstring& filePath);

    // Play a sound by its name
    int PlaySoundByName(const std::wstring& name, bool loop = false, float volume = 1.0f);

    // Stop a specific sound
    void StopSoundByName(const std::wstring& name);

    // Stop all sounds
    void StopAllSounds();

    // Check if the sound engine is operational
    bool IsOperational() const;


private:
    // helper function - irrKlang needs const char* rather than const wchar_t*
    std::string wstringToString(const std::wstring& wstr);

};



