#version 330 core
uniform uint objectID;
out uvec3 fragColor;

void main()
{
    fragColor = uvec3(objectID, 0, 0);
}
