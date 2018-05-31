#version 430

// Global variables for lighting calculations
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D texShadow;
layout(location = 6) uniform sampler2D landscape;
layout(location = 9) uniform sampler2D metal;
layout(location = 10) uniform sampler2D snow;
layout(location = 14) uniform sampler2D texbullet;
layout(location = 15) uniform sampler2D ship;
layout(location = 16) uniform vec4 enemydata0;
layout(location = 17) uniform vec4 enemydata1;
layout(location = 18) uniform vec4 enemydata2;
layout(location = 19) uniform vec4 enemydata3;
layout(location = 20) uniform vec4 enemydata4;
layout(location = 21) uniform float hp;
layout(location = 22) uniform sampler2D texToon;
layout(location = 3) uniform float time;

layout(location = 4) uniform mat4 lightMVP;
layout(location = 5) uniform vec3 lightPos;

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos;    // World-space position
in vec3 fragNormal; // World-space normal
in vec3 fragColor;
in vec2 fragUV;
flat in int fragID;
flat in int fragNum;

void main() {
	// Output the normal as color
	int surround = 1;
	vec4 fragLightCoord = lightMVP * vec4(fragPos, 1.0);
	fragLightCoord.xyz /= fragLightCoord.w;
	fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5;
	float fragLightDepth = fragLightCoord.z;
	vec2 shadowMapCoord = fragLightCoord.xy;
	float shadowMapDepth = texture(texShadow, shadowMapCoord.xy).x;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 h = normalize(lightDir + viewDir);

	//Landscape with shadows
	if (fragID == 0) {
		float shadow = 0.0;
		vec2 texelSize = 1.0 / textureSize(texShadow, 0);
		for(int x = -surround; x <= surround; ++x) {
			for(int y = -surround; y <= surround; ++y) {
				float pcfDepth = texture(texShadow, shadowMapCoord.xy + vec2(x, y) * texelSize).r; 
				shadow += fragLightDepth - 0.0005 > pcfDepth ? 1.0 : 0.0;        
			}    
		}
		shadow /= (2*surround+1)*(2*surround+1);
		vec3 texts = vec3(texture(snow, fragUV).rgb); 
		vec3 textg = vec3(texture(landscape, fragUV).rgb);
		vec3 textf = ((2*fragPos.z + 0.50) * texts) + ((0.50 - 2*fragPos.z)* textg);
		vec3 col = fragColor;
		col.x = 1.8*fragPos.z + 0.25f;
		col.z = 1.8*fragPos.z + 0.25f;
		float phong = dot(fragNormal, lightDir);
		float spec = pow(dot(fragNormal, h), 2);
		outColor = vec4(textf*col*vec3(max((phong + spec)/2, 0.0f))*(1-shadow*0.4f), 1.0);
	} 

	//Ships with big specularity
	if (fragID == 1) {
	vec3 text = vec3(texture(ship, fragUV).rgb);
	float phong = dot(fragNormal, lightDir);
	float spec = pow(dot(fragNormal, h), 15);
	outColor = vec4(text*fragColor*vec3(max(phong + 4*spec, 0.0) + 0.2f), 1.0);
	}
	if (fragID == 2) {
	vec3 text = vec3(texture(metal, fragUV).rgb);
	float phong = dot(fragNormal, lightDir);
	float spec = pow(dot(fragNormal, h), 15);
	outColor = vec4(text*fragColor*vec3(max(phong + 4*spec, 0.0) + 0.2f), 1.0);
	}
	if (fragID == 3) {
	vec3 text = vec3(texture(texbullet, fragUV).rgb);
	outColor = vec4(text*fragColor, 1.0);
	}
	if (fragID == 6) {
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 h = normalize(lightDir + viewDir);
	float diff = dot(fragNormal, lightDir);
	float spec = pow(dot(normalize(fragNormal), h), 15);
	if (spec > 0.2) {
	spec = 1.0;
	} else {
	spec = 0.0;}
	float res = (diff + spec)/2;
	if (res < 0.25) {
	res = 0;
	} else { 
		if (res < 0.5) {
		res = 0.25;
		} else {
		if (res < 0.75) {
			res = 0.5;
			} else {
				if (res < 1) {
				res = 0.75;
				}
			}
		}
	}
	float dist = distance(viewPos, fragPos);
    outColor = texture(texToon, vec2 ( diff , ((normalize(viewPos - fragPos)))));
	}
	if (fragID == 5) {
	vec3 col = fragColor;
	if (fragNum == 0) {
	col.y *= enemydata0.z / hp; //sorry for bad code ethics
	col.z *= enemydata0.z / hp;
	}
	if (fragNum == 1) {
	col.y *= enemydata1.z / hp;
	col.z *= enemydata1.z / hp;
	}
	if (fragNum == 2) {
	col.y *= enemydata2.z / hp;
	col.z *= enemydata2.z / hp;
	}
	if (fragNum == 3) {
	col.y *= enemydata3.z / hp;
	col.z *= enemydata3.z / hp;
	}
	if (fragNum == 4) {
	col.y *= enemydata4.z / hp;
	col.z *= enemydata4.z / hp;
	}
	vec3 text = vec3(texture(metal, fragUV).rgb);
	float phong = dot(fragNormal, lightDir);
	float spec = pow(dot(fragNormal, h), 15);
	outColor = vec4(text*col*vec3(max(phong + 4*spec, 0.0) + 0.2f), 1.0);
	}
}