#version 330 core

in vec2 fragPos;
out vec4 FragColor;

uniform vec2 u_rectCenter;
uniform vec2 u_rectHalfExtents;
uniform float u_radius;
uniform float u_aa;
uniform int u_fillType;

uniform vec4 u_color;
uniform vec4 u_color0, u_color1;
uniform vec2 u_gradP0, u_gradP1;

uniform vec4 u_outlineColor;
uniform float u_outlineWidth;

// SDF for rounded rectangle (by Inigo Quilez)
float sdRoundBox(vec2 p, vec2 b, float r)
{
    vec2 q = abs(p) - b + vec2(r);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    vec2 local = fragPos - u_rectCenter;
    float dist = sdRoundBox(local, u_rectHalfExtents, u_radius);

    float halfInner = 0.0;
    float halfOuter = u_outlineWidth;

    float fillAlpha = 1.0 - smoothstep(halfInner, halfInner + u_aa, dist);
    vec4 fillCol = u_color;
    if (u_fillType == 1) {
        float t = dot(fragPos - u_gradP0, u_gradP1 - u_gradP0) /
                  dot(u_gradP1 - u_gradP0, u_gradP1 - u_gradP0);
        t = clamp(t, 0.0, 1.0);
        fillCol = mix(u_color0, u_color1, t);
    }

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
