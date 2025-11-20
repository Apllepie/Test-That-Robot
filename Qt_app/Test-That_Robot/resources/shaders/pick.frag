#version 330 core
uniform uint objectID;
uniform uint handleID;
out uvec3 fragColor;

void main()
{
    fragColor = uvec3(objectID, handleID, 0);
}
