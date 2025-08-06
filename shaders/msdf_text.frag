#version 330 core

in vec2 vUv;
out vec4 FragColor;
uniform sampler2D u_msdfAtlas;
uniform vec4 u_color;
uniform float u_pxRange;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}
void main() {
    vec3 msdf = texture(u_msdfAtlas, vUv).rgb;
    float sd = median(msdf.r, msdf.g, msdf.b) - 0.5;
    float pxDist = sd * u_pxRange;
    float alpha = clamp(pxDist + 0.5, 0.0, 1.0);
    FragColor = vec4(u_color.rgb, u_color.a * alpha);
    //FragColor = vec4(1.0,1.0,0.0,1.0);
}
