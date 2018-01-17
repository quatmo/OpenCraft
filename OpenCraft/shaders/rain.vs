#version 330 core

layout (location = 0) in vec3 position;

uniform float time;
uniform float id;

float rand(vec2 co)
{ 
	return abs(fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453)); 
}

void main()
{
	float new_time = time;

	float id = position.z;
	vec2 new_pos = position.xy;

	vec2 accel = vec2(-rand(vec2(id, id)), -rand(vec2(id, id)));
	vec2 vel = vec2(0.1, 0.1) + accel * new_time;
	vel = clamp(vel, -id, -0.001);	
	new_pos += vec2(vel*new_time); 
	new_pos.y = mod(new_pos.y, 2) - 1;
	new_pos.x = mod(new_pos.x, 2) - 1;
	gl_Position = vec4(new_pos, 0, 1);
}