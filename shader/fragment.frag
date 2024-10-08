#version 320 es
precision mediump float;

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 fragcolor;

void main() {
    FragColor = vec4(fragcolor, 1.0);
}