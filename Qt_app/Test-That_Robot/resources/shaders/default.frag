    #version 330 core
    out vec4 FragColor;
    in vec3 color;

uniform bool isSelected;

    void main()
    {
    vec4 finalColor = isSelected ? vec4(color, 1.0) : vec4(color, 1.0);
    FragColor = finalColor;
    }