#version 330 core
in vec2 point;
uniform sampler2D img;

void main(){
    gl_FragColor = texture(img, point);
};