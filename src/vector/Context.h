#pragma once

#include "renderer/IRenderBackend.h"
#include "geometry/Circle.h"
#include "geometry/Rect.h"
#include "geometry/RoundedRect.h"
#include "geometry/Triangle.h"
#include "geometry/Line.h"
#include <glm/glm.hpp>

class Context {
public:
	explicit Context(IRenderBackend& backend) : m_backend(backend) {}

	// --- Paint API ---
	void setFillColor(const glm::vec4& color) {
		m_backend.setFillColor(color);
	}
	void setLinearGradient(const glm::vec2& p0, const glm::vec2& p1,
		const glm::vec4& color0, const glm::vec4& color1) {
		m_backend.setLinearGradient(p0, p1, color0, color1);
	}

	void setOutlineColor(const glm::vec4& color) { m_backend.setOutlineColor(color); }
	void setOutlineWidth(float width) { m_backend.setOutlineWidth(width); }

	// --- Shape Drawing ---
	void fillCircle(const Circle& c, float aa = 1.5f) {
		m_backend.drawCircle(c.cx, c.cy, c.radius, aa);
	}
	void fillRect(const Rect& r, float aa = 1.5f) {
		m_backend.drawRect(r.x, r.y, r.width, r.height, aa);
	}
	void fillRoundedRect(const RoundedRect& r, float aa = 1.5f) {
		m_backend.drawRoundedRect(r.x, r.y, r.width, r.height, r.radius, aa);
	}
	void fillTriangle(const Triangle& t, float aa = 1.5f) {
		m_backend.drawTriangle(t.a, t.b, t.c, aa);
	}
	void drawLine(const Line& l, float width, float aa = 1.5f) {
		m_backend.drawLine(l.a, l.b, width, aa);
	}

	// --- Frame Control ---
	void beginFrame() { m_backend.beginFrame(); }
	void endFrame() { m_backend.endFrame(); }

private:
	IRenderBackend& m_backend;
};
