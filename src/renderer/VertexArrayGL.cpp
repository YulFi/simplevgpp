// VertexArrayGL.cpp
#include "VertexArrayGL.h"

VertexArrayGL::VertexArrayGL() { glGenVertexArrays(1, &m_id); }
VertexArrayGL::~VertexArrayGL() { if (m_id) glDeleteVertexArrays(1, &m_id); }
VertexArrayGL::VertexArrayGL(VertexArrayGL&& other) noexcept { m_id = other.m_id; other.m_id = 0; }
VertexArrayGL& VertexArrayGL::operator=(VertexArrayGL&& other) noexcept {
    if (this != &other) {
        if (m_id) glDeleteVertexArrays(1, &m_id);
        m_id = other.m_id; other.m_id = 0;
    }
    return *this;
}
void VertexArrayGL::bind() const { glBindVertexArray(m_id); }
void VertexArrayGL::unbind() const { glBindVertexArray(0); }

// --- Attribute helper ---
void VertexArrayGL::addAttribute(GLuint index, GLint size, GLenum type, bool normalized, GLsizei stride, size_t offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, (const void*)offset);
}
