//
//  Final project: Lumpy
//  Jacob Bloom
//  CSCI 4239/5239 Spring 2019
//

#include <QApplication>
#include "viewer.h"

//
//  Main function
//
int main(int argc, char *argv[])
{
   //  Create the application
   QApplication app(argc,argv);
   //  Create and show view widget
   viewer view;
   view.show();
   //  Main loop for application
   return app.exec();
}
