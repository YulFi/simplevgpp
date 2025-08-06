#include "renderer/RendererGL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <algorithm>

RendererGL::RendererGL(int width, int height, ShaderManager& shaderMgr)
	: m_width(width), m_height(height), m_shaderMgr(shaderMgr)
{
	m_vao = VertexArrayGL();
	m_vbo = BufferGL(GL_ARRAY_BUFFER);

	m_vao.bind();
	m_vbo.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	m_vbo.unbind();
	m_vao.unbind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_width, m_height);
}

// Paint API
void RendererGL::setFillColor(const glm::vec4& color) {
	m_fillColor = color;
	m_isGradient = false;
}
void RendererGL::setLinearGradient(const glm::vec2& p0, const glm::vec2& p1,
	const glm::vec4& color0, const glm::vec4& color1) {
	m_gradP0 = p0;
	m_gradP1 = p1;
	m_gradColor0 = color0;
	m_gradColor1 = color1;
	m_isGradient = true;
}

// Frame control
void RendererGL::beginFrame() {
	glViewport(0, 0, m_width, m_height);
	//glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
}
void RendererGL::endFrame() {}

// Draw circle
void RendererGL::drawCircle(float cx, float cy, float radius, float aa) {
	auto shader = m_shaderMgr.load("shaders/shape.vert", "shaders/circle.frag");
	if (!shader) return;
	shader->use();
	glm::mat4 proj = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f);

	shader->setUniform("u_proj", proj);
	shader->setUniform("u_center", glm::vec2(cx, cy));
	shader->setUniform("u_radius", radius);
	shader->setUniform("u_aa", aa);
	shader->setUniform("u_outlineColor", m_outlineColor);
	shader->setUniform("u_outlineWidth", m_outlineWidth);

	//shader->setUniform("u_outlineColor", glm::vec4(1.f,0.f,0.f,1.f));
	//shader->setUniform("u_outlineWidth", 2.f);

	if (m_isGradient) {
		shader->setUniform("u_color0", m_gradColor0);
		shader->setUniform("u_color1", m_gradColor1);
		shader->setUniform("u_gradP0", m_gradP0);
		shader->setUniform("u_gradP1", m_gradP1);
		shader->setUniform("u_fillType", 1);
	}
	else {
		shader->setUniform("u_color", m_fillColor);
		shader->setUniform("u_fillType", 0);
	}

	float margin = aa + 2.0f;
	float x = cx - radius - margin, y = cy - radius - margin;
	float w = 2 * radius + 2 * margin, h = 2 * radius + 2 * margin;
	std::vector<glm::vec2> verts = {
		{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}
	};

	m_vao.bind();
	m_vbo.bind();
	m_vbo.uploadData(verts.data(), verts.size() * sizeof(glm::vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	m_vao.unbind();
}

void RendererGL::drawRect(float x, float y, float w, float h, float aa) {
	auto shader = m_shaderMgr.load("shaders/shape.vert", "shaders/rect.frag");
	if (!shader) return;
	shader->use();
	glm::mat4 proj = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f);

	float cx = x + w * 0.5f, cy = y + h * 0.5f;
	float hx = w * 0.5f, hy = h * 0.5f;

	shader->setUniform("u_proj", proj);
	shader->setUniform("u_rectCenter", glm::vec2(cx, cy));
	shader->setUniform("u_halfExtents", glm::vec2(hx, hy));
	shader->setUniform("u_aa", aa);
	shader->setUniform("u_outlineColor", m_outlineColor);
	shader->setUniform("u_outlineWidth", m_outlineWidth);

	if (m_isGradient) {
		shader->setUniform("u_color0", m_gradColor0);
		shader->setUniform("u_color1", m_gradColor1);
		shader->setUniform("u_gradP0", m_gradP0);
		shader->setUniform("u_gradP1", m_gradP1);
		shader->setUniform("u_fillType", 1);
	}
	else {
		shader->setUniform("u_color", m_fillColor);
		shader->setUniform("u_fillType", 0);
	}

	float margin = aa + 2.0f;
	float x0 = x - margin, y0 = y - margin, x1 = x + w + margin, y1 = y + h + margin;
	std::vector<glm::vec2> verts = {
		{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}
	};

	m_vao.bind();
	m_vbo.bind();
	m_vbo.uploadData(verts.data(), verts.size() * sizeof(glm::vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	m_vao.unbind();
}

void RendererGL::drawRoundedRect(float x, float y, float w, float h, float radius, float aa) {
	auto shader = m_shaderMgr.load("shaders/shape.vert", "shaders/roundedrect.frag");
	if (!shader) return;
	shader->use();
	glm::mat4 proj = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f);

	float cx = x + w * 0.5f, cy = y + h * 0.5f;
	float hx = w * 0.5f, hy = h * 0.5f;

	shader->setUniform("u_proj", proj);
	shader->setUniform("u_rectCenter", glm::vec2(cx, cy));
	shader->setUniform("u_rectHalfExtents", glm::vec2(hx, hy));
	shader->setUniform("u_radius", radius);
	shader->setUniform("u_aa", aa);
	shader->setUniform("u_outlineColor", m_outlineColor);
	shader->setUniform("u_outlineWidth", m_outlineWidth);

	if (m_isGradient) {
		shader->setUniform("u_color0", m_gradColor0);
		shader->setUniform("u_color1", m_gradColor1);
		shader->setUniform("u_gradP0", m_gradP0);
		shader->setUniform("u_gradP1", m_gradP1);
		shader->setUniform("u_fillType", 1);
	}
	else {
		shader->setUniform("u_color", m_fillColor);
		shader->setUniform("u_fillType", 0);
	}

	float margin = aa + radius + 2.0f;
	float x0 = x - margin, y0 = y - margin, x1 = x + w + margin, y1 = y + h + margin;
	std::vector<glm::vec2> verts = {
		{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}
	};

	m_vao.bind();
	m_vbo.bind();
	m_vbo.uploadData(verts.data(), verts.size() * sizeof(glm::vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	m_vao.unbind();
}

void RendererGL::drawTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float aa) {
	auto shader = m_shaderMgr.load("shaders/shape.vert", "shaders/triangle.frag");
	if (!shader) return;
	shader->use();
	glm::mat4 proj = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f);

	shader->setUniform("u_proj", proj);
	shader->setUniform("u_a", a);
	shader->setUniform("u_b", b);
	shader->setUniform("u_c", c);
	shader->setUniform("u_aa", aa);
	shader->setUniform("u_outlineColor", m_outlineColor);
	shader->setUniform("u_outlineWidth", m_outlineWidth);

	if (m_isGradient) {
		shader->setUniform("u_color0", m_gradColor0);
		shader->setUniform("u_color1", m_gradColor1);
		shader->setUniform("u_gradP0", m_gradP0);
		shader->setUniform("u_gradP1", m_gradP1);
		shader->setUniform("u_fillType", 1);
	}
	else {
		shader->setUniform("u_color", m_fillColor);
		shader->setUniform("u_fillType", 0);
	}

	float margin = aa + 2.0f;
	float minX = std::min({ a.x, b.x, c.x }) - margin;
	float maxX = std::max({ a.x, b.x, c.x }) + margin;
	float minY = std::min({ a.y, b.y, c.y }) - margin;
	float maxY = std::max({ a.y, b.y, c.y }) + margin;
	std::vector<glm::vec2> verts = {
		{minX, minY}, {maxX, minY}, {maxX, maxY}, {minX, maxY}
	};

	m_vao.bind();
	m_vbo.bind();
	m_vbo.uploadData(verts.data(), verts.size() * sizeof(glm::vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	m_vao.unbind();
}

void RendererGL::drawLine(const glm::vec2& a, const glm::vec2& b, float width, float aa) {
	auto shader = m_shaderMgr.load("shaders/shape.vert", "shaders/line.frag");
	if (!shader) return;
	shader->use();
	glm::mat4 proj = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f);

	shader->setUniform("u_proj", proj);
	shader->setUniform("u_a", a);
	shader->setUniform("u_b", b);
	shader->setUniform("u_lineWidth", width);
	shader->setUniform("u_aa", aa);
	shader->setUniform("u_outlineColor", m_outlineColor);
	shader->setUniform("u_outlineWidth", m_outlineWidth);

	if (m_isGradient) {
		shader->setUniform("u_color0", m_gradColor0);
		shader->setUniform("u_color1", m_gradColor1);
		shader->setUniform("u_gradP0", m_gradP0);
		shader->setUniform("u_gradP1", m_gradP1);
		shader->setUniform("u_fillType", 1);
	}
	else {
		shader->setUniform("u_color", m_fillColor);
		shader->setUniform("u_fillType", 0);
	}

	float margin = aa + width + 2.0f;
	float minX = std::min(a.x, b.x) - margin;
	float maxX = std::max(a.x, b.x) + margin;
	float minY = std::min(a.y, b.y) - margin;
	float maxY = std::max(a.y, b.y) + margin;
	std::vector<glm::vec2> verts = {
		{minX, minY}, {maxX, minY}, {maxX, maxY}, {minX, maxY}
	};

	m_vao.bind();
	m_vbo.bind();
	m_vbo.uploadData(verts.data(), verts.size() * sizeof(glm::vec2));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	m_vao.unbind();
}

void RendererGL::setOutlineColor(const glm::vec4& color) {
	m_outlineColor = color;
}
void RendererGL::setOutlineWidth(float width) {
	m_outlineWidth = width;
}