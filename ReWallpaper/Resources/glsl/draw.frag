#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D ripple;
uniform sampler2D background;
uniform sampler2D img;
void main(){
    FragColor = texture(img, TexCoord);
};