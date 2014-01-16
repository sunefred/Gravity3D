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
	vec3 Attr0;
} out_;
out PerVertexXBF
{
	vec3 Attr0;
	vec3 Attr1;
} xbf_;

//out 	vec3 xbfAttr0;

//layout( xfb_buffer  = 0, xfb_offset = 0, xfb_stride = 12 ) out vec3 xbfAttr0;
	//layout( xfb_buffer  = 0, xfb_offset = 0 ) vec3 xbfAttr0;
	//layout( xfb_buffer  = 2, xfb_offset = 0 ) vec3 xbfAttr2;

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
		xbf_.Attr0 = 100 * in_[i].position;
		xbf_.Attr1 = 100 * in_[i].position;
		//xbfAttr2 = 3 * in_[i].normal;
		//xbfAttr8 = 4 * in_[i].texCoord;
        EmitVertex();
    }
	EndPrimitive();
    
}