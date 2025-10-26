#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// uniform sampler2D screenTexture; // <-- БЫЛО
uniform usampler2D screenTexture;  // <-- СТАЛО (usampler2D)

void main()
{
  // Читаем целочисленные данные
    uvec4 texData = texture(screenTexture, TexCoords);

    // Преобразуем ID в видимый цвет для отладки.
    // Делим на какое-то число, чтобы ID=1, 2, 3 были видны как градации серого/красного.
    FragColor = vec4(vec3(float(texData.r) / 10.0), 1.0);
}