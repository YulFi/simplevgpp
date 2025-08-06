#pragma once
#include <glm/glm.hpp>
class Line {
public:
	Line(const glm::vec2& a, const glm::vec2& b);
	glm::vec2 a, b;
};
