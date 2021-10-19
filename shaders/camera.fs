#version 410 core

in vec3 VertexColor;

out vec4 FragColor;

void main() { 
    FragColor = vec4((1.f + VertexColor.x) / 2, (1.f + VertexColor.y) / 2, (1.f + VertexColor.z) / 2.f, 1.0f);
} 
