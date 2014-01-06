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
	vec3 distance;
} in_;


// output variables, user defined (no output blocks in fragment shader)
out vec4 out_color;


// user defined uniforms
uniform vec4 solidColor;
uniform vec4 wireColor;


// texture uniforms use layout semantic for easy client code flow
layout( binding = 0 ) uniform sampler2D diffuseTex;



// application entry point
void main()
{
	float LineWidth = 1, distance, alpha;
	distance = min( in_.distance.x, min( in_.distance.y, in_.distance.z ) );

	if ( distance > 0.5*LineWidth+1 )
	{
		out_color = vec4( 0, 0, 0, 0 );
	}
	else
	{
		distance = clamp( ( distance - ( 0.5 * LineWidth - 1 ) ), 0, 2 );
	//alpha = exp2( -2 * distance * distance );
	//out_color = texture( diffuseTex, in_.texCoord );
    //out_color = textureLod( diffuseTex, in_.texCoord, 0 );
	//out_color = vec4( abs( in_.position ), 0 );
	//out_color = vec4( abs( in_.normal ), 0 );
	//out_color = vec4( in_.texCoord, 0, 0 );
	//out_color = vec4( 1, 1, 1, 0 );
		alpha = exp2( -2 * distance * distance );
		out_color = wireColor;
		out_color.a *= alpha;
	}
}
