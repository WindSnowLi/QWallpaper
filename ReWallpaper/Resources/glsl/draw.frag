#version 330 core
in vec2 point;
uniform sampler2D img;
uniform sampler2D ripple;
out vec4 FragColor;

void main(){
    float x = point.x + texture(ripple, vec2(point.x,point.y-1)).r - texture(ripple, vec2(point.x,point.y+1)).r;
    float y = point.y + texture(ripple, vec2(point.x-1,point.y)).r - texture(ripple, vec2(point.x+1,point.y)).r;
    FragColor = texture(img, vec2(x,y));
};