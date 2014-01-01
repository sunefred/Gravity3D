#version 420 core


layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;





in gl_PerVertex
{
    vec4 gl_Position;
} gl_in[];

in vec3 vertPosition[];
in vec3 vertNormal;
in vec2 vertTexCoord;


out gl_PerVertex
{
	vec4 gl_Position;
};

out vec3 geomPosition[];
out vec3 geomNormal;
out vec2 geomTexCoord;


void main(void)
{
    for ( int i = 0; i < gl_in.length(); i++ )
	{
		gl_Position = projMatrix * viewMatrix * modelMatrix
					  * gl_in[i].gl_Position;
		geomPosition[i] = vertPosition[i];
        EmitVertex();
    }
	EndPrimitive();
    
}