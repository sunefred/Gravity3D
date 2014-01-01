#version 420 core


layout( quads, equal_spacing, ccw ) in;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
	vec4 gl_Position;
};


void main()
{
	// triangle interpolation
	if ( false )
	{
		vec3 a = gl_in[0].gl_Position.xyz;
		vec3 b = gl_in[1].gl_Position.xyz;
		vec3 c = gl_in[2].gl_Position.xyz;
		float s = gl_TessCoord.x;
		float t = gl_TessCoord.y;
		float u = gl_TessCoord.z;
		gl_Position = vec4( s*a + t*b + u*c, 1.0 );
	}


	// quad interpolation
	else
	{
		vec3 a = gl_in[0].gl_Position.xyz;
		vec3 b = gl_in[1].gl_Position.xyz;
		vec3 c = gl_in[2].gl_Position.xyz;
		vec3 d = gl_in[3].gl_Position.xyz;
		float s = gl_TessCoord.x;
		float t = gl_TessCoord.y;
		gl_Position = vec4( mix( mix( a, b, s ), mix( d, c, s ), t ), 1.0 );
	}

}
