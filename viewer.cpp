//
//  viewer Widget
//

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include "viewer.h"

//
//  Constructor
//
viewer::viewer()
{
   //  Set window title
   setWindowTitle(tr("Lumpy"));

   //  Create new OpenGL widget
   ogl = new opengl;

   //  First Person
   QPushButton* toggleFirstPerson = new QPushButton("First Person");
   toggleFirstPerson->setCheckable(true);

   //  Resolution (precision) dropdown
   res = new QComboBox();
   res->addItem("Low");
   res->addItem("Medium");
   res->addItem("High");
   res->setCurrentIndex(2);

   //  Cursor size slider
   cursorSize = new QSlider(Qt::Horizontal);
   cursorSize->setRange(5,50);
   cursorSize->setValue(20);

   //  Pause/resume button
   light = new QPushButton("Pause");

   //  Cursor buttons
   QPushButton* left = new QPushButton(QString::fromUtf8("\u2190"));
   QPushButton* up = new QPushButton(QString::fromUtf8("\u2191"));
   QPushButton* right = new QPushButton(QString::fromUtf8("\u2192"));
   QPushButton* down = new QPushButton(QString::fromUtf8("\u2193"));
   draw = new QPushButton("Draw");

   //  Reset & Quit buttons
   QPushButton* reset = new QPushButton("Reset");
   QPushButton* quit = new QPushButton("Quit");

   //  Set layout of child widgets
   QGridLayout* layout = new QGridLayout;
   layout->addWidget(ogl,0,0,10,1);
   layout->addWidget(toggleFirstPerson,0,2,1,3);
   layout->addWidget(new QLabel("Resolution (reset)"),1,1);
   layout->addWidget(res,1,2,1,3);
   layout->addWidget(new QLabel("Cursor size"),2,1);
   layout->addWidget(cursorSize,2,2,1,3);

   layout->addWidget(up,5,3);
   layout->addWidget(left,6,2);
   layout->addWidget(draw,6,3);
   layout->addWidget(right,6,4);
   layout->addWidget(down,7,3);

   layout->addWidget(reset,9,1);
   layout->addWidget(quit,9,2,1,3);
   //  Manage resizing
   layout->setColumnStretch(0,100);
   layout->setColumnMinimumWidth(0,100);
   layout->setRowStretch(9,100);
   setLayout(layout);

   //  Connect valueChanged() signals to ogl
   connect(toggleFirstPerson,SIGNAL(clicked(bool))     , this,SLOT(setFirstPerson(bool)));
   connect(res,SIGNAL(currentIndexChanged(int))        , ogl,SLOT(setPrecision(int)));
   connect(cursorSize,SIGNAL(valueChanged(int))     , ogl,SLOT(setCursorSize(int)));
   //  Connect cursor signals
   connect(left, SIGNAL(pressed()),this,SLOT(moveLeft()));
   connect(up,   SIGNAL(pressed()),this,SLOT(moveUp()));
   connect(right,SIGNAL(pressed()),this,SLOT(moveRight()));
   connect(down, SIGNAL(pressed()),this,SLOT(moveDown()));
   connect(draw, SIGNAL(pressed()),ogl,SLOT(cursorDown()));

   //  Connect reset() and quit() signals 
   connect(reset,SIGNAL(pressed()) , this,SLOT(resetThings()));
   connect(quit,SIGNAL(pressed()) , qApp,SLOT(quit()));

   //  Force keyboard focus to ogl
   ogl->setFocus();
}

//
//  Reset things
//
void viewer::resetThings()
{
   res->setCurrentIndex(2);
   cursorSize->setValue(20);
   ogl->reset(); // May be redundant...
}

//
// Handle cursor interaction
//
void viewer::moveLeft()  {ogl->moveCursorBy(-1,  0);}
void viewer::moveRight() {ogl->moveCursorBy( 1,  0);}
void viewer::moveUp()    {ogl->moveCursorBy( 0, -1);}
void viewer::moveDown()  {ogl->moveCursorBy( 0,  1);}
void viewer::keyPressEvent(QKeyEvent * event)
{
   switch(event->key())
   {
      case Qt::Key_A:
         moveLeft();
         break;
      case Qt::Key_D:
         moveRight();
         break;
      case Qt::Key_W:
         moveUp();
         break;
      case Qt::Key_S:
         moveDown();
         break;
      case Qt::Key_Space:
         ogl->cursorDown();
         break;
   }
}

void viewer::setFirstPerson(bool ifp)
{
   ogl->setFirstPerson(ifp);
   draw->setVisible(!ifp);
}