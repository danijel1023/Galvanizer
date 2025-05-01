#version 460 core

layout (location = 0) in int cpu_type;
layout (location = 1) in vec2 cpu_pos;
layout (location = 2) in vec4 cpu_color;
layout (location = 3) in vec2 cpu_rotCenter;
layout (location = 4) in vec2 cpu_texCoords;
layout (location = 5) in int cpu_texSlot;
layout (location = 6) in float cpu_rotation;
layout (location = 7) in float cpu_innerDiameter;
layout (location = 8) in vec4 cpu_quadPosSize;

uniform ivec2 u_winSize;


flat out int vert_type;
out vec4 vert_color;
out vec2 vert_texCoords;
flat out int vert_texSlot;
out float vert_innerDiameter;
out vec2 vert_uv;


void main()
{
    float rad = radians(cpu_rotation);
    vec2 pos = cpu_pos;

    vert_uv = (pos - cpu_quadPosSize.xy) / cpu_quadPosSize.zw * 2.0 - 1.0;

    pos -= cpu_rotCenter;
    pos = vec2(pos.x * vec2(cos(rad), sin(rad))) + vec2(pos.y * vec2(-sin(rad), cos(rad)));
    pos += cpu_rotCenter;

    vec2 normalisationVec = vec2(u_winSize) / 2;
    vec2 invertedVertex = vec2(pos.x, pos.y);

    // Normalized pos
    vec2 npos = (invertedVertex / normalisationVec) - vec2(1);
    gl_Position = vec4(npos, 0.0, 1.0);


    vert_color = cpu_color;
    vert_texCoords = cpu_texCoords;
    vert_texSlot = cpu_texSlot;
    vert_type = cpu_type;
    vert_innerDiameter = cpu_innerDiameter;
}
