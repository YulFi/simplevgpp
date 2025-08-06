#pragma once
#include <glm/glm.hpp>

class IRenderBackend {
public:
	virtual ~IRenderBackend() {}

	// --- Paint API ---
	// Set a solid fill color (rgba)
	virtual void setFillColor(const glm::vec4& color) = 0;

	// Set a linear gradient fill (p0, p1, color0, color1)
	virtual void setLinearGradient(const glm::vec2& p0, const glm::vec2& p1,
		const glm::vec4& color0, const glm::vec4& color1) = 0;

	virtual void setOutlineColor(const glm::vec4& color) = 0;
	virtual void setOutlineWidth(float width) = 0;

	// --- Drawing primitives ---
	// All antialiased by 'aa' (default 1.5 is good for screen)
	virtual void drawCircle(float cx, float cy, float radius, float aa) = 0;
	virtual void drawRect(float x, float y, float w, float h, float aa) = 0;
	virtual void drawRoundedRect(float x, float y, float w, float h, float radius, float aa) = 0;
	virtual void drawTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float aa) = 0;
	virtual void drawLine(const glm::vec2& a, const glm::vec2& b, float width, float aa) = 0;

	// --- Frame control (for double-buffered APIs like OpenGL) ---
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;
};
