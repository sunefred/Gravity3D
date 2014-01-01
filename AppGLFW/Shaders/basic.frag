#version 420 core

layout( binding = 0 ) uniform sampler2D diffuseTex;

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec4 FragColor;


void main()
{
	FragColor = texture( diffuseTex, vertTexCoord );
	//FragColor = pow( texture( diffuseTex, vertTexCoord ), vec4( 1/2.2, 1/2.2, 1/2.2, 1 ) );
    //FragColor = textureLod( diffuseTex, vertTexCoord, 0 );
	//FragColor = vec4( vertTexCoord, 0, 0 );
	//FragColor = vec4( abs( vertNormal ), 0 );
	//FragColor = vec4( 1, 1, 1, 0 );
}
