#version 410 core

layout (location = 0) in vec3 aVert;
layout (location = 1) in vec3 aCent;
layout (location = 2) in vec4 aColor;

out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main(){
  
  mat4 model = mat4(1.f);
  model[3][0] = aCent[0];
  model[3][1] = aCent[1];
  model[3][2] = aCent[2];

  ParticleColor = aColor;

	gl_Position = projection * view * model * vec4(aVert, 1.f);
}
