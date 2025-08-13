    #version 330 core
    out vec4 FragColor;
    in vec3 color;

uniform bool isSelected;

    void main()
    {
    vec3 finalColor = isSelected ? vec3(1.0, 0.0, 0.0) : color;
    FragColor = vec4(finalColor, 1.0);
    }