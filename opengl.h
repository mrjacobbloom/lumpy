//
//  OpenGL opengl Widget
//

#ifndef opengl_H
#define opengl_H

#include "CUgl.h"
#include "Plane.h"

class opengl : public CUgl
{
Q_OBJECT
private:
    QVector2D cursor;                       // Cursor position (in texture space)
    float cursorRad;                        // Cursor radius (in texture space)
    int bufSize;                            // Size for framebuffers
    QOpenGLFramebufferObject* framebuf[2];  // Framebuffers for lump maps
    QImage bufImg;                          // Current framebuffer as image (for getting pixel colors w/o repeatedly binding the buffer)
    int bufId;                              // Which frame buffer to use
    QOpenGLFramebufferObject* pickbuf;      // Framebuffer for couse picking
    bool isGestureDrag;                     // Track whether a mouse interaction is a drag or click (moves outside a click radius)
    GLenum precision;                       // Allow user t ocontrol precision
    Plane* landPlane;                       // Planes for land and sea
    Plane* waterPlane;
    unsigned int DL_PLANT;                  // DL for plants
    bool isFirstPerson;                     // Whether in first person mode
    QVector2D firstPersonLoc;               // First person location (texcoords)
    QVector2D firstPersonVelocity;          // Velocity for forward/back and th
    QTimer velocityTimer;                   // Timer for velocity damping
    float firstPersonLumpData[4];           // Info about lump st firstPersonLoc
    // Shaders
    QOpenGLShaderProgram* S_RENDER;
    QOpenGLShaderProgram* S_RENDER_GOD;
    QOpenGLShaderProgram* S_ADD_LUMP;
    QOpenGLShaderProgram* S_PICK;
    QOpenGLShaderProgram* S_INIT;
    QOpenGLShaderProgram* S_WATER;
    QOpenGLShaderProgram* S_SNOW;
    // Textures
    QOpenGLTexture* T_SAND;
    QOpenGLTexture* T_GRASS;
    QOpenGLTexture* T_ROCK;
    QOpenGLTexture* T_LEAF;
    QOpenGLTexture* T_S_BACK;
    QOpenGLTexture* T_S_DOWN;
    QOpenGLTexture* T_S_FRONT;
    QOpenGLTexture* T_S_LEFT;
    QOpenGLTexture* T_S_RIGHT;
    QOpenGLTexture* T_S_UP;
    // Snow
    int     N;          // Snow particle count
    float*  Snow_Vert;  // Vertices
    float*  Snow_Tex;   // Tex coords
    float*  Snow_Start; // Start time
public:
   opengl(QWidget* parent=0);              // Constructor
   QSize sizeHint() const {return QSize(400,400);} // Default size of widget
public slots:
    void moveCursorBy(float,float);        // Move cursor
    void cursorDown();                     // Cursor activated
    void setCursorSize(int);               // "Cursor Size" slider
    void setPrecision(int);                // "Resolution" dropdown
    void reset();                          // Reset lump map
    void setFirstPerson(bool);
    void handleVelocity();                 // Handle first-person velocity damping and stuff
signals:
    void zoom(QString text);               // Zoom level
protected:
    void initializeGL();                   // Initialize widget
    void paintGL();                        // Draw widget
    void doModelViewProjection();
    void mousePressEvent(QMouseEvent*);    // Mouse pressed
    void mouseReleaseEvent(QMouseEvent*);  // Mouse released
    void mouseMoveEvent(QMouseEvent*);     // Mouse moved
    QOpenGLTexture* addTexture(QString filename);     // Load a texture from file.
    void doSkyBox();                       // Do skybox
    void initParticles();                  // Initialize snow particles
    void drawParticles();                  // Draw snow particles
    void initPlants();                     // Initialize plant DL
    void drawPlants();                     // Draw plants
    void updateFirstPersonLumpData();      // To be run on setFirstPerson and move events...
};

#endif
