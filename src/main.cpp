#include <SDL.h>

#include <cstdio>

#include "chip8.h"

namespace {

constexpr int kScale = 12;
constexpr int kCyclesPerFrame = 11;
constexpr uint32_t kOnColor = 0xFF33FF66;
constexpr uint32_t kOffColor = 0xFF0A0F0A;

int mapKey(SDL_Scancode sc) {
    switch (sc) {
    case SDL_SCANCODE_1: return 0x1;
    case SDL_SCANCODE_2: return 0x2;
    case SDL_SCANCODE_3: return 0x3;
    case SDL_SCANCODE_4: return 0xC;
    case SDL_SCANCODE_Q: return 0x4;
    case SDL_SCANCODE_W: return 0x5;
    case SDL_SCANCODE_E: return 0x6;
    case SDL_SCANCODE_R: return 0xD;
    case SDL_SCANCODE_A: return 0x7;
    case SDL_SCANCODE_S: return 0x8;
    case SDL_SCANCODE_D: return 0x9;
    case SDL_SCANCODE_F: return 0xE;
    case SDL_SCANCODE_Z: return 0xA;
    case SDL_SCANCODE_X: return 0x0;
    case SDL_SCANCODE_C: return 0xB;
    case SDL_SCANCODE_V: return 0xF;
    default: return -1;
    }
}

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::fprintf(stderr, "Kullanim: %s <rom.ch8>\n", argv[0]);
        return 1;
    }

    Chip8 chip8;
    if (!chip8.loadRom(argv[1])) return 1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL baslatilamadi: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        Chip8::kWidth * kScale, Chip8::kHeight * kScale, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        Chip8::kWidth, Chip8::kHeight);

    if (!window || !renderer || !texture) {
        std::fprintf(stderr, "Pencere olusturulamadi: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    uint32_t pixels[Chip8::kWidth * Chip8::kHeight];
    bool running = true;

    while (running) {
        uint32_t frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                    continue;
                }
                int k = mapKey(event.key.keysym.scancode);
                if (k >= 0) chip8.keys[k] = (event.type == SDL_KEYDOWN) ? 1 : 0;
            }
        }

        for (int i = 0; i < kCyclesPerFrame; ++i) {
            chip8.cycle();
        }
        chip8.tickTimers();

        if (chip8.drawFlag) {
            const auto& disp = chip8.display();
            for (size_t i = 0; i < disp.size(); ++i) {
                pixels[i] = disp[i] ? kOnColor : kOffColor;
            }
            SDL_UpdateTexture(texture, nullptr, pixels, Chip8::kWidth * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            chip8.drawFlag = false;
        }

        uint32_t elapsed = SDL_GetTicks() - frameStart;
        if (elapsed < 16) SDL_Delay(16 - elapsed);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
