//
//  CU OpenGL Widget
//

#ifndef CUGL_H
#define CUGL_H

#include <QtOpenGL>
#include <QString>
#include <QVector>
#include "Object.h"

class CUgl : public QOpenGLWidget
{
Q_OBJECT
private:
   float            La,Ld,Ls; // Light intensity
   float            Lr;       // Light radius
   float            zh;       // Light angle
   float            ylight;   // Light elevation
   bool             move;     // Moving light
   int              obj;      // Selected Object
   QTimer           timer;    // Timer for animations
protected:
   QElapsedTimer    time;     // Track elapsed time
   QVector<Object*> objects;  // Objects
   bool             mouse;    // Mouse pressed
   QPoint           pos;      // Mouse position
   int              fov;      // Field of view
   float            Dim;      // Default size
   float            dim;      // Display size
   int              th,ph;    // Display angles
   int              mode;     // Selected shader
   QVector<QOpenGLShaderProgram*> shader; // Shaders
public:
   CUgl(QWidget* parent=0,bool fixed=true);        // Constructor
   QSize sizeHint() const {return QSize(400,400);} // Default size of widget
public slots:
   void reset();
   void setDim(float d);                            // Set scene size
   void setPerspective(int on);                     // Set perspective
   void setShader(int sel);                         // Set shader
   void setObject(int type);                        // Set displayed object
   void addObject(Object* obj);                     // Add object
   void doScene();                                  // Draw scene
   void setLightMove(bool on);                      // Set light animation
   void setLightAngle(int th);                      // Set light angle
   void setLightElevation(int pct);                 // Set light elevation (percent)
   void setLightIntensity(float a,float d,float s); // Set light intensity
   void maxFPS(bool);                               // Set max fps
   QOpenGLShaderProgram* addShader(QString vert,QString frag,QString names=""); // Add shader
   void addShader3(QString vert,QString geom,QString frag);    // Add shader
protected:
   void initializeGL();                             // Initialization
   void mousePressEvent(QMouseEvent*);              // Mouse pressed
   void mouseReleaseEvent(QMouseEvent*);            // Mouse released
   void mouseMoveEvent(QMouseEvent*);               // Mouse moved
   void wheelEvent(QWheelEvent*);                   // Mouse wheel
   void Fatal(QString message);                     // Error handler
   QVector3D doLight();                             // Enable light
   void doModelViewProjection();                    // Apply projection
   void doOrtho(int w,int h,float zoom=1,float dx=0,float dy=0); // Apply orthographic projection with zoom and pan
   float getTime() {return 0.001*time.elapsed();}   // Elapsed time in seconds
   QOpenGLTexture* loadImage(const QString file);   // Method to load texture
private slots:
   void tick();                                     // Method to capture timer ticks
signals:
   void light(int angle);                           // Signal for light angle
   void angles(QString text);                       // Signal for view angles
};

#endif
