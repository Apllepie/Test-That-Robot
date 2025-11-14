#version 330 core
out vec4 FragColor;

//uniform vec3 outlineColor; //

void main()
{
    vec3 outlineColor = vec3(0.0, 0.0, 0.0); // Черный цвет для контура
    FragColor = vec4(outlineColor, 1.0);
}