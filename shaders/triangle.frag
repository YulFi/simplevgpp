// triangle.frag
#version 330 core

in vec2 fragPos;
out vec4 FragColor;

uniform vec2 u_a, u_b, u_c;
uniform float u_aa;          // Antialias radius
uniform int u_fillType;      // 0 = solid, 1 = gradient

uniform vec4 u_color;
uniform vec4 u_color0, u_color1;
uniform vec2 u_gradP0, u_gradP1;

uniform vec4 u_outlineColor;
uniform float u_outlineWidth;

// SDF for triangle (IQ version)
float sdTriangle(vec2 p, vec2 a, vec2 b, vec2 c) {
    vec2 ba = b - a, pa = p - a;
    vec2 cb = c - b, pb = p - b;
    vec2 ac = a - c, pc = p - c;
    vec2 nor = normalize(vec2(ba.y, -ba.x));
    float sign = (dot(nor, c - a) < 0.0) ? -1.0 : 1.0;
    float d1 = sign * (ba.y * pa.x - ba.x * pa.y) / length(ba);
    float d2 = sign * (cb.y * pb.x - cb.x * pb.y) / length(cb);
    float d3 = sign * (ac.y * pc.x - ac.x * pc.y) / length(ac);
    return -min(min(d1, d2), d3);
}

void main() {
    float dist = sdTriangle(fragPos, u_a, u_b, u_c);

    // Fill: solid (or gradient)
    float fillAlpha = 1.0 - smoothstep(-u_aa, u_aa, dist);

    vec4 fillCol = u_color;
    if (u_fillType == 1) {
        float t = dot(fragPos - u_gradP0, u_gradP1 - u_gradP0) /
                  dot(u_gradP1 - u_gradP0, u_gradP1 - u_gradP0);
        t = clamp(t, 0.0, 1.0);
        fillCol = mix(u_color0, u_color1, t);
    }

    // Outline: "band" from edge (dist ~ 0) outwards (dist ~ u_outlineWidth)
    float outlineBand = smoothstep(u_outlineWidth + u_aa, u_outlineWidth - u_aa, dist);
    float outlineAlpha = (1.0 - fillAlpha) * outlineBand;

    // Compose
    vec4 outCol = mix(fillCol, u_outlineColor, outlineAlpha);

    // Total alpha (never let outline hide fill)
    outCol.a *= max(fillAlpha, outlineAlpha);

    FragColor = outCol;
    if (FragColor.a < 0.01) discard;
}
