#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// --- ИЗМЕНЕНИЯ ЗДЕСЬ ---
// Убираем старую uniform-переменную и добавляем две новые:
// uniform float outline_thickness; // <-- УДАЛИТЬ
uniform float outline_pixels;     // <-- НОВАЯ: толщина в пикселях (например, 2.0)
uniform vec2  viewport_size;      // <-- НОВАЯ: размер окна (например, 1920, 1080)

void main()
{
    // Преобразуем позицию в clip space (координаты от -1 до 1)
    vec4 clip_pos = projection * view * model * vec4(aPos, 1.0);

    // Определяем направление смещения (нормаль в пространстве обзора)
    vec3 normal_viewspace = normalize(mat3(view * model) * aPos);

    // --- НОВАЯ ЛОГИКА ВЫЧИСЛЕНИЯ СМЕЩЕНИЯ ---
    // Вычисляем, какую долю экрана занимает 1 пиксель
    vec2 pixel_size_clip_space = vec2(2.0 / viewport_size.x, 2.0 / viewport_size.y);
    
    // Вычисляем общее смещение в Clip Space для нужного кол-ва пикселей
    vec2 offset = normalize(normal_viewspace.xy) * outline_pixels * pixel_size_clip_space;

    // Применяем смещение, корректируя его на перспективу (умножение на .w)
    clip_pos.xy += offset * clip_pos.w;

    gl_Position = clip_pos;
}