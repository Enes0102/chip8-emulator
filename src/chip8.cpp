#include "chip8.h"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <random>
#include <vector>

namespace {

constexpr std::array<uint8_t, 80> kFontset = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

constexpr uint16_t kFontStart = 0x050;
constexpr uint16_t kRomStart = 0x200;

std::mt19937 rng{std::random_device{}()};

}

Chip8::Chip8() {
    for (size_t i = 0; i < kFontset.size(); ++i) {
        memory_[kFontStart + i] = kFontset[i];
    }
}

bool Chip8::loadRom(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::fprintf(stderr, "ROM acilamadi: %s\n", path.c_str());
        return false;
    }

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    if (data.size() > memory_.size() - kRomStart) {
        std::fprintf(stderr, "ROM cok buyuk: %zu byte\n", data.size());
        return false;
    }

    for (size_t i = 0; i < data.size(); ++i) {
        memory_[kRomStart + i] = data[i];
    }
    pc_ = kRomStart;
    return true;
}

void Chip8::cycle() {
    uint16_t opcode = static_cast<uint16_t>(
        (memory_[pc_ & 0x0FFF] << 8) | memory_[(pc_ + 1) & 0x0FFF]);
    pc_ += 2;

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000) {

    case 0x0000:
        if (opcode == 0x00E0) {
            display_.fill(0);
            drawFlag = true;
        } else if (opcode == 0x00EE) {
            --sp_;
            pc_ = stack_[sp_ & 0x0F];
        }
        break;

    case 0x1000:
        pc_ = nnn;
        break;

    case 0x2000:
        stack_[sp_ & 0x0F] = pc_;
        ++sp_;
        pc_ = nnn;
        break;

    case 0x3000:
        if (V_[x] == nn) pc_ += 2;
        break;

    case 0x4000:
        if (V_[x] != nn) pc_ += 2;
        break;

    case 0x5000:
        if (V_[x] == V_[y]) pc_ += 2;
        break;

    case 0x9000:
        if (V_[x] != V_[y]) pc_ += 2;
        break;

    case 0xB000:
        pc_ = nnn + V_[0];
        break;

    case 0x6000:
        V_[x] = nn;
        break;

    case 0x7000:
        V_[x] = static_cast<uint8_t>(V_[x] + nn);
        break;

    case 0xA000:
        I_ = nnn;
        break;

    case 0xC000:
        V_[x] = static_cast<uint8_t>(rng() & nn);
        break;

    case 0x8000:
        switch (n) {
        case 0x0:
            V_[x] = V_[y];
            break;
        case 0x1:
            V_[x] |= V_[y];
            V_[0xF] = 0;
            break;
        case 0x2:
            V_[x] &= V_[y];
            V_[0xF] = 0;
            break;
        case 0x3:
            V_[x] ^= V_[y];
            V_[0xF] = 0;
            break;
        case 0x4: {
            int sum = V_[x] + V_[y];
            V_[x] = static_cast<uint8_t>(sum);
            V_[0xF] = (sum > 255) ? 1 : 0;
            break;
        }
        case 0x5: {
            uint8_t flag = (V_[x] >= V_[y]) ? 1 : 0;
            V_[x] = static_cast<uint8_t>(V_[x] - V_[y]);
            V_[0xF] = flag;
            break;
        }
        case 0x6: {
            uint8_t bit = V_[y] & 0x1;
            V_[x] = V_[y] >> 1;
            V_[0xF] = bit;
            break;
        }
        case 0x7: {
            uint8_t flag = (V_[y] >= V_[x]) ? 1 : 0;
            V_[x] = static_cast<uint8_t>(V_[y] - V_[x]);
            V_[0xF] = flag;
            break;
        }
        case 0xE: {
            uint8_t bit = (V_[y] >> 7) & 0x1;
            V_[x] = static_cast<uint8_t>(V_[y] << 1);
            V_[0xF] = bit;
            break;
        }
        default:
            std::fprintf(stderr, "Bilinmeyen opcode: %04X\n", opcode);
            break;
        }
        break;

    case 0xD000: {
        uint8_t startX = V_[x] % kWidth;
        uint8_t startY = V_[y] % kHeight;
        V_[0xF] = 0;

        for (int row = 0; row < n; ++row) {
            int py = startY + row;
            if (py >= kHeight) break;

            uint8_t spriteByte = memory_[(I_ + row) & 0x0FFF];
            for (int col = 0; col < 8; ++col) {
                int px = startX + col;
                if (px >= kWidth) break;

                if (spriteByte & (0x80 >> col)) {
                    uint8_t& pixel = display_[py * kWidth + px];
                    if (pixel) V_[0xF] = 1;
                    pixel ^= 1;
                }
            }
        }
        drawFlag = true;
        break;
    }

    case 0xE000:
        if (nn == 0x9E) {
            if (keys[V_[x] & 0x0F]) pc_ += 2;
        } else if (nn == 0xA1) {
            if (!keys[V_[x] & 0x0F]) pc_ += 2;
        } else {
            std::fprintf(stderr, "Bilinmeyen opcode: %04X\n", opcode);
        }
        break;

    case 0xF000:
        switch (nn) {
        case 0x07:
            V_[x] = delayTimer_;
            break;

        case 0x0A: {
            if (waitingKey_ < 0) {
                for (int k = 0; k < 16; ++k) {
                    if (keys[k]) { waitingKey_ = k; break; }
                }
                pc_ -= 2;
            } else if (keys[waitingKey_]) {
                pc_ -= 2;
            } else {
                V_[x] = static_cast<uint8_t>(waitingKey_);
                waitingKey_ = -1;
            }
            break;
        }

        case 0x15:
            delayTimer_ = V_[x];
            break;

        case 0x18:
            soundTimer_ = V_[x];
            break;

        case 0x1E:
            I_ = static_cast<uint16_t>(I_ + V_[x]);
            break;

        case 0x29:
            I_ = kFontStart + (V_[x] & 0x0F) * 5;
            break;

        case 0x33:
            memory_[I_ & 0x0FFF]       = V_[x] / 100;
            memory_[(I_ + 1) & 0x0FFF] = (V_[x] / 10) % 10;
            memory_[(I_ + 2) & 0x0FFF] = V_[x] % 10;
            break;

        case 0x55:
            for (int i = 0; i <= x; ++i) memory_[(I_ + i) & 0x0FFF] = V_[i];
            I_ = static_cast<uint16_t>(I_ + x + 1);
            break;

        case 0x65:
            for (int i = 0; i <= x; ++i) V_[i] = memory_[(I_ + i) & 0x0FFF];
            I_ = static_cast<uint16_t>(I_ + x + 1);
            break;

        default:
            std::fprintf(stderr, "Bilinmeyen opcode: %04X\n", opcode);
            break;
        }
        break;

    default:
        std::fprintf(stderr, "Bilinmeyen opcode: %04X\n", opcode);
        break;
    }
}

void Chip8::tickTimers() {
    if (delayTimer_ > 0) --delayTimer_;
    if (soundTimer_ > 0) --soundTimer_;
}
