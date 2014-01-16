#version 420 core


// latout specifier
//
// Specifies the number of vertices for patch primitive
// I don't currently see how this could differ from the input patch size
//
layout( vertices = 4 ) out;


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
} gl_out[];
out PerVertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} out_[];


// user defined uniforms
uniform float tesselationLevel;


// application entry point
void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	out_[gl_InvocationID].position = in_[gl_InvocationID].position;
	out_[gl_InvocationID].normal = in_[gl_InvocationID].normal;
	out_[gl_InvocationID].texCoord = in_[gl_InvocationID].texCoord;

	
	// main is invoced once for each patch vertex. Althrough there is not 
	// reason to set the patch vertices more than once I assume it makes no 
	// difference as shader calculations are done in batches where a batch is
	// first finished when ALL vertices in that batch has finished. 
	if ( gl_InvocationID == 0 )
	{
		gl_TessLevelInner[0] = tesselationLevel;
		gl_TessLevelInner[1] = tesselationLevel;
		gl_TessLevelOuter[0] = tesselationLevel;
		gl_TessLevelOuter[1] = tesselationLevel;
		gl_TessLevelOuter[2] = tesselationLevel;
		gl_TessLevelOuter[3] = tesselationLevel;
	}
}