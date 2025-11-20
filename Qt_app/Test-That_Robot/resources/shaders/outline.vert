// File: resources/shaders/outline.vert
#version 330 core
layout (location = 0) in vec3 aPos;

// Нам больше не нужна нормаль здесь, т.к. мы будем использовать другой трюк
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Новая uniform-переменная для толщины обводки в единицах экрана
uniform float outline_thickness;

void main()
{
    // Преобразуем позицию в clip space (координаты экрана)
    vec4 clip_pos = projection * view * model * vec4(aPos, 1.0);

    // Получаем нормаль в пространстве камеры (view space)
    // Это нужно, чтобы правильно определить направление "наружу" от объекта
    vec3 normal_viewspace = normalize(mat3(view * model) * aPos); // Для простых фигур, как куб, можно использовать aPos как нормаль

    // Сдвигаем вершину в экранном пространстве
    // Умножение на .w компенсирует перспективу, делая толщину постоянной
    clip_pos.xy += normalize(normal_viewspace.xy) * outline_thickness * clip_pos.w;

    gl_Position = clip_pos;
}