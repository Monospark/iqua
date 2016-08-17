#include "window.h"

#include "hmd_mirror.h"
#include <QMenuBar>
#include "letterbox.h"
#include <QApplication>
#include <memory>
#include <future>
#include <vre_server_driver.h>

Window::Window() : QMainWindow(nullptr)
{
    waitForServer();
    mirror = new HmdMirror(controller);
    QWidget* letterbox = new Letterbox(mirror, mirror->getAspectRatio());
    letterbox->setMinimumSize(500, 500);
    setCentralWidget(letterbox);

    QAction* leftEye = new QAction("Left Eye", this);
    leftEye->setCheckable(true);
    connect(leftEye, &QAction::triggered, this, &Window::leftEye);
    QAction* rightEye = new QAction("Right Eye", this);
    rightEye->setCheckable(true);
    QAction* bothEyes = new QAction("Both Eyes", this);
    bothEyes->setCheckable(true);
    QActionGroup* viewGroup = new QActionGroup(this);
    viewGroup->addAction(leftEye);
    viewGroup->addAction(rightEye);
    viewGroup->addAction(bothEyes);
    leftEye->setChecked(true);
    QMenu* viewMenu = new QMenu("View");
    viewMenu->addAction(leftEye);
    viewMenu->addAction(rightEye);
    viewMenu->addAction(bothEyes);
    QMenuBar* menuBar = new QMenuBar();
    menuBar->addMenu(viewMenu);
    setMenuBar(menuBar);

    setWindowTitle("OpenVR emulator");
}

Window::~Window() {
}


void Window::waitForServer()
{
    int waitedMs = 0;
    while (VreServerDriver::getInstance().get() == nullptr)
    {
        if (waitedMs > 5000)
        {
            this->close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waitedMs += 100;
    }
}

void Window::leftEye()
{
    mirror->setEyeView(HmdMirror::EyeView::LEFT);
}

void Window::rightEye()
{
    mirror->setEyeView(HmdMirror::EyeView::RIGHT);
}

void Window::bothEyes()
{
    mirror->setEyeView(HmdMirror::EyeView::BOTH);
}
