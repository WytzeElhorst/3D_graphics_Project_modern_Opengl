#version 430
// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;
layout(location = 8) uniform mat4 model;
layout(location = 3) uniform float time;
layout(location = 11) uniform float bullettime;
layout(location = 7) uniform vec2 shiptrans; 

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal
layout(location = 2) in vec3 color; // Color
layout(location = 3) in vec2 UV;
layout(location = 4) in int ID;
layout(location = 5) in float bulnum;
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
out vec2 fragUV;
flat out int fragID;

void main() {
	// Transform 3D position into on-screen position
	vec3 move = pos;
	vec3 nor = normal;
	
	if (ID == 0) {
	move.y += -0.1*time;
	move.z = 0.2*cos(3.1415*pos.x/2)*sin(3.1415*pos.y);
	float dx = -0.31415f*sin(3.1415*pos.x/2)*sin(3.1415*pos.y);
	float dy = 3.1415f*0.2f*cos(3.1415*pos.x/2)*cos(3.1415*pos.y);
	vec3 tx = vec3(move.x, 0, dx);
	vec3 ty = vec3(0, move.y, dy);
	nor.y = cross(tx,ty).y;
	nor.x = cross(tx,ty).x;
	nor = normalize(nor);
	gl_Position = mvp * vec4(move, 1.0);
	}
	if (ID == 1) {
	gl_Position = mvp * vec4(pos.x + shiptrans.x, pos.y + shiptrans.y, pos.z + 0.55f, 1.0f);
	}
	if (ID == 2) {
	vec4 posr = vec4(pos, 1);
	posr.y += 0.5f;
	posr = posr * model;
	gl_Position = mvp * vec4(posr.x + shiptrans.x, posr.y + shiptrans.y, (posr.z/25) + 0.48f, 1.0f);
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
	float off = 0;
	if (bulnum < 8) {
	off = 20;
	} else {
	off = 0;
	}
	gl_Position = mvp * vec4(posr.x + offset.x + 4 * traj.x * (bullettime + off - 5.85f - 0.2721 * bulnum), posr.y + offset.y + 4 * -traj.y * (bullettime + off - 5.85f - 0.2721f * bulnum), (posr.z/25) + 0.48f, 1.0f);
	}
	fragPos = move;
	fragNormal = nor;
	fragColor = color;
	fragUV = UV;
	fragID = ID;

}