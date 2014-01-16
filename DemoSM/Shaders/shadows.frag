#version 420 core


// input blocks, built in and user defined
//
// Input isnt really PerVertex any more, but GL matches block names during 
// linking so we need to keep it the same.
//
in PerVertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec4 positionLgt;
} in_;


// output variables, user defined (no output blocks in fragment shader)
out vec4 out_color;


// user defined uniforms
uniform vec3 camPosition;
uniform vec3 lgtPosition;


// texture uniforms use layout semantic for easy client code flow
layout( binding = 0 ) uniform sampler2D depthTexture;


// application entry point
void main()
{
	mat3 m = mat3( 0, 0, 0.5, 0, 0, 0, 1, 0, 0 );

	vec3 V = normalize( camPosition );
	vec3 L = normalize( lgtPosition );
	vec2 T = 0.5 * (in_.positionLgt.xy / in_.positionLgt.w) + 0.5;

	float zFromCam = in_.positionLgt.w;
	float zFromLgt = texture( depthTexture, T );


	if ( 0 <= T.x && T.x <= 1 &&
		 0 <= T.y && T.y <= 1 )
	{
		out_color = vec4( (zFromLgt -150) / 100 );
	}
	else
	{
		out_color = vec4( 1, 0, 0, 0 );
	}




	//out_color = (vec4( depth ) - 150) / 100;//0.005 * in_.lgtPosition;

	//out_color = vec4( zFromLgt > (zFromCam -0.1) );
//	out_color = vec4( 0 < T.x || T.x > 1 || T.y < 0 || T.y > 1 );
}
