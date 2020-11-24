#ifndef RENDERCAMERA_H
#define RENDERCAMERA_H

#include <QVector3D>


class RenderCamera {

public:

    RenderCamera();
    explicit RenderCamera(const QVector3D& pos);
    RenderCamera(const QVector3D& pos, const QVector3D& ctr);
    RenderCamera(const QVector3D& pos, double angleH, double angleV);

    void zoom(int steps, double factor, double minDist, double maxDist);
    void pan(const QVector2D& pan, double factor);
    void rotate(double yaw, double pitch);

    void setPosition(const QVector3D& pos);
    void setCenter(const QVector3D& ctr);
    void setRotation(double yaw, double pitch);

    QVector3D getPosition() const;
    QVector3D getCenter() const;

private:

    QVector3D position;
    QVector3D center;

    constexpr static double bias = 0.0001;
    constexpr static double maxPitch = 89.999;

};

#endif // RENDERCAMERA_H
