#pragma once

#include <array>
#include <cstdint>
#include <string>

class Chip8 {
public:
    static constexpr int kWidth = 64;
    static constexpr int kHeight = 32;

    Chip8();

    bool loadRom(const std::string& path);

    void cycle();

    void tickTimers();

    const std::array<uint8_t, kWidth * kHeight>& display() const { return display_; }
    bool soundOn() const { return soundTimer_ > 0; }

    bool drawFlag = false;
    std::array<uint8_t, 16> keys{};

private:
    std::array<uint8_t, 4096> memory_{};
    std::array<uint8_t, 16> V_{};
    uint16_t I_ = 0;
    uint16_t pc_ = 0x200;
    std::array<uint16_t, 16> stack_{};
    uint8_t sp_ = 0;
    uint8_t delayTimer_ = 0;
    uint8_t soundTimer_ = 0;
    int waitingKey_ = -1;
    std::array<uint8_t, kWidth * kHeight> display_{};
};
