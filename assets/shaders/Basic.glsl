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

layout(location = 0) out vec4 color;
layout(location = 1) out int ID;

uniform vec4 u_Color;
uniform int u_ID;

void main()
{
    color = u_Color;
    ID = u_ID;
};