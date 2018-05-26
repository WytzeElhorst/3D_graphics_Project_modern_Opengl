#version 430

// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;
layout(location = 8) uniform mat4 model;
layout(location = 2) uniform float time;
layout(location = 11) uniform float bullettime;
layout(location = 7) uniform vec2 shiptrans; 

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal
layout(location = 2) in vec3 color; // Color
layout(location = 4) in int ID;
layout(location = 5) in int bulnum;
layout(location = 6) in vec2 traj;
layout(location = 8) in vec4 ori0;
layout(location = 9) in vec4 ori1;
layout(location = 10) in vec4 ori2;
layout(location = 11) in vec4 ori3;
layout(location = 7) in vec2 offset;

// Data to pass to fragment shader
out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragColor;

void main() {
	// Transform 3D position into on-screen position

    // Pass position and normal through to fragment shader
	vec3 move = pos;
	if (ID == 0) {
	gl_Position = mvp * vec4(move, 1.0);
	}
	if (ID == 1) {
	gl_Position = mvp * vec4(pos.x + shiptrans.x, pos.y + shiptrans.y, pos.z + 0.55f, 1.0f);
	}
	if (ID == 2) {
	vec4 posr = vec4(pos, 1);
	posr.y += 0.5f;
	posr = posr * model;
	gl_Position = mvp * vec4(posr.x + shiptrans.x, posr.y + shiptrans.y, (posr.z/25) + 0.55f, 1.0f);
	}
	if (ID == 3) {
	mat4 ori = model;
	ori[0] = ori0;
	ori[1] = ori1;
	ori[2] = ori2;
	ori[3] = ori3;
	vec4 posr = vec4(pos, 1);
	posr.y += 0.5f;
	posr = posr * ori;
	gl_Position = mvp * vec4(posr.x + offset.x + 4 * traj.x * (bullettime - 5.6 - 0.272 * bulnum), posr.y + offset.y + 4 * -traj.y * (bullettime - 5.6 - 0.2718f * bulnum), (posr.z/25) + 0.48f, 1.0f);
	}
    fragPos = move;
    fragNormal = normal;
	fragColor = color;
}