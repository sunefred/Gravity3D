#version 420 core


layout( vertices = 4 ) out;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float tesselationLevel;

in gl_PerVertex {
    vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
    vec4 gl_Position;
} gl_out[];


void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	gl_TessLevelInner[0] = tesselationLevel;
	gl_TessLevelInner[1] = tesselationLevel;
	gl_TessLevelOuter[0] = tesselationLevel;
	gl_TessLevelOuter[1] = tesselationLevel;
	gl_TessLevelOuter[2] = tesselationLevel;
	gl_TessLevelOuter[3] = tesselationLevel;
}