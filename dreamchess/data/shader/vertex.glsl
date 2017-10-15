#ifdef GL_ES
	precision highp float;
#endif

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

attribute vec3 aPosition;
attribute vec4 aColour;
attribute vec2 aTexCoord;
attribute float aCurvature;
attribute vec4 aColourMod;

varying vec2 vTexCoord;
varying vec4 vColour;

void main() {    
	vColour = aColour*aColourMod;
	vTexCoord = aTexCoord;
    vec4 pos = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
       
	gl_Position = pos;
}
