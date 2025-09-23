#version 460 core

flat in int vert_type;
in vec4 vert_color;
in vec2 vert_texCoords;
flat in int vert_texSlot;
in float vert_innerDiameter;
in vec2 vert_uv;
in vec4 vert_quadPosSize;

uniform sampler2D u_textures[16];

out vec4 out_color;


vec4 Rectangle();
vec4 Circle();
vec4 MSDF();
vec4 Mask();


void main()
{
    switch (vert_type)
    {
        case 0:
            out_color = Rectangle();
            break;

        case 1:
            out_color = Circle();
            break;

        case 2:
            out_color = MSDF();
            break;

        case 3:
            out_color = Mask();
            break;

        default:
            out_color = vec4(vec3(0), 1);
    }
}



vec4 GetTexture(int texSlot, vec2 texCoord) {
    switch (texSlot) {
        case 0: return texture(u_textures[0], texCoord);
        case 1: return texture(u_textures[1], texCoord);
        case 2: return texture(u_textures[2], texCoord);
        case 3: return texture(u_textures[3], texCoord);
        case 4: return texture(u_textures[4], texCoord);
        case 5: return texture(u_textures[5], texCoord);
        case 6: return texture(u_textures[6], texCoord);
        case 7: return texture(u_textures[7], texCoord);
        case 8: return texture(u_textures[8], texCoord);
        case 9: return texture(u_textures[9], texCoord);
        case 10: return texture(u_textures[10], texCoord);
        case 11: return texture(u_textures[11], texCoord);
        case 12: return texture(u_textures[12], texCoord);
        case 13: return texture(u_textures[13], texCoord);
        case 14: return texture(u_textures[14], texCoord);
        case 15: return texture(u_textures[15], texCoord);
        default: return vec4(vec3(0), 1);
    }
}

ivec2 GetTexSize(int texSlot, int lod) {
    switch (texSlot) {
        case 0: return textureSize(u_textures[0], lod);
        case 1: return textureSize(u_textures[1], lod);
        case 2: return textureSize(u_textures[2], lod);
        case 3: return textureSize(u_textures[3], lod);
        case 4: return textureSize(u_textures[4], lod);
        case 5: return textureSize(u_textures[5], lod);
        case 6: return textureSize(u_textures[6], lod);
        case 7: return textureSize(u_textures[7], lod);
        case 8: return textureSize(u_textures[8], lod);
        case 9: return textureSize(u_textures[9], lod);
        case 10: return textureSize(u_textures[10], lod);
        case 11: return textureSize(u_textures[11], lod);
        case 12: return textureSize(u_textures[12], lod);
        case 13: return textureSize(u_textures[13], lod);
        case 14: return textureSize(u_textures[14], lod);
        case 15: return textureSize(u_textures[15], lod);
    }
}


vec4 Rectangle()
{
    vec4 color;
    if (vert_texSlot == -1)
    {
        color = vert_color;
    }

    else
    {
        color = GetTexture(vert_texSlot, vert_texCoords);
    }

    return color;
}


vec4 Circle()
{
    vec4 color;
    if (vert_texSlot == -1)
    {
        color = vert_color;
    }

    else
    {
        color = GetTexture(vert_texSlot, vert_texCoords);
    }

    float distance_ = 1.0 - length(vert_uv);
    color.a *= smoothstep(0.0, 0.005, distance_);

    float thickness = 1.0 - vert_innerDiameter;
    if (thickness != 1.0)
    {
        color.a *= 1.0 - smoothstep(thickness - 0.005, thickness, distance_);
    }

    return color;
}






float screenPxRange() {
    const float pxRange = 6.0;

    vec2 unitRange = vec2(pxRange) / vec2(GetTexSize(vert_texSlot, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(vert_texCoords);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

vec4 MSDF()
{
    vec4 msd = GetTexture(vert_texSlot, vert_texCoords);
    float sd = median(msd.r, msd.g, msd.b);

    float screenPxDistance = screenPxRange() * (sd - 0.5);

    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    return vec4(vert_color.rgb, vert_color.a * opacity);
}

vec4 Mask() { return vec4(vec3(1), 1); }
