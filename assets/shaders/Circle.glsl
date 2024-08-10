#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * position;
};

#shader fragment
#version 450 core

layout (location = 0) out vec4 color;
layout (location = 3) out int ID; 

struct VertexOutput
{
    vec3 LocalPosition;
    vec4 Color;
    float Thickness;
    float Fade;
};

uniform vec2 u_LocalPosition;
uniform float u_Thickness;
uniform float u_Fade; 
uniform vec4 u_Color;
uniform int u_ID;

void main()
{
    // Parameters
    vec3 circleColor = vec3(0.85, 0.35, 0.2);
    float thickness = 1;
    float fade = 0.005;

    // -1 -> 1 local space, adjusted for aspect ratio
    
    // Calculate distance and fill circle with white
    vec2 uv = vec2(gl_FragCoord.x*1280, gl_FragCoord.y*1024);
    float distance = 1.0 - length(uv);
    vec3 circolor = vec3(smoothstep(0.0, fade, distance));
    circolor *= vec3(smoothstep(thickness + fade, thickness, distance));

    color = vec4(circolor, 1.0);
    color.rgb *= circleColor;

    ID = u_ID;
};