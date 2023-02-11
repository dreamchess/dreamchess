#version 330 core

in vec4 fColour;

uniform sampler2D texture1;
uniform vec4 colour;

void main() {
    gl_FragColor = fColour * colour;
}
