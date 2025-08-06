#include "renderer/BufferGL.h"

BufferGL::BufferGL(GLenum type) : m_type(type) {
	glGenBuffers(1, &m_id);
}

BufferGL::~BufferGL() {
	if (m_id) glDeleteBuffers(1, &m_id);
}

BufferGL::BufferGL(BufferGL&& other) noexcept
	: m_id(other.m_id), m_type(other.m_type) {
	other.m_id = 0;
}

BufferGL& BufferGL::operator=(BufferGL&& other) noexcept {
	if (this != &other) {
		if (m_id) glDeleteBuffers(1, &m_id);
		m_id = other.m_id;
		m_type = other.m_type;
		other.m_id = 0;
	}
	return *this;
}

void BufferGL::bind() const {
	glBindBuffer(m_type, m_id);
}

void BufferGL::unbind() const {
	glBindBuffer(m_type, 0);
}

void BufferGL::uploadData(const void* data, GLsizeiptr size, GLenum usage) const {
	//glBindBuffer(m_type, m_id);
	glBufferData(m_type, size, data, usage);
}
