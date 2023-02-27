#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
uniform mat4 u_ViewProjection;

layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;

vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0)*100, vec3(-1, -1, 0)*100, vec3(-1, 1, 0)*100,
    vec3(-1, -1, 0)*100, vec3(1, 1, 0)*100, vec3(1, -1, 0)*100
);

vec3 UnprojectPoint(float x, float y, float z, mat4 viewproj)
{
    mat4 invViewProj = inverse(viewproj);
    vec4 unprojectedPoint = invViewProj * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, u_ViewProjection).xyz;
    farPoint = UnprojectPoint(p.x, p.y, 1.0, u_ViewProjection).xyz;
    gl_Position = vec4(p, 1.0);
}

#shader fragment
#version 330 core

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
uniform vec4 scale;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xy * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumy = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // y axis
    if(fragPos3D.x > -100 * minimumx && fragPos3D.x < 100 * minimumx)
        color.x = 1.0;
    // x axis
    if(fragPos3D.y > -100 * minimumy && fragPos3D.y < 100 * minimumy)
        color.z = 1.0;
    return color;
}

void main() {
    float t = -nearPoint.z / (farPoint.z - nearPoint.z);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    color = (vec4(0,0,0,0));
    color = grid(fragPos3D, 0.01);
}
