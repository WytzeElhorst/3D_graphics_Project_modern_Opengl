#version 430

// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;
layout(location = 8) uniform mat4 model;
layout(location = 2) uniform float time;
layout(location = 12) uniform vec4 bulletmult;
layout(location = 13) uniform vec4 bulletmult2;
layout(location = 16) uniform vec4 enemydata0;
layout(location = 17) uniform vec4 enemydata1;
layout(location = 18) uniform vec4 enemydata2;
layout(location = 19) uniform vec4 enemydata3;
layout(location = 20) uniform vec4 enemydata4;
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
	vec3 nor = normal;
	if (ID == 0) {
	gl_Position = mvp * vec4(move, 1.0);
	}
	if (ID == 1) {
	move.x += shiptrans.x;
	move.y += shiptrans.y;
	move.z += 0.65f;
	gl_Position = mvp * vec4(move, 1.0f);
	}
	if (ID == 2) {
	vec4 posr = vec4(pos, 1);
	posr.y += 0.5f;
	posr = posr * model;
	vec4 norr = vec4(pos, 1);
	nor = (norr * model).xyz;
	move.x = posr.x + shiptrans.x;
	move.y = posr.y + shiptrans.y;
	move.z = (posr.z/25) + 0.58f;
	gl_Position = mvp * vec4(move, 1.0f);
	}
	if (ID == 3) {
		//disable bullet shadow
		gl_Position = mvp * vec4(0, 0, -10, 1.0);
		//enable bullet shadow
		//mat4 ori = model;
		//ori[0] = ori0;
		//ori[1] = ori1;
		//ori[2] = ori2;
		//ori[3] = ori3;
		//vec4 posr = vec4(pos, 1);
		//posr.y += 0.5f;
		//posr = posr * ori;
		//float off = 0;
		//if (bulnum < 8) {
		//	off = 20;
		//} else {
		//	off = 0;
		//}
		//float mult = 0.1f;
		//if (bulnum < 4) {
		//	mult = bulletmult[bulnum];
		//	}
		//if (bulnum >= 4) {
		//	mult = bulletmult2[bulnum - 4];
		//}
		//gl_Position = mvp * vec4(posr.x + offset.x + traj.x * 0.4 *mult, posr.y + offset.y + -traj.y * 0.4 *mult, (posr.z/25) + 0.58f, 1.0f);

	}
	if (ID == 5) {
		if (bulnum == 0) {
			move.x += enemydata0.x;
			move.y += enemydata0.y;
		}
		if (bulnum == 1) {
			move.x += enemydata1.x;
			move.y += enemydata1.y;
		}
		if (bulnum == 2) {
			move.x += enemydata2.x;
			move.y += enemydata2.y;
		}
		if (bulnum == 3) {
			move.x += enemydata3.x;
			move.y += enemydata3.y;
		}
		if (bulnum == 4) {
			move.x += enemydata4.x;
			move.y += enemydata4.y;
		}
		move.z += 0.58f;
		gl_Position = mvp * vec4(move, 1.0f);
	}
    fragPos = move;
    fragNormal = normal;
	fragColor = color;
}