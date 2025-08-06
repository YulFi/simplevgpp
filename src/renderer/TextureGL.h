#pragma once
#include <glad/glad.h>

class TextureGL {
public:
	TextureGL();
	~TextureGL();

	TextureGL(const TextureGL&) = delete;
	TextureGL& operator=(const TextureGL&) = delete;
	TextureGL(TextureGL&& other) noexcept;
	TextureGL& operator=(TextureGL&& other) noexcept;

	void create2D(int width, int height, const void* data, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
	void bind(int unit = 0) const;
	GLuint id() const { return m_id; }

private:
	GLuint m_id = 0;
};
