#version 100
precision mediump float;

varying vec2 tex_coords;
varying vec4 fcolor;

uniform sampler2D texture;

void main() {
	vec4 c = texture2D(texture, tex_coords);

	if(c.a>0.0) {
		gl_FragColor = c * fcolor;
	} else
		discard;
}
