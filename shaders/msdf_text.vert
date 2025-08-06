#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUv;
out vec2 vUv;
uniform mat4 u_proj;
void main() {
    vUv = aUv;
    gl_Position = u_proj * vec4(aPos, 0.0, 1.0);
}
