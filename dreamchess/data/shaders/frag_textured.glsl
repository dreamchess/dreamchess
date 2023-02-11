#version 330 core

in vec2 fTexCoord;

uniform sampler2D texture1;
uniform vec4 colour;

void main() {
    gl_FragColor = texture(texture1, fTexCoord) * colour;
}
