#version 420 core


// layout specifier
//
// Specifies the format from the fixed function tesselator. So here is some 
// room to control how this is done. The fixed function tesselator only
// operates on per patch tesselation coordinates. For triangles this is the 
// same as barycentric coordinates and for quads it is the same as coordinates
// for linear interpolation. Tesselation coordinates is given in glTessCoord.
//
layout( quads, equal_spacing, ccw ) in;


// input blocks, built in and user defined
in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];
in PerVertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} in_[];


// output blocks, built in and user defined
out gl_PerVertex {
	vec4 gl_Position;
};
out PerVertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} out_;


// application entry point
void main()
{
	// triangle interpolation
	if ( false )
	{
		vec3 a, b, c;
		vec2 e, f, g;
		float s = gl_TessCoord.x;
		float t = gl_TessCoord.y;
		float u = gl_TessCoord.z;

		a = gl_in[0].gl_Position.xyz;
		b = gl_in[1].gl_Position.xyz;
		c = gl_in[2].gl_Position.xyz;
		gl_Position = vec4( s*a + t*b + u*c, 1.0 );

		a = in_[0].position;
		b = in_[1].position;
		c = in_[2].position;
		out_.position = s*a + t*b + u*c;

		a = in_[0].normal;
		b = in_[1].normal;
		c = in_[2].normal;
		out_.normal = s*a + t*b + u*c;

		e = in_[0].texCoord;
		f = in_[1].texCoord;
		g = in_[2].texCoord;
		out_.texCoord = s*e + t*f + u*g;
	}


	// quad interpolation
	else
	{
		vec3 a, b, c, d;
		vec2 e, f, g, h;
		float s = gl_TessCoord.x;
		float t = gl_TessCoord.y;
		
		a = gl_in[0].gl_Position.xyz;
		b = gl_in[1].gl_Position.xyz;
		c = gl_in[2].gl_Position.xyz;
		d = gl_in[3].gl_Position.xyz;
		gl_Position = vec4( mix( mix( a, b, s ), mix( d, c, s ), t ), 1.0 );

		a = in_[0].position;
		b = in_[1].position;
		c = in_[2].position;
		d = in_[3].position;
		out_.position = mix( mix( a, b, s ), mix( d, c, s ), t );

		a = in_[0].normal;
		b = in_[1].normal;
		c = in_[2].normal;
		d = in_[3].normal;
		out_.normal = mix( mix( a, b, s ), mix( d, c, s ), t );

		e = in_[0].texCoord;
		f = in_[1].texCoord;
		g = in_[2].texCoord;
		h = in_[3].texCoord;
		out_.texCoord = mix( mix( e, f, s ), mix( h, g, s ), t );
	}

}
