//
//  Final Project:  Fishbowl
//  Diana Southard
//  CSCI 4239/5239 Spring 2016
//

#include <QApplication>
#include "includes/projectViewer.h"

//
//  Main function
//
int main(int argc, char *argv[])
{
   //  Create the application
   QApplication app(argc,argv);
   //  Create and show view widget
   ProjectViewer view;
   view.showFullScreen();
   //  Main loop for application
   return app.exec();
}
