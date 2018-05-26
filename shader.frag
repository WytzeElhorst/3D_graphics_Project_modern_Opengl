#version 430

// Global variables for lighting calculations
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D texShadow;
layout(location = 6) uniform sampler2D landscape;
layout(location = 9) uniform sampler2D metal;
layout(location = 10) uniform sampler2D snow;
layout(location = 14) uniform sampler2D texbullet;
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
		float spec = pow(dot(fragNormal, h), 1);
		outColor = vec4(textf*col*vec3(max((phong + spec)/2, 0.0f))*(1-shadow*0.7f), 1.0);
	} 

	//Ships with big specularity
	if (fragID == 1 || fragID == 2) {
	vec3 text = vec3(texture(metal, fragUV).rgb);
	float phong = dot(fragNormal, lightDir);
	float spec = pow(dot(fragNormal, h), 5);
	outColor = vec4(text*fragColor*vec3(max(phong + 10*spec, 0.0)), 1.0);
	}
	if (fragID == 3) {
	vec3 text = vec3(texture(texbullet, fragUV).rgb);
	outColor = vec4(text*fragColor, 1.0);
	}
}