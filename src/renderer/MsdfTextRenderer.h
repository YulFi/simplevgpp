#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "renderer/ShaderGL.h"
#include "renderer/TextureGL.h"
#include "renderer/BufferGL.h"
#include "renderer/VertexArrayGL.h"

// Per-glyph metadata (from msdf-atlas-gen)
struct MsdfGlyph {
	glm::vec4 uvRect;   // [u0,v0, u1,v1]
	glm::vec2 size;     // in px
	glm::vec2 bearing;  // offset from baseline (px)
	float advance;      // px
};

class MsdfTextRenderer {
public:
	MsdfTextRenderer();
	~MsdfTextRenderer();

	// Load atlas image and msdf-atlas-gen JSON (BMFont or JSON)
	bool load(const std::string& atlasPng, const std::string& metadataJson, float pxRange = 4.0f);

	// Draw text (UTF-8, left baseline)
	void drawText(const std::string& text, float x, float y, float scale, const glm::vec4& color, const glm::mat4& proj);

	// Compute bounding box
	glm::vec2 measureText(const std::string& text, float scale = 1.0f) const;

private:
	std::unordered_map<uint32_t, MsdfGlyph> glyphs;
	float m_pxRange = 4.0f;
	float m_atlasW = 0, m_atlasH = 0;
	float m_ascent = 0, m_descent = 0, m_lineGap = 0;

	ShaderGL m_shader;
	TextureGL m_atlas;
	VertexArrayGL m_vao;
	BufferGL m_vbo;
	bool m_ready = false;

	void parseJson(const std::string& path);
	void buildVertexBuffer(const std::u32string& text, float x, float y, float scale, std::vector<float>& buf, int& quads) const;
};
