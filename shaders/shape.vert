// [shape].vert
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 u_proj;
out vec2 fragPos;
void main() {
    fragPos = aPos;
    gl_Position = u_proj * vec4(aPos, 0.0, 1.0);
}
