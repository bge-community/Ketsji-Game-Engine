uniform vec2 ScaleU;
uniform sampler2D textureSource;

void main()
{
	vec2 uv = gl_TexCoord[0].st;

#ifdef USE_DEPTH
	vec2 scale = ScaleU * texture2D(textureSource, uv).z;
#else
	vec2 scale = ScaleU;
#endif



	vec2 color = vec2(0.0);
	color += texture2D(textureSource, uv + vec2(-3.0, -3.0) * scale).xy * 0.015625;
	color += texture2D(textureSource, uv + vec2(-2.0, -2.0) * scale).xy * 0.09375;
	color += texture2D(textureSource, uv + vec2(-1.0, -1.0) * scale).xy * 0.234375;
	color += texture2D(textureSource, uv + vec2(0.0, 0.0)).xy * 0.3125;
	color += texture2D(textureSource, uv + vec2(1.0,  1.0) * scale).xy * 0.234375;
	color += texture2D(textureSource, uv + vec2(2.0,  2.0) * scale).xy * 0.09375;
	color += texture2D(textureSource, uv + vec2(3.0,  3.0) * scale).xy * 0.015625;


	gl_FragColor = vec4(color, texture2D(textureSource, uv).z, 1.0);
}
