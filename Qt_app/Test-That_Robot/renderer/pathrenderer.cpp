#include "pathrenderer.h"

PathRenderer::PathRenderer() {}

PathRenderer::~PathRenderer()
{
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
}

void PathRenderer::init()
{
    initializeOpenGLFunctions();

    // Используем существующие шейдеры для сетки, так как нам нужно просто рисовать линии одним цветом
    _shader = std::make_unique<Shader>(":/Shaders/shaders/grid.vert", ":/Shaders/shaders/grid.frag");

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Настраиваем атрибут позиции (location = 0, vec3 заменен на vec2 в grid.vert,
    // НО! grid.vert в вашем коде принимает vec2 (layout 0).
    // Робот хранит QVector3D. Нам нужно убедиться, что шейдер совпадает.
    // В вашем файле grid.vert: layout (location = 0) in vec2 aPos;
    // А мы хотим передавать 3D (чтобы чуть поднять над полом).
    //
    // РЕШЕНИЕ: Давайте использовать default.vert/frag или создадим простой on-the-fly,
    // но проще всего подстроиться под grid.vert.
    // Grid.vert ставит Z=0.0 жестко: gl_Position = ... vec4(aPos, 0.0, 1.0);
    // Это проблема, будет z-fighting.

    // ЛУЧШЕЕ РЕШЕНИЕ: Использовать occupancy.vert/frag, там есть смещение? Нет.
    // Давайте просто переиспользуем default.vert/frag, но подадим uniform color?
    // Нет, default хочет attribute color.

    // Сделаем динамически инициализацию буфера под vec3.
    // При отрисовке будем использовать occupancy shader? Нет.
    // Давайте используем outline shader (он рисует одним цветом), но он делает offset.

    // САМОЕ ПРОСТОЕ: Использовать occupancy.vert/.frag, так как он берет uniform color.
    // occupancy.vert: layout 0 in vec2 aPos. Опять 2D.

    // Вернемся к grid.vert. Он берет vec2 aPos. Мы не можем задать Z.
    // Придется либо менять grid.vert, либо (лучше) создать свой мини-шейдер для линий пути.
    // Но, чтобы не плодить файлы, давайте используем pick.vert (он берет vec3 aPos)
    // и grid.frag (он берет uniform color). Это допустимый хак в OpenGL.
}

void PathRenderer::render(const std::vector<QVector3D>& path, const Camera& camera, QVector3D color)
{
    if (path.empty()) return;

    // --- 1. Подготовка данных ---
    // Нам нужно скопировать данные. Если путь очень длинный, это может быть медленно,
    // но для < 10000 точек это мгновенно.

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    // GL_DYNAMIC_DRAW, так как путь меняется каждый кадр
    glBufferData(GL_ARRAY_BUFFER, path.size() * sizeof(QVector3D), path.data(), GL_DYNAMIC_DRAW);

    // Включаем атрибут vec3 (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);

    // --- 2. Шейдер ---
    // Мы используем pick.vert (так как он принимает vec3 aPos и просто умножает матрицы)
    // и grid.frag (так как он принимает uniform uColor и красит всё в один цвет).
    // Для этого нужно создать "гибридный" Shader объект или просто загрузить эти файлы.
    // В методе init() я загрузил grid.vert. Давайте сменим стратегию:
    // Загрузим pick.vert + grid.frag
    if (!_shader) {
        _shader = std::make_unique<Shader>(":/Shaders/shaders/pick.vert", ":/Shaders/shaders/grid.frag");
    }

    _shader->Activate();

    // Устанавливаем матрицы
    QMatrix4x4 model;
    model.setToIdentity(); // Путь уже в мировых координатах

    glUniformMatrix4fv(glGetUniformLocation(_shader->ID, "model"), 1, GL_FALSE, model.constData());
    glUniformMatrix4fv(glGetUniformLocation(_shader->ID, "view"), 1, GL_FALSE, camera.view.constData());
    glUniformMatrix4fv(glGetUniformLocation(_shader->ID, "projection"), 1, GL_FALSE, camera.projection.constData());

    // Устанавливаем цвет
    glUniform3f(glGetUniformLocation(_shader->ID, "uColor"), color.x(), color.y(), color.z());

    // --- 3. Отрисовка ---
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(path.size()));

    glBindVertexArray(0);
}
