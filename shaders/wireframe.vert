#version 330 core
layout (location = 0) in vec3 position;   // the position variable has attribute position 0
layout (location = 4) in mat4 model;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
}
