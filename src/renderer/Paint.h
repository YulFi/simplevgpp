#pragma once
#include <glm/glm.hpp>
#include <optional>
#include <variant>

// Solid color
struct SolidColor {
	glm::vec4 color = { 1, 1, 1, 1 };
};

// Linear gradient (from p0 to p1, color0 to color1)
struct LinearGradient {
	glm::vec2 p0, p1;
	glm::vec4 color0 = { 1,1,1,1 };
	glm::vec4 color1 = { 1,1,1,1 };
};

using FillType = std::variant<SolidColor, LinearGradient>;

struct Paint {
	FillType fill = SolidColor{ {1,1,1,1} };    // Fill type (default solid white)
	glm::vec4 strokeColor = { 0, 0, 0, 0 };     // Transparent by default (no stroke)
	float strokeWidth = 0.0f;                 // No stroke by default
	float aa = 1.5f;                          // Antialiasing width (in pixels)

	// Add shadow, blur, or other effects here if desired

	// Helpers for convenience
	void setSolidColor(const glm::vec4& color) { fill = SolidColor{ color }; }
	void setLinearGradient(const glm::vec2& p0, const glm::vec2& p1,
		const glm::vec4& c0, const glm::vec4& c1) {
		fill = LinearGradient{ p0, p1, c0, c1 };
	}
};
