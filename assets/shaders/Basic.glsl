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
layout(location = 2) out int BorderType;

uniform vec4 u_Color;
uniform int u_ID;
uniform int u_Border;

void main()
{
    color = u_Color;
    ID = u_ID;
    BorderType = u_Border;
};