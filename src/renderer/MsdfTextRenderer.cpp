#include "renderer/MsdfTextRenderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct GlyphMetrics {
	// Atlas coordinates (normalized 0-1)
	float atlasLeft, atlasBottom, atlasRight, atlasTop;

	// Glyph positioning
	float planeBounds[4]; // left, bottom, right, top
	float atlasBounds[4]; // left, bottom, right, top (pixel coordinates)

	// Typography metrics
	float advance;

	// Unicode codepoint
	uint32_t unicode;
};

struct FontAtlas {
	std::unordered_map<uint32_t, GlyphMetrics> glyphs;

	// Font metrics
	float ascender;
	float descender;
	float lineHeight;
	float underlineY;
	float underlineThickness;

	// Atlas properties
	int atlasWidth;
	int atlasHeight;
	float distanceRange;
	float size;

	// OpenGL texture
	GLuint textureID;

	void loadFromFile(const std::string& jsonPath);
};

void FontAtlas::loadFromFile(const std::string& jsonPath) {
	// Load and parse JSON
	std::ifstream file(jsonPath, std::ifstream::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open JSON file: " << jsonPath << std::endl;
	}

	json fontData;
	try {
		file >> fontData;
	}
	catch (const json::exception& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
	}

	// Parse atlas properties
	if (fontData.contains("atlas")) {
		const auto& atlas = fontData["atlas"];
		atlasWidth = atlas["width"];
		atlasHeight = atlas["height"];
		distanceRange = atlas["distanceRange"];
		size = atlas["size"];
	}

	// Parse font metrics
	if (fontData.contains("metrics")) {
		const auto& metrics = fontData["metrics"];
		ascender = metrics["ascender"];
		descender = metrics["descender"];
		lineHeight = metrics["lineHeight"];

		if (metrics.contains("underlineY"))
			underlineY = metrics["underlineY"];
		if (metrics.contains("underlineThickness"))
			underlineThickness = metrics["underlineThickness"];
	}

	// Parse glyphs
	if (fontData.contains("glyphs")) {
		for (const auto& glyphData : fontData["glyphs"]) {
			GlyphMetrics glyph = {};

			// Unicode codepoint
			glyph.unicode = glyphData["unicode"];

			// Advance
			glyph.advance = glyphData["advance"];

			// Plane bounds (glyph positioning)
			if (glyphData.contains("planeBounds")) {
				const auto& pb = glyphData["planeBounds"];
				glyph.planeBounds[0] = pb["left"];
				glyph.planeBounds[1] = pb["bottom"];
				glyph.planeBounds[2] = pb["right"];
				glyph.planeBounds[3] = pb["top"];
			}

			// Atlas bounds (texture coordinates in pixels)
			if (glyphData.contains("atlasBounds")) {
				const auto& ab = glyphData["atlasBounds"];
				glyph.atlasBounds[0] = ab["left"];
				glyph.atlasBounds[1] = ab["bottom"];
				glyph.atlasBounds[2] = ab["right"];
				glyph.atlasBounds[3] = ab["top"];

				// Convert to normalized texture coordinates (0-1)
				glyph.atlasLeft = (float)ab["left"] / atlasWidth;
				glyph.atlasBottom = (float)ab["bottom"] / atlasHeight;
				glyph.atlasRight = (float)ab["right"] / atlasWidth;
				glyph.atlasTop = (float)ab["top"] / atlasHeight;
			}

			// Store glyph
			glyphs[glyph.unicode] = glyph;
		}
	}
}


// Helper: Convert UTF-8 to UTF-32 for codepoint iteration (minimal)
static std::u32string utf8_to_utf32(const std::string& s) {
	std::u32string result;
	size_t i = 0, n = s.size();
	while (i < n) {
		uint32_t c = (unsigned char)s[i++];
		if (c < 0x80) { result += c; continue; }
		if ((c & 0xE0) == 0xC0 && i < n) {
			result += ((c & 0x1F) << 6) | (s[i++] & 0x3F); continue;
		}
		if ((c & 0xF0) == 0xE0 && i + 1 < n) {
			result += ((c & 0x0F) << 12) | ((s[i++] & 0x3F) << 6) | (s[i++] & 0x3F); continue;
		}
		if ((c & 0xF8) == 0xF0 && i + 2 < n) {
			result += ((c & 0x07) << 18) | ((s[i++] & 0x3F) << 12) | ((s[i++] & 0x3F) << 6) | (s[i++] & 0x3F); continue;
		}
		result += '?';
	}
	return result;
}

MsdfTextRenderer::MsdfTextRenderer() {}
MsdfTextRenderer::~MsdfTextRenderer() {}

bool MsdfTextRenderer::load(const std::string& atlasPng, const std::string& metadataJson, float pxRange) {
	m_pxRange = pxRange;
	// Load texture
	int w, h, comp;
	stbi_uc* data = stbi_load(atlasPng.c_str(), &w, &h, &comp, 4);
	if (!data) { std::cerr << "Cannot load MSDF atlas: " << atlasPng << std::endl; return false; }
	m_atlas.create2D(w, h, data, GL_RGBA, GL_UNSIGNED_BYTE);
	m_atlasW = float(w); m_atlasH = float(h);
	stbi_image_free(data);

	//FontAtlas atlas;
	//atlas.loadFromFile(metadataJson);

	// Parse JSON
	parseJson(metadataJson);

	// Load shaders
	m_shader.load("shaders/msdf_text.vert", "shaders/msdf_text.frag");

	// Setup VAO/VBO (dynamic draw)
	m_vao = VertexArrayGL();
	m_vbo = BufferGL(GL_ARRAY_BUFFER);
	m_vao.bind();
	m_vbo.bind();
	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // uv
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	m_vbo.unbind();
	m_vao.unbind();

	m_ready = true;
	return true;
}

void MsdfTextRenderer::parseJson(const std::string& path) 
{
	std::ifstream file(path);
	nlohmann::json j;
	file >> j;

	// Example: msdf-atlas-gen JSON format
	auto& metrics = j["metrics"];
	m_ascent = metrics.value("ascender", 0.0f);
	m_descent = metrics.value("descender", 0.0f);
	m_lineGap = metrics.value("lineHeight", 0.0f) - m_ascent + m_descent;

	//// Parse atlas properties
	//if (j.contains("atlas")) {
 //       //const auto& atlas = fontData["atlas"];
 //       //atlasWidth = atlas["width"];
 //       //atlasHeight = atlas["height"];
 //       //distanceRange = atlas["distanceRange"];
 //       //size = atlas["size"];
	//}

	//// Parse font metrics
	//if (j.contains("metrics")) {
 //       //const auto& metrics = fontData["metrics"];
 //       //ascender = metrics["ascender"];
 //       //descender = metrics["descender"];
 //       //lineHeight = metrics["lineHeight"];

 //       //if (metrics.contains("underlineY"))
 //       //    underlineY = metrics["underlineY"];
 //       //if (metrics.contains("underlineThickness"))
 //       //    underlineThickness = metrics["underlineThickness"];
	//}

	//// Parse glyphs
	//if (j.contains("glyphs")) {
	//	for (const auto& glyphData : j["glyphs"]) {
	//		int i = 0;
	//	}
	//}

	//for (auto& g : j["glyphs"]) 
	//{
	//	uint32_t code = g.value("unicode", 0);

	//	MsdfGlyph glyph;

	//	glyph.uvRect = glm::vec4(
	//		g["atlasBounds"]["left"].get<float>() / m_atlasW,
	//		g["atlasBounds"]["bottom"].get<float>() / m_atlasH,
	//		g["atlasBounds"]["right"].get<float>() / m_atlasW,
	//		g["atlasBounds"]["top"].get<float>() / m_atlasH
	//	);
	//	glyph.size = glm::vec2(
	//		g["planeBounds"]["right"].get<float>() - g["planeBounds"]["left"].get<float>(),
	//		g["planeBounds"]["top"].get<float>() - g["planeBounds"]["bottom"].get<float>()
	//	);
	//	glyph.bearing = glm::vec2(
	//		g["planeBounds"]["left"].get<float>(),
	//		g["planeBounds"]["bottom"].get<float>()
	//	);
	//	glyph.advance = g.value("advance", glyph.size.x);
	//	glyphs[code] = glyph;
	//}

    // Parse glyphs
    if (j.contains("glyphs")) 
	{
		for (auto& g : j["glyphs"])
		{
            uint32_t code = g.value("unicode", 0);

            MsdfGlyph glyph;

            // Advance
            glyph.advance = g["advance"];

            // Plane bounds (glyph positioning)
            if (g.contains("planeBounds")) {
				glyph.size = glm::vec2(
					g["planeBounds"]["right"].get<float>() - g["planeBounds"]["left"].get<float>(),
					g["planeBounds"]["top"].get<float>() - g["planeBounds"]["bottom"].get<float>());

                glyph.bearing = glm::vec2(
					g["planeBounds"]["left"].get<float>(),
                    g["planeBounds"]["bottom"].get<float>());
            }

            // Atlas bounds (texture coordinates in pixels)
            if (g.contains("atlasBounds")) {
                glyph.uvRect = glm::vec4(
                    g["atlasBounds"]["left"].get<float>() / m_atlasW,
                    g["atlasBounds"]["bottom"].get<float>() / m_atlasH,
                    g["atlasBounds"]["right"].get<float>() / m_atlasW,
                    g["atlasBounds"]["top"].get<float>() / m_atlasH
                );
            }

            // Store glyph
            glyphs[code] = glyph;
        }
    }
}

void MsdfTextRenderer::buildVertexBuffer(const std::u32string& text, float x, float y, float scale, std::vector<float>& buf, int& quads) const {
	float penX = x, penY = y;
	quads = 0;
	for (char32_t ch : text) {
		auto it = glyphs.find(ch);
		if (it == glyphs.end()) continue;
		const MsdfGlyph& g = it->second;

		float xpos = penX + g.bearing.x * scale;
		//float ypos = penY - g.bearing.y * scale;

		float ypos = y + (g.size.y - g.bearing.y);

		float w = g.size.x * scale, h = g.size.y * scale;

		float u0 = g.uvRect.x, v0 = 1.f - g.uvRect.y, u1 = g.uvRect.z, v1 = 1.f - g.uvRect.w;

		// 2 triangles per quad (CCW)
		buf.insert(buf.end(), {
			xpos,     ypos - h, u0, v1,
			xpos,     ypos,     u0, v0,
			xpos + w, ypos,     u1, v0,
			xpos,     ypos - h, u0, v1,
			xpos + w, ypos,     u1, v0,
			xpos + w, ypos - h, u1, v1
			});
		penX += g.advance * scale;
		++quads;
	}
}

void MsdfTextRenderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec4& color, const glm::mat4& proj) 
{
    if (!m_ready) 
		return;

    std::u32string str = utf8_to_utf32(text);
    std::vector<float> verts;
    int quads = 0;

    buildVertexBuffer(str, x, y, scale, verts, quads);

    if (quads == 0) 
		return;

    m_vao.bind();
    m_vbo.bind();
    m_vao.addAttribute(0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);                 // position: x, y
    m_vao.addAttribute(1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float)); // uv:      u, v
    m_vbo.uploadData(verts.data(), verts.size() * sizeof(float));

    m_shader.use();
    m_shader.setUniform("u_proj", proj);
    m_shader.setUniform("u_color", color);
    m_shader.setUniform("u_pxRange", m_pxRange);
    m_shader.setUniform("u_msdfAtlas", 0);

    m_atlas.bind(0);
    glDrawArrays(GL_TRIANGLES, 0, 6 * quads);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    m_vao.unbind();
}

glm::vec2 MsdfTextRenderer::measureText(const std::string& text, float scale) const {
	float width = 0;
	float maxH = 0;
	std::u32string str = utf8_to_utf32(text);
	for (char32_t ch : str) {
		auto it = glyphs.find(ch);
		if (it == glyphs.end()) continue;
		const MsdfGlyph& g = it->second;
		width += g.advance * scale;
		maxH = std::max(maxH, g.size.y * scale);
	}
	return { width, maxH };
}
