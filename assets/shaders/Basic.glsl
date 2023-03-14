#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

uniform vec4 u_Color;

void main()
{
    color = u_Color;

    color2 = 50;
};