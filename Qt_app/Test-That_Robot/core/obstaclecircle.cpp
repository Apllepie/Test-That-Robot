#include "obstaclecircle.h"

ObstacleCircle::ObstacleCircle() {}

ObstacleCircle::ObstacleCircle(float radius, int nSides, Mesh*mesh):
    _radius(radius), _nSides(nSides)
{
    Object::_mesh = mesh;

}

bool ObstacleCircle::isInside(QVector2D localPoint) const
{
    // 1. Быстрая оптимизация: Bounding Circle
    // Если точка дальше радиуса, она точно снаружи любого вписанного многоугольника.
    // (localPoint уже в локальных координатах, где масштаб 1.0 = реальный размер,
    //  потому что Scale применяется к матрице модели, а localPoint получен через её инверсию).
    //
    // НО: Если мы использовали Scale матрицы для задания радиуса (как обсуждали выше),
    // то локальная система координат всегда имеет радиус 0.5.
    // Давайте исходить из того, что localPoint находится в пространстве "Единичного круга" (радиус 0.5).

    float rSq = localPoint.lengthSquared();
    // Радиус единичного меша = 0.5
    if (rSq > 0.5f * 0.5f) {
        return false;
    }

    // Если граней очень много (например, > 16), считаем это просто кругом
    if (_nSides > 16) {
        return true;
    }

    // 2. Точная математика для многоугольника (Triangle, Hexagon...)

    float x = localPoint.x();
    float y = localPoint.y();

    // Вычисляем угол точки в полярных координатах (-PI ... +PI)
    float angle = std::atan2(y, x);

    // Нормализуем угол в диапазон [0, 2*PI], чтобы было удобно работать
    if (angle < 0) angle += 2.0f * M_PI;

    // Угловой размер одного сектора (одной грани)
    float sectorStep = 2.0f * M_PI / _nSides;

    // Находим угол внутри конкретного сектора (от 0 до sectorStep)
    // По сути мы "вращаем" точку так, будто проверяем всегда одну и ту же грань
    float angleInSector = std::fmod(angle, sectorStep);

    // Находим отклонение от центра грани (биссектрисы сектора).
    // Центр грани находится на sectorStep / 2.
    // Получаем угол delta в диапазоне [-sectorStep/2 ... +sectorStep/2]
    float delta = angleInSector - (sectorStep / 2.0f);

    // Апофема - расстояние от центра (0,0) до середины грани.
    // Для нашего "единичного" меша радиус R = 0.5.
    // Формула апофемы: a = R * cos(PI / n)
    float apothem = 0.5f * std::cos(M_PI / _nSides);

    // Проекция расстояния точки на ось апофемы.
    // dist * cos(delta) должна быть меньше апофемы.
    float dist = std::sqrt(rSq);

    return (dist * std::cos(delta)) <= apothem;
}
