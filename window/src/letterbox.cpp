#include "letterbox.h"

Letterbox::Letterbox(QWidget* widget, float aspectRatio) : widget(widget), aspectRatio(aspectRatio)
{
    widget->setParent(this);
    setPalette(QPalette(Qt::blue));
}

Letterbox::~Letterbox() {
}


void Letterbox::resizeEvent(QResizeEvent* evt)
{
    resizeChild(evt->size().width(), evt->size().height());
}

void Letterbox::changeAspectRatio(float newAspectRatio)
{
    aspectRatio = newAspectRatio;
    resizeChild(geometry().width(), geometry().height());
}

void Letterbox::resizeChild(int width, int height) const
{
    float letterboxAspect = static_cast<float>(width) / static_cast<float>(height);

    int newWidth = width;
    int newHeight = height;
    if (letterboxAspect > aspectRatio) {
        //Widget size is limited by height.
        newWidth = height * aspectRatio;
    } else {
        //Widget size is limited by width.
        newHeight = width / aspectRatio;
    }

    //Center widget
    int offsetX = (width - newWidth) / 2;
    int offsetY = (height - newHeight) / 2;
    widget->move(offsetX, offsetY);
    widget->resize(newWidth, newHeight);
}
