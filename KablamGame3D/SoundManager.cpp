#include "SoundManager.h"
#include "KablamEngine.h"
#include <locale>
#include <codecvt>
#include <iostream>

SoundManager::SoundManager(KablamEngine* gEngine)
    : graphicsEngine(gEngine), soundEngine(nullptr), isOperational(false), inGameMusicPlaying(false), inGameMusicAdded(false), inGameMusicVolume(50), sfxVolume(100) {
    try {
        soundEngine = irrklang::createIrrKlangDevice();
        if (!soundEngine) {
            graphicsEngine->AddToLog(L"Unable to start up sound engine.");
        }
        else {
            isOperational = true;
            graphicsEngine->AddToLog(L"Sound engine created.");

        }
    }
    catch (const std::exception& e) {
        graphicsEngine->AddToLog(L"Exception caught while initializing sound engine: " + std::wstring(e.what(), e.what() + strlen(e.what())));
    }
}

SoundManager::~SoundManager() {
    if (soundEngine) {
        soundEngine->drop();
    }
}

void SoundManager::AddInGameMusic(const std::wstring& name, const std::wstring& filePath) {
    if (!isOperational) return;
    inGameMusicName = name;
    inGameMusicFilePath = filePath;
    soundMap[name] = filePath;
    inGameMusicAdded = true;
}

void SoundManager::PlayInGameMusic() {
    if (!isOperational) return;
    if (!inGameMusicAdded) return; // not added 
    if (inGameMusicPlaying) return; // Music is already playing

    // Check if the in-game music is loaded
    auto it = soundMap.find(inGameMusicName);
    if (it == soundMap.end()) return; // In-game music not loaded

    std::string filePathStr = wstringToString(it->second);

    // Play the in-game music in a loop
    irrklang::ISound* music = soundEngine->play2D(filePathStr.c_str(), true, false, true);
    if (music) {
        music->setVolume(inGameMusicVolume / 100.0f);
        playingSounds[inGameMusicName] = music;
        inGameMusicPlaying = true;
        graphicsEngine->AddToLog(L"In-game music started: " + it->second);
    }
    else {
        graphicsEngine->AddToLog(L"Error: Unable to play in-game music: " + it->second);
    }
}

void SoundManager::StopInGameMusic() {
    if (!isOperational) return;
    if (!inGameMusicAdded) return; // not added
    if (!inGameMusicPlaying) return; // Music is not playing

    auto it = playingSounds.find(inGameMusicName);
    if (it != playingSounds.end() && it->second) {
        it->second->stop();
        it->second->drop();
        playingSounds.erase(it);
        inGameMusicPlaying = false;
        graphicsEngine->AddToLog(L"In-game music stopped.");
    }
}

void SoundManager::AddSound(const std::wstring& name, const std::wstring& filePath) {
    if (!isOperational) return;
    soundMap[name] = filePath;
}

int SoundManager::PlaySoundByName(const std::wstring& name, bool loop, float volume) {
    if (!isOperational) return -1; // Sound engine not operational
    auto it = soundMap.find(name);
    if (it != soundMap.end()) {
        // // Ensure the sound is removed from the map after stopping
        //if (playingSounds[name]) {
        //    playingSounds[name]->stop();
        //    playingSounds[name]->drop();
        //   // playingSounds.erase(name);
        //}
        std::string filePathStr = wstringToString(it->second);

        irrklang::ISound* sound = soundEngine->play2D(filePathStr.c_str(), loop, false, true);
        if (sound) {
            sound->setVolume((sfxVolume / 100.0f) * (volume ));
            playingSounds[name] = sound;
            return 0; // Success
        }
        return 2; // Error starting sound

    }
    return 1; // Sound not found
}

void SoundManager::StopSoundByName(const std::wstring& name) {
    if (!isOperational) return;
    auto it = playingSounds.find(name);
    if (it != playingSounds.end() && it->second) {
        it->second->stop();
        it->second->drop();
        playingSounds.erase(it);
    }
}

void SoundManager::StopAllSounds() {
    if (!isOperational) return;
    soundEngine->stopAllSounds();
    for (auto& pair : playingSounds) {
        if (pair.second) {
            pair.second->drop();
        }
    }
    playingSounds.clear();
}

bool SoundManager::IsOperational() const {
    return isOperational;
}

// Helper function to convert wstring to string
std::string SoundManager::wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
