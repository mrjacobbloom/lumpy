#ifndef bloom_finalV_H
#define bloom_finalV_H

#include <QWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include "opengl.h"

class viewer : public QWidget
{
Q_OBJECT
private:
   QPushButton*    draw;
   QPushButton*    light;
   QComboBox*      res;
   QSlider*        cursorSize;
   opengl*         ogl;
private slots:
   void resetThings();  //  Reset things
   void moveLeft();     //  Move cursor
   void moveUp();
   void moveRight();
   void moveDown();
   void setFirstPerson(bool);
public:
    viewer();
protected:
   void keyPressEvent(QKeyEvent*);
};

#endif
