#include "window.h"
#include "openvr_system.h"
#include <QApplication>

int main()
{
    OpenVrSystem::init();

    int args = 0;
    QApplication app(args, nullptr);
    Window w;
    w.show();
    return app.exec();
}
