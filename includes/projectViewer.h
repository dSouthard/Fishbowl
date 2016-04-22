#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include <QWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include "includes/projectOpengl.h"

class ProjectViewer : public QWidget
{
Q_OBJECT
private:
   QSlider* fishSlider;
   ProjectOpengl* ogl;
private slots:
public:
    ProjectViewer();
};

#endif
