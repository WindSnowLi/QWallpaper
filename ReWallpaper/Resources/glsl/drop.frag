#version 330 core
in vec2 point;
uniform sampler2D ripple;

void main(){
    gl_FragColor = texture(ripple, point);
};