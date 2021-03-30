#version 410

layout(location = 0) in vec3 vertex_position;
uniform vec4 ourPosition;

void main() {
	gl_Position = vec4(vertex_position.x + ourPosition.x , vertex_position.y + ourPosition.y, vertex_position.z,  1.0);
}
