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
} in_;


// output variables, user defined (no output blocks in fragment shader)
out vec4 out_color;


// texture uniforms use layout semantic for easy client code flow
layout( binding = 0 ) uniform sampler2D diffuseTex;


// application entry point
void main()
{
	//out_color = texture( diffuseTex, in_.texCoord );
    //out_color = textureLod( diffuseTex, in_.texCoord, 0 );
	//out_color = vec4( abs( in_.position ), 0 );
	out_color = vec4( abs( in_.normal ), 0 );
	//out_color = vec4( in_.texCoord, 0, 0 );
	//out_color = vec4( 1, 1, 1, 0 );
}
