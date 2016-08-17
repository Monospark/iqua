#pragma once

#include <QMainWindow>
#include "hmd_mirror.h"
#include <memory>
#include "controller.h"

class VreServerDriver;

class Window : public QMainWindow
{
public:
    Window();
    ~Window();

private:
    Controller controller;
    HmdMirror* mirror;

    void waitForServer();

 private slots:
    void leftEye();
    void rightEye();
    void bothEyes();
};

