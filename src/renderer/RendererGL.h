#pragma once

#include "renderer/IRenderBackend.h"
#include "renderer/VertexArrayGL.h"
#include "renderer/BufferGL.h"
#include "renderer/ShaderManager.h"
#include <glm/glm.hpp>

class RendererGL : public IRenderBackend {
public:
	RendererGL(int width, int height, ShaderManager& shaderMgr);
	~RendererGL() override = default;

	// Paint API
	void setFillColor(const glm::vec4& color) override;
	void setLinearGradient(const glm::vec2& p0, const glm::vec2& p1,
		const glm::vec4& color0, const glm::vec4& color1) override;
	void setOutlineColor(const glm::vec4& color);
	void setOutlineWidth(float width);

	// Drawing primitives
	void drawCircle(float cx, float cy, float radius, float aa) override;
	void drawRect(float x, float y, float w, float h, float aa) override;
	void drawRoundedRect(float x, float y, float w, float h, float radius, float aa) override;
	void drawTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float aa) override;
	void drawLine(const glm::vec2& a, const glm::vec2& b, float width, float aa) override;

	// Frame control
	void beginFrame() override;
	void endFrame() override;

	int width() const { return m_width; }
	int height() const { return m_height; }

private:
	int m_width, m_height;

	VertexArrayGL m_vao;
	BufferGL      m_vbo;
	ShaderManager& m_shaderMgr;

	// Paint state
	glm::vec4 m_fillColor{ 1,1,1,1 };
	bool m_isGradient = false;
	glm::vec2 m_gradP0, m_gradP1;
	glm::vec4 m_gradColor0, m_gradColor1;
	glm::vec4 m_outlineColor{ 0,0,0,0 }; // default: no outline
	float m_outlineWidth = 0.0f;       // default: no outline
};
