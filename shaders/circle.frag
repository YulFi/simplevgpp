#version 330 core

in vec2 fragPos;
out vec4 FragColor;

uniform vec2 u_center;
uniform float u_radius;
uniform float u_aa;
uniform int u_fillType; // 0 = solid, 1 = gradient

uniform vec4 u_color;
uniform vec4 u_color0, u_color1;
uniform vec2 u_gradP0, u_gradP1;

// Outline
uniform vec4 u_outlineColor;
uniform float u_outlineWidth;

void main() {
    float dist = length(fragPos - u_center) - u_radius;
    float halfInner = 0.0;
    float halfOuter = u_outlineWidth;

    // Fill
    float fillAlpha = 1.0 - smoothstep(halfInner, halfInner + u_aa, dist);
    vec4 fillCol = u_color;
    if (u_fillType == 1) {
        float t = dot(fragPos - u_gradP0, u_gradP1 - u_gradP0) /
                  dot(u_gradP1 - u_gradP0, u_gradP1 - u_gradP0);
        t = clamp(t, 0.0, 1.0);
        fillCol = mix(u_color0, u_color1, t);
    }

    // Outline
    float outlineAlpha = smoothstep(halfInner - u_aa, halfInner + u_aa, dist)
                       - smoothstep(halfOuter - u_aa, halfOuter + u_aa, dist);
    vec4 outlineCol = u_outlineColor;
    vec4 color = mix(fillCol, outlineCol, outlineAlpha);

    float alpha = max(fillAlpha, outlineAlpha);
    color.a *= alpha;

    FragColor = color;
    if (FragColor.a < 0.01)
        discard;
}
