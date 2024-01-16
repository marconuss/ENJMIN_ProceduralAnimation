#version 430 core

// You can compil and refresh the shader at runtime using the F7 key

#define BufferAttribVertex 0
#define BufferAttribNormal 1
#define BufferAttribColor 2
#define ShaderType 1


//-- Uniform are variable that are common to all vertices of the drawcall
uniform mat4 Model; // Model matrix
uniform mat4 View;  // View matrix
uniform mat4 Projection; // Projection Matrix
uniform float Time; // Elapsed time since the begining of the program

//-- attrinutes can change for each vertex
layout(location = BufferAttribVertex) in vec3 Position; // Position of current vertex
layout(location = BufferAttribNormal) in vec3 Normal;		// Normal of current vertex
layout(location = BufferAttribColor) in vec4 Color;			// Color of current vertex


//-- Here is the GPU counterpart of the VertexShaderAdditionalData structure
layout(std430, binding= 3) buffer bufferData
{ 
	vec3 center;
	float power;
	float radius;
	float time;
} Data;

out block //define the additional output that will be received by the fragment shader
{
	vec4 Color;
	vec3 CameraSpacePosition;
	vec3 CameraSpaceNormal;
} Out;

void main()
{
	mat4 MV = View * Model;


	#if ShaderType == 0
		// Let's squash

		Out.CameraSpacePosition = vec3(MV * vec4(Position, 1));
		Out.CameraSpaceNormal = vec3(MV * vec4(Normal, 0.0f));
		Out.Color = Color;
		gl_Position = Projection * MV * Position;


	#elif ShaderType == 1
		//Let's bounce

		float distanceFromBounce = length(Position- Data.center);
		float distanceRate =  1 - clamp( distanceFromBounce/Data.radius, 0.0, 1.0);
		vec4 NewPos = vec4(Position, 1);
		NewPos.y += sin(Time) * Data.power * distanceRate;

		Out.CameraSpacePosition = vec3(MV * NewPos);
		Out.CameraSpaceNormal = vec3(MV * vec4(Normal, 0.0f));
		Out.Color = Color;
		gl_Position = Projection * MV * NewPos;


	#else
		//Let's wave

		float XParity = mod(3.*Position.x + Time, 2.0f);
		XParity = step(XParity, 0.2f);
		vec4 NewPos = vec4(Position, 1);
		NewPos.x += Data.center.x;
		NewPos.y += XParity * 0.25 + Data.center.y;
		NewPos.z += Data.center.z;

		Out.CameraSpacePosition = vec3(MV * NewPos);
		Out.CameraSpaceNormal = vec3(MV * vec4(Normal, 0.0f));
		Out.Color = Color;
		Out.Color.r = (sin(Time) + 1.0f)*0.5f;
		//gl_position is always an output and is the resulting vertex pos that will be feeded to fragment shader
		gl_Position = Projection * MV * NewPos;


	#endif
}




