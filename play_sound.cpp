#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load a sound effect
    Mix_Chunk* sound = Mix_LoadWAV("./music/eat.wav");
    if (!sound) {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Play the sound effect
    Mix_PlayChannel(1, sound, 0);

    // Wait for a few seconds to let the sound play
    SDL_Delay(3000000002);

    // Clean up and quit
    Mix_FreeChunk(sound);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
