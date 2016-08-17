#pragma once

#include <QTimer>
#include <QElapsedTimer>
#include <pose.h>
#include <cstdint>

class VreServerDriver;

class Controller : public QObject {

    Q_OBJECT

public:
    Controller();
    ~Controller();

    void moveMouse(int32_t x, int32_t y);

private:
    QElapsedTimer updateTimer;
    QTimer updater;

    void updateInput();
    void updatePose() const;

    Pose pose;
    float rotationX;
    float rotationY;
};

