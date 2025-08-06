#include "renderer/ShaderGL.h"
#include <fstream>
#include <sstream>
#include <iostream>

static GLuint compile(GLenum type, const std::string& src) {
	GLuint shader = glCreateShader(type);
	const char* cstr = src.c_str();
	glShaderSource(shader, 1, &cstr, nullptr);
	glCompileShader(shader);
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[512]; glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cerr << "Shader compile error: " << log << std::endl;
	}
	return shader;
}

ShaderGL::ShaderGL() {}
ShaderGL::~ShaderGL() { if (m_id) glDeleteProgram(m_id); }
ShaderGL::ShaderGL(ShaderGL&& other) noexcept : m_id(other.m_id) { other.m_id = 0; }
ShaderGL& ShaderGL::operator=(ShaderGL&& other) noexcept {
	if (this != &other) {
		if (m_id) glDeleteProgram(m_id);
		m_id = other.m_id;
		other.m_id = 0;
	}
	return *this;
}

bool ShaderGL::load(const char* vsPath, const char* fsPath) {
	std::ifstream vsFile(vsPath), fsFile(fsPath);
	if (!vsFile || !fsFile) return false;
	std::stringstream vss, fss;
	vss << vsFile.rdbuf(); fss << fsFile.rdbuf();
	GLuint vs = compile(GL_VERTEX_SHADER, vss.str());
	GLuint fs = compile(GL_FRAGMENT_SHADER, fss.str());
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs); glAttachShader(prog, fs);
	glLinkProgram(prog);
	int status; glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (!status) {
		char log[512]; glGetProgramInfoLog(prog, 512, nullptr, log);
		std::cerr << "Shader link error: " << log << std::endl;
		glDeleteShader(vs); glDeleteShader(fs); glDeleteProgram(prog);
		return false;
	}
	glDeleteShader(vs); glDeleteShader(fs);
	if (m_id) glDeleteProgram(m_id);
	m_id = prog;
	return true;
}
void ShaderGL::use() const { glUseProgram(m_id); }
void ShaderGL::setUniform(const char* name, int v) const { glUniform1i(glGetUniformLocation(m_id, name), v); }
void ShaderGL::setUniform(const char* name, float v) const { glUniform1f(glGetUniformLocation(m_id, name), v); }
void ShaderGL::setUniform(const char* name, const glm::vec2& v) const { glUniform2fv(glGetUniformLocation(m_id, name), 1, &v[0]); }
void ShaderGL::setUniform(const char* name, const glm::vec4& v) const { glUniform4fv(glGetUniformLocation(m_id, name), 1, &v[0]); }
void ShaderGL::setUniform(const char* name, const glm::mat4& m) const { glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &m[0][0]); }
