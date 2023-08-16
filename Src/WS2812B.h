#pragma once
#include <type_traits>
#include <cstdint>

namespace LFramework::Driver {

class WS2812B {
public:
    static constexpr int RetCommandLength = 160;
    static constexpr int LedChannelBits = 8;
    static constexpr int LedColorBits = LedChannelBits * 3;

    static constexpr int T0H_ns = 350;
    static constexpr int T1H_ns = 900;
    static constexpr int T0L_ns = 900;
    static constexpr int T1L_ns = 350;

    static constexpr int T0_ns = T0H_ns + T0L_ns;
    static constexpr int T1_ns = T1H_ns + T1L_ns;



    template<typename DmaWordType_, std::uint32_t PwmPeriodLength_, std::uint32_t LedCount_ >
    class LedStripDmaBuffer {
    public:
        using DmaWordType = DmaWordType_;
        static constexpr std::uint32_t PwmPeriodLength = PwmPeriodLength_;
        static constexpr std::uint32_t LedCount = LedCount_;


        static_assert(std::is_same_v<DmaWordType, std::uint8_t> || std::is_same_v<DmaWordType, std::uint16_t> || std::is_same_v<DmaWordType, std::uint32_t>, "Unexpected DmaWordType");
        static_assert(!std::is_signed_v<DmaWordType>, "Only unsigned DmaWordType is allowed");
        static constexpr int DmaBufferSize = RetCommandLength + LedColorBits * LedCount;

        static constexpr DmaWordType T0_pwm = static_cast<DmaWordType>(static_cast<std::uint32_t>(PwmPeriodLength) * T0H_ns / T0_ns);
        static constexpr DmaWordType T1_pwm = static_cast<DmaWordType>(static_cast<std::uint32_t>(PwmPeriodLength) * T1H_ns / T0_ns);

        LedStripDmaBuffer() {
            for(int i = 0; i < RetCommandLength; ++i){
                dmaBuffer[i] = 0;
            }
        }


        void setLedColor(int pixelId, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
            int pixelOffset = RetCommandLength + pixelId * LedColorBits;

            for(int i = 0; i < 8; ++i){
                dmaBuffer[pixelOffset + i] = (g & 0x80) == 0 ? T0_pwm : T1_pwm;
                g <<= 1;
            }

            for(int i = 0; i < 8; ++i){
                dmaBuffer[pixelOffset + 8 + i] = (r & 0x80) == 0 ? T0_pwm : T1_pwm;
                r <<= 1;
            }

            for(int i = 0; i < 8; ++i){
                dmaBuffer[pixelOffset + 16 + i] = (b & 0x80) == 0 ? T0_pwm : T1_pwm;
                b <<= 1;
            }
        }

        uint32_t dmaBuffer[DmaBufferSize];
    };
};

}