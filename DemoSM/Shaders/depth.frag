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
	float depth;
} in_;


// output variables, user defined (no output blocks in fragment shader)
//out vec4 out_color;
layout( location = 0 ) out float out_color;


// application entry point
void main()
{

	out_color = in_.depth;

}
