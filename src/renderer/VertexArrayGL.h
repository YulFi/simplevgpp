// VertexArrayGL.h
#pragma once
#include <glad/glad.h>

class VertexArrayGL {
public:
    VertexArrayGL();
    ~VertexArrayGL();

    VertexArrayGL(const VertexArrayGL&) = delete;
    VertexArrayGL& operator=(const VertexArrayGL&) = delete;
    VertexArrayGL(VertexArrayGL&& other) noexcept;
    VertexArrayGL& operator=(VertexArrayGL&& other) noexcept;

    void bind() const;
    void unbind() const;
    GLuint id() const { return m_id; }

    // --- NEW: attribute helper ---
    // Typical usage: addAttribute(0, 2, GL_FLOAT, false, 16, 0); // x, y
    //                addAttribute(1, 2, GL_FLOAT, false, 16, 8); // u, v
    void addAttribute(GLuint index, GLint size, GLenum type, bool normalized, GLsizei stride, size_t offset);

private:
    GLuint m_id = 0;
};
