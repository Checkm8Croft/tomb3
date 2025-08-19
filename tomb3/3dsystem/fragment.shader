#version 330 core
in vec4 vColor;
in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D uTexture;

void main() {
    fragColor = vColor * texture(uTexture, vTexCoord);
}