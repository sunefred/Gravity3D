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

out gl_PerVertex
{
	vec4 gl_Position;
};


void main(void)
{
    for ( int i = 0; i < gl_in.length(); i++ )
	{
		gl_Position = projMatrix * viewMatrix * modelMatrix
					  * gl_in[i].gl_Position;
        EmitVertex();
    }
	EndPrimitive();
    
}