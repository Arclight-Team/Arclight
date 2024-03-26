#include "rendercamera.h"

#include <QtMath>
#include <QtGlobal>
#include <QVector2D>


RenderCamera::RenderCamera() : RenderCamera(QVector3D(1, 0, 0)) {}

RenderCamera::RenderCamera(const QVector3D& pos) : RenderCamera(pos, QVector3D(0, 0, 0)) {};

RenderCamera::RenderCamera(const QVector3D& pos, const QVector3D& ctr) {
    setPosition(pos);
    setCenter(ctr);
}

RenderCamera::RenderCamera(const QVector3D& pos, double yaw, double pitch) {
    setPosition(pos);
    setRotation(yaw, pitch);
};



void RenderCamera::zoom(int steps, double factor, double minDist, double maxDist) {

    QVector3D front = center - position;
    double d = front.length() * qPow(factor, -steps);

    d = qMin(maxDist, qMax(minDist, d));
    position = center - front.normalized() * d;

}



void RenderCamera::pan(const QVector2D& pan, double factor) {

    QVector3D front = center - position;
    QVector3D right = QVector3D::crossProduct(front, QVector3D(0, 1, 0)).normalized();
    QVector3D up = QVector3D::crossProduct(right, front).normalized();
    double d = front.length() / factor;

    QVector3D offset = right * pan.x() * d + up * pan.y() * d;
    position += offset;
    center += offset;

}



void RenderCamera::rotate(double yaw, double pitch) {

    QVector3D direction = position - center;
    double r = direction.length();
    double ry = direction.y();
    double currentYaw = qAtan2(direction.z(), direction.x());
    double currentPitch = qAsin(ry / r);

    currentYaw += yaw;
    currentPitch += pitch;
    currentPitch = qMax(qDegreesToRadians(-maxPitch), qMin(qDegreesToRadians(maxPitch), currentPitch));

    position = r * QVector3D(qCos(currentYaw) * qCos(currentPitch), qSin(currentPitch), qSin(currentYaw) * qCos(currentPitch)) + center;

}



void RenderCamera::setPosition(const QVector3D& pos) {

    position = pos;

    if(qFuzzyCompare(center.x(), position.x()) && qFuzzyCompare(center.z(), position.z())){
        position.setX(bias);
    }

}



void RenderCamera::setCenter(const QVector3D& ctr) {

    center = ctr;

    if(qFuzzyCompare(center.x(), position.x()) && qFuzzyCompare(center.z(), position.z())){
        center.setX(bias);
    }

}



void RenderCamera::setRotation(double yaw, double pitch) {

    double r = QVector3D(position - center).length();
    pitch = qMax(qDegreesToRadians(-maxPitch), qMin(qDegreesToRadians(maxPitch), pitch));
    position = r * QVector3D(qCos(yaw) * qCos(pitch), qSin(pitch), qSin(yaw) * qCos(pitch)) + center;

}



QVector3D RenderCamera::getPosition() const {
    return position;
}



QVector3D RenderCamera::getCenter() const {
    return center;
}
