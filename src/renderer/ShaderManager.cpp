#include "renderer/ShaderManager.h"

ShaderGL* ShaderManager::load(const std::string& vsPath, const std::string& fsPath) {
	std::string key = makeKey(vsPath, fsPath);
	auto it = shaders.find(key);
	if (it != shaders.end())
		return it->second.get();

	auto shader = std::make_unique<ShaderGL>();
	if (shader->load(vsPath.c_str(), fsPath.c_str())) {
		ShaderGL* ptr = shader.get();
		shaders[key] = std::move(shader);
		return ptr;
	}
	return nullptr;
}

bool ShaderManager::reload(const std::string& vsPath, const std::string& fsPath) {
	std::string key = makeKey(vsPath, fsPath);
	auto it = shaders.find(key);
	if (it != shaders.end()) {
		auto shader = std::make_unique<ShaderGL>();
		if (shader->load(vsPath.c_str(), fsPath.c_str())) {
			shaders[key] = std::move(shader);
			return true;
		}
		return false;
	}
	// If not loaded, try to load
	return load(vsPath, fsPath) != nullptr;
}

ShaderGL* ShaderManager::get(const std::string& vsPath, const std::string& fsPath) const {
	std::string key = makeKey(vsPath, fsPath);
	auto it = shaders.find(key);
	return (it != shaders.end()) ? it->second.get() : nullptr;
}

void ShaderManager::remove(const std::string& vsPath, const std::string& fsPath) {
	std::string key = makeKey(vsPath, fsPath);
	shaders.erase(key);
}
