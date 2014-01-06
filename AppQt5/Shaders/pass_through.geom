#version 420 core


// layout specifier 
layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;


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


// user defined uniforms
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


void main(void)
{
    for ( int i = 0; i < gl_in.length(); i++ )
	{
		gl_Position = projMatrix * viewMatrix * modelMatrix
					  * gl_in[i].gl_Position;
		out_.position = in_[i].position;
		out_.normal = in_[i].normal;
		out_.texCoord = in_[i].texCoord;
        EmitVertex();
    }
	EndPrimitive();
    
}