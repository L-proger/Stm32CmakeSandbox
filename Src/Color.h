#pragma once

#include <cstdint>

struct Color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;

	static Color red(std::uint8_t value = 0xff) {
		return {value, 0, 0};
	}

	static Color green(std::uint8_t value = 0xff) {
		return {0, value, 0};
	}

	static Color blue(std::uint8_t value = 0xff) {
		return {0, 0, value};
	}

	Color operator *( float k) const{
		return {
			(std::uint8_t)(r * k),
			(std::uint8_t)(g * k),
			(std::uint8_t)(b * k)
		};
	}

	static Color lerp(const Color& a, const Color& b, float k){
		return {
			static_cast<std::uint8_t>(a.r * (1.0f - k) + b.r * k),
			static_cast<std::uint8_t>(a.g * (1.0f - k) + b.g * k),
			static_cast<std::uint8_t>(a.b * (1.0f - k) + b.b * k)
		};
	} 
};