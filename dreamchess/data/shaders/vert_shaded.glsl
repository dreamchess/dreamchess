#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColour;

out vec4 fColour;

uniform mat4 model;
uniform mat4 projection;

void main() {
    gl_Position = projection * model * vec4(aPos.x, aPos.y, 1.0, 1.0);
    fColour = aColour;
}
