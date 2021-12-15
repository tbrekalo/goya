#version 410 core

layout (location = 0) in vec3 aVert;
layout (location = 1) in vec3 aCent;
layout (location = 2) in vec4 aColor;

out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 systemScale;

void main(){
  
  mat4 centerShift = mat4(1.f);

  centerShift[3][0] = aCent[0];
  centerShift[3][1] = aCent[1];
  centerShift[3][2] = aCent[2];

  mat4 particleCamerView = view * centerShift; 

  particleCamerView[0][0] = 1;
	particleCamerView[1][1] = 1;
	particleCamerView[2][2] = 1;

	particleCamerView[0][1] = 0;
	particleCamerView[0][2] = 0;

	particleCamerView[1][0] = 0;
	particleCamerView[1][2] = 0;

	particleCamerView[2][0] = 0;
	particleCamerView[2][1] = 0;

	gl_Position = projection * 
                particleCamerView * 
                systemScale * 
                vec4(aVert, 1.f);

  ParticleColor = aColor;
}
