#include "renderer/TextureGL.h"

TextureGL::TextureGL() { glGenTextures(1, &m_id); }
TextureGL::~TextureGL() { if (m_id) glDeleteTextures(1, &m_id); }
TextureGL::TextureGL(TextureGL&& other) noexcept : m_id(other.m_id) { other.m_id = 0; }
TextureGL& TextureGL::operator=(TextureGL&& other) noexcept {
	if (this != &other) {
		if (m_id) glDeleteTextures(1, &m_id);
		m_id = other.m_id;
		other.m_id = 0;
	}
	return *this;
}
void TextureGL::create2D(int width, int height, const void* data, GLenum format, GLenum type) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void TextureGL::bind(int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_id);
}
