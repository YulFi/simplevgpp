#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "renderer/ShaderGL.h"

class ShaderManager {
public:
	ShaderManager() = default;
	~ShaderManager() = default;

	// Returns a pointer to the shader, or nullptr on failure
	ShaderGL* load(const std::string& vsPath, const std::string& fsPath);

	// Optionally: reload a shader (for hot-reload tools)
	bool reload(const std::string& vsPath, const std::string& fsPath);

	// Get raw pointer (returns nullptr if not loaded)
	ShaderGL* get(const std::string& vsPath, const std::string& fsPath) const;

	// (Optional) remove a shader from manager
	void remove(const std::string& vsPath, const std::string& fsPath);

	// (Optional) total managed shaders
	size_t count() const { return shaders.size(); }

private:
	std::string makeKey(const std::string& vs, const std::string& fs) const {
		return vs + "||" + fs;
	}
	std::unordered_map<std::string, std::unique_ptr<ShaderGL>> shaders;
};
