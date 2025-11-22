#ifndef ROBOT_H
#define ROBOT_H


#include <cerrno>
#include <QMatrix4x4>
#include <object.h>
#include <qmath.h>

#include "occupancy_grid.h"


struct RobotPos{
    float x;
    float y;
    float theta; //angle radians

};

class Robot: public Object
{

private: 
    float _theta;
    float _linearspeed;
    float _angularspeed;
    QVector3D _velocity;

    QVector3D _destination; //  destination point
    bool _hasDestination = false;
    std::vector<QVector2D> _path; // Stores the path as a sequence of points
    int _currentPathIndex = -1;   // Index of the current target point in the path
    size_t _destinationMarkerId = 0;

    std::vector<QVector3D> _travelTrace;
    QVector3D _lastTracePos;
    float _traceMinDist = 0.05f;

    const OccupancyGrid* _grid = nullptr;
    //functions
    float calculateDistance(float nx, float ny);
    QVector3D normalizeVector();
    float calculateAngle();

public:
    Robot();
    Robot(Mesh *mesh);

    void update(float dt) override;
    void start() override;
    void stop() override;

    void setPath(const std::vector<QVector2D>& path);
    void setDestination(const QVector3D& dest);
    void setGrid(const OccupancyGrid* grid);

    RobotPos getRobotPos() const;
    void setVelocity(const QVector2D& vel) { _velocity = QVector3D(vel,0.0f); }
    bool hasDestination() const { return _hasDestination; }
    QVector3D getDestination() const { return _destination; }
    
    void setDestinationMarkerId(size_t marker_id) { _destinationMarkerId = marker_id; }
    size_t getDestinationMarkerId() const { return _destinationMarkerId; }
    QString getType() const override { return "robot"; }
     const std::vector<QVector3D>& getTravelTrace() const { return _travelTrace; }
    
    // Метод очистки, если нужно сбросить след
    void clearTrace() { _travelTrace.clear(); }


    float omega; //angular speed

};
#endif // ROBOT_H
