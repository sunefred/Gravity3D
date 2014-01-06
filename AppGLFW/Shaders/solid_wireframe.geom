//
//	Single pass wirefrane rendering:
//	http://www2.imm.dtu.dk/~janba/Wireframe/
//	http://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf
//

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
	vec3 distance;
} out_;


// user defined uniforms
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

//vec2 viewport = vec2( 512, 512 );
//vec2 projToWindow( in vec4 _pos )
//{
//    return vec2(  viewport.x * 0.5 * ( 1 + ( pos.x / pos.w ) ),
//                  Viewport.y * 0.5 * ( 1 - ( pos.y / pos.w ) ) );
//}


void main(void)
{
	// calculate projected positions
	vec4 position[3];
	position[0] = projMatrix * viewMatrix * modelMatrix * gl_in[0].gl_Position;
	position[1] = projMatrix * viewMatrix * modelMatrix * gl_in[1].gl_Position;
	position[2] = projMatrix * viewMatrix * modelMatrix * gl_in[2].gl_Position;


	// calculate position pixel coordinates
	vec2 window[3];
	window[0] = 512 * position[0].xy / position[0].w;
	window[1] = 512 * position[1].xy / position[1].w;
	window[2] = 512 * position[2].xy / position[2].w;


	// calculate triangle edges
	vec2 edges[3];
    edges[0] = window[1] - window[0];
    edges[1] = window[2] - window[1];
    edges[2] = window[0] - window[2];


    // length of edges
    float lengths[3];
    lengths[0] = length( edges[0] );
    lengths[1] = length( edges[1] );
    lengths[2] = length( edges[2] );


	// triangle area is the length of the cross product of any two edges
	// divided by two (assuming z component is 0)
	float triangleArea = abs( edges[1].x * edges[2].y -
							  edges[1].y * edges[2].x ) / 2;


    // The distance to opposite edge for each vertex
    float heights[3];
    heights[0] = triangleArea / lengths[1];
	heights[1] = triangleArea / lengths[2];
    heights[2] = triangleArea / lengths[0];


	// emit the primitive (triangle)
	gl_Position = position[0];
	out_.distance = vec3( 0, heights[0], 0 );
	EmitVertex();
	gl_Position = position[1];
	out_.distance = vec3( 0, 0, heights[1] );
	EmitVertex();
	gl_Position = position[2];
	out_.distance = vec3( heights[2], 0, 0 );
	EmitVertex();
	EndPrimitive();
    
}