//
//  ProjectViewer Widget
//

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include "includes/projectViewer.h"

//
//  Constructor
//
ProjectViewer::ProjectViewer()
{
   //  Set window title
   setWindowTitle(tr("Final Project: Fishbowl [D. Southard]"));

   //  Create new Triangle widget
   ogl = new ProjectOpengl;

   //  Number of fish
   fishSlider = new QSlider(Qt::Vertical);
   fishSlider->setMinimum(0);
   fishSlider->setMaximum(100);
   fishSlider->setSliderPosition(10);
   fishSlider->setSingleStep(1);

   //  Quit
   QPushButton* quit = new QPushButton("Quit");

   //  Set layout of child widgets
   QGridLayout* layout = new QGridLayout;
   layout->addWidget(ogl,0,0,3,1);
   layout->addWidget(new QLabel("Add Fish"),0,1);
   layout->addWidget(fishSlider,0,2);
   layout->addWidget(quit,2,2);
   //  Manage resizing
   layout->setColumnStretch(0,25);
   layout->setColumnMinimumWidth(0,25);
   layout->setRowStretch(3,25);
   setLayout(layout);

   //  Connect valueChanged() signals to ogl
   connect(fishSlider, SIGNAL(valueChanged(int)), ogl,SLOT(setNumberOfFish(int)));
   //  Connect quit() signal to qApp::quit()
   connect(quit,SIGNAL(pressed()) , qApp,SLOT(quit()));
}
