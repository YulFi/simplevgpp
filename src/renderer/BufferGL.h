#pragma once

#include <glad/glad.h>

class BufferGL {
public:
	BufferGL(GLenum type = GL_ARRAY_BUFFER);
	~BufferGL();

	BufferGL(const BufferGL&) = delete;
	BufferGL& operator=(const BufferGL&) = delete;

	BufferGL(BufferGL&& other) noexcept;
	BufferGL& operator=(BufferGL&& other) noexcept;

	void bind() const;
	void unbind() const;
	void uploadData(const void* data, GLsizeiptr size, GLenum usage = GL_DYNAMIC_DRAW) const;

	GLuint id() const { return m_id; }
	GLenum type() const { return m_type; }

private:
	GLuint m_id = 0;
	GLenum m_type = GL_ARRAY_BUFFER;
};
