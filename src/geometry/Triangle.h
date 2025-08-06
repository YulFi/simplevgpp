#pragma once
#include <glm/glm.hpp>
class Triangle {
public:
	Triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
	glm::vec2 a, b, c;
};
