#version 330 core
in vec2 point;
uniform sampler2D currentRipple;
uniform sampler2D rippleBuff;

void main(){
    float avg = (
        texture(currentRipple, vec2(point.x-1,point.y)).r+
        texture(currentRipple, vec2(point.x,point.y+1)).r+
        texture(currentRipple, vec2(point.x+1,point.y)).r+
        texture(currentRipple, vec2(point.x-1,point.y-1)).r
    ) / 2 - texture(rippleBuff, point).r;
    float mid = avg - avg/32;

    gl_FragColor = vec4(0.5,0,0,0);
};