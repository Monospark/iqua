#pragma once

#include <QWidget>
#include <QResizeEvent>

class Letterbox : public QWidget {

    Q_OBJECT

public:
    Letterbox(QWidget* widget, float aspectRatio);
    ~Letterbox();

public slots:
    void changeAspectRatio(float newAspectRatio);

private:
    QWidget* widget;
    float aspectRatio;

    void resizeEvent(QResizeEvent*) override;
    void resizeChild(int width, int height) const;
};

