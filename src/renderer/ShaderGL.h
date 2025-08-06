#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class ShaderGL {
public:
	ShaderGL();
	~ShaderGL();

	ShaderGL(const ShaderGL&) = delete;
	ShaderGL& operator=(const ShaderGL&) = delete;
	ShaderGL(ShaderGL&& other) noexcept;
	ShaderGL& operator=(ShaderGL&& other) noexcept;

	bool load(const char* vsPath, const char* fsPath);
	void use() const;
	GLuint id() const { return m_id; }

	// Uniform helpers (minimal set, extend as needed)
	void setUniform(const char* name, int v) const;
	void setUniform(const char* name, float v) const;
	void setUniform(const char* name, const glm::vec2& v) const;
	void setUniform(const char* name, const glm::vec4& v) const;
	void setUniform(const char* name, const glm::mat4& m) const;

private:
	GLuint m_id = 0;
};
