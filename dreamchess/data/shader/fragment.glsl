#ifdef GL_ES
	precision highp float;
#endif

varying vec2 vTexCoord;
varying vec4 vColour;

uniform int palette;
uniform int textureDepth;
uniform vec2 texOffset;
uniform sampler2D tex;
uniform sampler2D pal;

void main()
{
    // Depth of 0 means no texture..
	if (textureDepth == 0)
	    gl_FragColor = vColour;
	else if (textureDepth == 1)
	    gl_FragColor = vColour * vec4(1.0, 1.0, 1.0, texture2D(tex, vTexCoord).a);
	else
	    gl_FragColor = texture2D(tex, vTexCoord + texOffset) * vColour;
	    
	if (palette != 0 && texture2D(tex, vTexCoord).a == 1.0)
		gl_FragColor = texture2D(pal, vec2(texture2D(tex, vTexCoord).r, 0)) * vColour;    
}
