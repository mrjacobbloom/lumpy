//
//  OpenGL widget
//
#include "opengl.h"
#define Cos(th) cos(M_PI/180*(th))
#define Sin(th) sin(M_PI/180*(th))

//
//  Constructor
//
opengl::opengl(QWidget* parent)
    : CUgl(parent)
{
   cursor[0] = cursor[1] = 0.5;
   cursorRad = 0.2;
   bufSize = 256;
   bufId = 0;
   framebuf[0] = NULL;
   framebuf[1] = NULL;
   pickbuf = NULL;

   isFirstPerson = false;
   firstPersonLoc[0] = firstPersonLoc[1] = 0.5;
   firstPersonVelocity[0] = firstPersonVelocity[1] = 0;

   N = 70;

   // Use a timer for velocity damping cuz we can never trust vsync
   velocityTimer.setInterval(1000/60);
   connect(&velocityTimer,SIGNAL(timeout()),this,SLOT(handleVelocity()));
   velocityTimer.start();
}

//
//  Initialize
//
void opengl::initializeGL()
{
   // Load shaders
   S_RENDER     = addShader(":/shaders/render.vert",":/shaders/render.frag");
   S_RENDER_GOD = addShader(":/shaders/render.god.vert",":/shaders/render.god.frag");
   S_ADD_LUMP   = addShader("",":/shaders/addlump.frag");
   S_PICK       = addShader(":/shaders/render.vert",":/shaders/pick.frag");
   S_INIT       = addShader("",":/shaders/initframebuffer.frag");
   S_WATER      = addShader(":/shaders/water.vert","");
   S_SNOW       = addShader(":/shaders/snow.vert","",",,,,,Start");

   // Load textures
   T_SAND    = addTexture(":/textures/sand.png");
   T_GRASS   = addTexture(":/textures/grass.png");
   T_ROCK    = addTexture(":/textures/rock.png");
   T_LEAF    = addTexture(":/textures/leaf.png");
   T_S_BACK  = addTexture(":/textures/sor_sea/sea_bk.JPG");
   T_S_DOWN  = addTexture(":/textures/sor_sea/sea_dn.JPG");
   T_S_FRONT = addTexture(":/textures/sor_sea/sea_ft.JPG");
   T_S_LEFT  = addTexture(":/textures/sor_sea/sea_lf.JPG");
   T_S_RIGHT = addTexture(":/textures/sor_sea/sea_rt.JPG");
   T_S_UP    = addTexture(":/textures/sor_sea/sea_up.JPG");

   landPlane = new Plane("");
   waterPlane = new Plane(":/textures/water.png");
   waterPlane->setColor(0.5, 0.6, 0.8, 0.4);
   waterPlane->setTranslate(0, 0.4, 0);

   // Initialize particles
   initParticles();

   // Initialize plants DL
   initPlants();

   // Initialize framebuffers
   setPrecision(2);

   // Dispatch mouse events when mouse not down
   setMouseTracking(true);

   // Make sure clear color window background color
   QColor wbg = QWidget::palette().color(QWidget::backgroundRole());
   glClearColor(wbg.redF(),wbg.greenF(),wbg.blueF(),1);

}

//
// Select framebuffer precision (reinitializes framebuffers)
//
void opengl::setPrecision(int prec)
{
   switch(prec) {
      case 0:
         precision = GL_RGBA4;
         bufSize   = 64;
         break;
      case 1:
         precision = GL_RGBA8;
         bufSize   = 256;
         break;
      case 2:
         precision = GL_RGBA16;
         bufSize   = 512;
         break;
   }
   bufId = 0;
   QOpenGLFramebufferObjectFormat frmt;
   frmt.setInternalTextureFormat(precision);
   if(framebuf[0]) delete framebuf[0];
   framebuf[0] = new QOpenGLFramebufferObject(bufSize,bufSize, frmt);
   if(framebuf[1]) delete framebuf[1];
   framebuf[1] = new QOpenGLFramebufferObject(bufSize,bufSize, frmt);

   doOrtho(bufSize,bufSize);

   //  Set output buffer
   framebuf[bufId]->bind();

   //  Enable shader
   S_INIT->bind();

   //  Draw image
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex2f(-1,-1);
   glTexCoord2f(1,0); glVertex2f(+1,-1);
   glTexCoord2f(1,1); glVertex2f(+1,+1);
   glTexCoord2f(0,1); glVertex2f(-1,+1);
   glEnd();

   //  Release shader
   S_INIT->release();

   //  Release output buffer
   framebuf[bufId]->release();

   QImage fboImage = framebuf[bufId]->toImage();
   bufImg = QImage(fboImage.constBits(), fboImage.width(), fboImage.height(), QImage::Format_ARGB32);

   update();
}

//
// Do projections based on whether we're in first-person or god modes
//
void opengl::doModelViewProjection()
{
   if(isFirstPerson)
   {
      //  Window dimensions accounting for retina displays
      int w = width()*devicePixelRatio();
      int h = height()*devicePixelRatio();
      float asp = w / (float)h;
      float dim = 4;
      //  Viewport is whole screen
      glViewport(0,0,w,h);
      //  Set Projection
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      double zmin = dim/25;
      double zmax = 16*dim;
      double ydim = zmin*tan(fov*M_PI/360);
      double xdim = ydim*asp;
      glFrustum(-xdim,+xdim,-ydim,+ydim,zmin,zmax);
      //  Set ModelView
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      float lumpHeight = firstPersonLumpData[0] * 3.5;
      float normal[3];
      normal[0] = 2.0*firstPersonLumpData[1] - 1.0;
      normal[1] = 2.0*firstPersonLumpData[2] - 1.0;
      normal[2] = 2.0*firstPersonLumpData[3] - 1.0;

      // Calculate camera angles based on normal of lump underneath POV
      glRotated(-ph, 1,0,0); // elevation
      glRotated(-th,0,1,0); // azimuth
      // angle between normal and vertical
      float angle = acos(normal[1])*57.3;
      // rotate around cross product of normal and vertical
      // not by the full angle because it's disorienting/head stabilization
      glRotated(-0.2*angle, normal[2],0,-normal[0]);


      float x2 = -(firstPersonLoc[0]*2.0-1.0)*landPlane->rad;
      float y2 = -.6 - lumpHeight;
      float z2 = -(firstPersonLoc[1]*2.0-1.0)*landPlane->rad;
      glTranslated(x2,y2,z2);
      
      //  Emit angles to display
      emit angles(QString::number(th)+","+QString::number(ph));
   }
   else
      CUgl::doModelViewProjection();
}

//
// Updates when switching to first person or when position changes
//
void opengl::updateFirstPersonLumpData()
{
   framebuf[bufId]->bind();
   int x = firstPersonLoc[0] * bufSize;
   int y = firstPersonLoc[1] * bufSize;
   glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, firstPersonLumpData);
   framebuf[bufId]->release();
}

//
//  Do skybox
//
#define sbr 30
void opengl::doSkyBox()
{
   glDisable(GL_LIGHTING);
   glColor4f(1,1,1,1);
   glEnable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);

   T_S_BACK->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0,1); glVertex3f(-sbr,  sbr, -sbr);
   glTexCoord2f(0,0); glVertex3f(-sbr, -sbr, -sbr);
   glTexCoord2f(1,0); glVertex3f( sbr, -sbr, -sbr);
   glTexCoord2f(1,1); glVertex3f( sbr,  sbr, -sbr);
   glEnd();
   
   T_S_LEFT->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-sbr, -sbr,  sbr);
   glTexCoord2f(1,0); glVertex3f(-sbr, -sbr, -sbr);
   glTexCoord2f(1,1); glVertex3f(-sbr,  sbr, -sbr);
   glTexCoord2f(0,1); glVertex3f(-sbr,  sbr,  sbr);
   glEnd();
   
   T_S_RIGHT->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f( sbr, -sbr, -sbr);
   glTexCoord2f(1,0); glVertex3f( sbr, -sbr,  sbr);
   glTexCoord2f(1,1); glVertex3f( sbr,  sbr,  sbr);
   glTexCoord2f(0,1); glVertex3f( sbr,  sbr, -sbr);
   glEnd();
   
   T_S_FRONT->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(1,0); glVertex3f(-sbr, -sbr,  sbr);
   glTexCoord2f(1,1); glVertex3f(-sbr,  sbr,  sbr);
   glTexCoord2f(0,1); glVertex3f( sbr,  sbr,  sbr);
   glTexCoord2f(0,0); glVertex3f( sbr, -sbr,  sbr);
   glEnd();
   
   T_S_UP->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0,1); glVertex3f(-sbr,  sbr, -sbr);
   glTexCoord2f(0,0); glVertex3f( sbr,  sbr, -sbr);
   glTexCoord2f(1,0); glVertex3f( sbr,  sbr,  sbr);
   glTexCoord2f(1,1); glVertex3f(-sbr,  sbr,  sbr);
   glEnd();
   
   T_S_DOWN->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-sbr, -sbr, -sbr);
   glTexCoord2f(0,1); glVertex3f( sbr, -sbr, -sbr);
   glTexCoord2f(1,1); glVertex3f(sbr, -sbr,  sbr);
   glTexCoord2f(1,0); glVertex3f(-sbr, -sbr,  sbr);
   glEnd();

   glEnable(GL_LIGHTING);
}

//
//  Draw the window
//
void opengl::paintGL()
{
   //  Make sure widget has keyboard focus
   //setFocus();

   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //  Apply projection
   doModelViewProjection();

   //  Enable lighting
   doLight();

   //  Do skybox
   if(isFirstPerson)
      doSkyBox();
      

   //  Apply shader for land map
   QOpenGLShaderProgram* landShader = isFirstPerson ? S_RENDER : S_RENDER_GOD;
   landShader->bind();

   // cursor info
   landShader->setUniformValue("cursorRad",cursorRad);
   landShader->setUniformValue("cursor",cursor);
   
   // Pass in display textures
   glActiveTexture(GL_TEXTURE1);
   T_SAND->bind();
   landShader->setUniformValue("sand", 1);

   glActiveTexture(GL_TEXTURE2);
   T_GRASS->bind();
   landShader->setUniformValue("grass", 2);

   glActiveTexture(GL_TEXTURE3);
   T_ROCK->bind();
   landShader->setUniformValue("rock", 3);

   // Pass in lump map
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,framebuf[bufId]->texture());
   landShader->setUniformValue("lumpMap", 0);

   //  Draw land
   landPlane->display();

   //  Release shader
   landShader->release();

   drawPlants();

   //  Draw the water last because transparency
   //  Apply shader
   S_WATER->bind();
   // cursor info
   S_WATER->setUniformValue("time",(float) 0.003*time.elapsed());

   waterPlane->display();

   S_WATER->release();

   // Draw snow
   drawParticles();

   glDisable(GL_BLEND);

   glDisable(GL_LIGHTING);
   glDisable(GL_DEPTH_TEST);
}

//
// Initialize plants DL
// Adapted from last year's final
//
void opengl::initPlants() {
   DL_PLANT = glGenLists(1);
   glNewList(DL_PLANT, GL_COMPILE);

   glEnable(GL_TEXTURE_2D);
   T_LEAF->bind();

   glScalef(0.2,0.1,0.1);
  
   glBegin(GL_QUAD_STRIP);
   glNormal3d(0,0,1);
   glTexCoord2d(0, 0.0); glVertex3d(-0.1, 0, 0.2);
   glTexCoord2d(1, 0.0); glVertex3d(+0.1, 0, 0.2);
   glNormal3d(0,-.2,1);
   glTexCoord2d(0, 0.2); glVertex3d(-0.1, 1.1, 0.25);
   glTexCoord2d(1, 0.2); glVertex3d(+0.1, 1.1, 0.25);
   glNormal3d(0,-.4,1);
   glTexCoord2d(0, 0.4); glVertex3d(-0.1, 1.3, 0.3);
   glTexCoord2d(1, 0.4); glVertex3d(+0.1, 1.3, 0.3);
   glNormal3d(0,-.6,1);
   glTexCoord2d(0, 0.6); glVertex3d(-0.07, 1.5, 0.5);
   glTexCoord2d(1, 0.6); glVertex3d(+0.07, 1.5, 0.5);
   glNormal3d(0,-.8,1);
   glTexCoord2d(0, 0.8); glVertex3d(-0.05, 1.7, 0.7);
   glTexCoord2d(1, 0.8); glVertex3d(+0.05, 1.7, 0.7);
   glEnd();
   glBegin(GL_TRIANGLES); // the tip bit
   glTexCoord2d(0, 0.8); glVertex3d(-0.05, 1.7, 0.7);
   glTexCoord2d(1, 0.8); glVertex3d(+0.05, 1.7, 0.7);
   glNormal3d(0,-1,1);
   glTexCoord2d(0.5, 1); glVertex3d(0, 1.8, 1);
   glEnd();

   glEndList();
}

//
// Draw some plants at grass height
//
#define plant_step 15
void opengl::drawPlants()
{
   for(int i = 0; i < bufSize-1; i += plant_step)
   {
      uchar* line = bufImg.scanLine(i);
      for(int j = 0; j < bufSize-1; j += plant_step)
      {
         uchar r = line[4*j + 2];
         float lumpHeight = r/256.0 * 3.5;
         if(lumpHeight > 0.8 && lumpHeight < 1.5)
         {
            glPushMatrix();
            float x = j/(float)bufSize;
            float y = 1.0 - i/(float)bufSize;
            glTranslatef(
               (x*2.0-1.0)*landPlane->rad,
               lumpHeight,
               (y*2.0-1.0)*landPlane->rad
            );
            glRotatef(i*11+j*20, 0,1,0);
            glCallList(DL_PLANT);
            glPopMatrix();
         }
      }
   }
}

//
// Initialize snow particles
//
void opengl::initParticles()
{
   Snow_Vert  = new float[3*N*N];
   Snow_Tex  = new float[2*N*N];
   Snow_Start = new float[N*N];
   //  Array Pointers
   float* vert  = Snow_Vert;
   float* tex   = Snow_Tex;
   float* start = Snow_Start;
   //  Loop over NxN patch
   for (int i=0;i<N;i++)
      for (int j=0;j<N;j++)
      {
         //  Location x,y,z
         *vert++ = (((float)i)/N * 2.0 - 1.0) * landPlane->rad;
         *vert++ = 4.5;
         *vert++ = (((float)j)/N * 2.0 - 1.0) * landPlane->rad;
         //  Texcoord s,t
         *tex++ = ((float)i)/N;
         *tex++ = ((float)j)/N;
         //  Launch time
         *start++ = rand()*2.0/RAND_MAX;
      }
}

//
// Draw snow particles
//
#define START_ARRAY 5
void opengl::drawParticles()
{
   // bind snow shader
   S_SNOW->bind();
   S_SNOW->setUniformValue("time",(float) 0.001*time.elapsed());
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,framebuf[bufId]->texture());
   S_SNOW->setUniformValue("lumpMap", 0);

   QOpenGLFunctions glf(QOpenGLContext::currentContext());
   //  Set particle size
   glPointSize(4);
   //  Point vertex location to local array Vert
   glVertexPointer(3,GL_FLOAT,0,Snow_Vert);
   glTexCoordPointer(2,GL_FLOAT,0,Snow_Tex);
   //  Point attribute arrays to local arrays
   glf.glVertexAttribPointer(START_ARRAY,1,GL_FLOAT,GL_FALSE,0,Snow_Start);
   //  Enable arrays used by DrawArrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glf.glEnableVertexAttribArray(START_ARRAY);
   //  Draw arrays
   glDrawArrays(GL_POINTS,0,N*N);
   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glf.glDisableVertexAttribArray(START_ARRAY);

   S_SNOW->release();
}

//
// Handle first-person velocity damping and stuff
//
#define fpedge 0.08 // Bounds of the first person view are smaller than the world
void opengl::handleVelocity()
{
   if(!isFirstPerson) return;
   firstPersonLoc[0] += Sin(th)*firstPersonVelocity[0];
   firstPersonLoc[1] += Cos(th)*firstPersonVelocity[0];

   th = (th+(int)firstPersonVelocity[1])%360;


   // clamp to edges
   if(firstPersonLoc[0] < fpedge) firstPersonLoc[0] = fpedge;
   if(firstPersonLoc[1] < fpedge) firstPersonLoc[1] = fpedge;
   if(firstPersonLoc[0] > 1-fpedge) firstPersonLoc[0] = 1-fpedge;
   if(firstPersonLoc[1] > 1-fpedge) firstPersonLoc[1] = 1-fpedge;

   updateFirstPersonLumpData();

   firstPersonVelocity[0] *= 0.7;
   firstPersonVelocity[1] *= 0.7;
}

//
// Move the cursor position (or in 1st person mode the camera) in texture space
// relative to camera angle
//
void opengl::moveCursorBy(float ds, float dt)
{
   if(isFirstPerson) {
      firstPersonVelocity[0] += 0.01*dt;
      if(firstPersonVelocity[0] < -0.03) firstPersonVelocity[0] = -0.03;
      if(firstPersonVelocity[0] >  0.03) firstPersonVelocity[0] =  0.03;

      firstPersonVelocity[1] -= 5*ds;
      if(firstPersonVelocity[1] < -10) firstPersonVelocity[1] = -10;
      if(firstPersonVelocity[1] >  10) firstPersonVelocity[1] =  10;
   } else {
      cursor[0] += 0.05*Cos(th)*ds - 0.05*Sin(th)*dt;
      cursor[1] += 0.05*Sin(th)*ds + 0.05*Cos(th)*dt;

      // clamp to edges
      if(cursor[0] < 0) cursor[0] = 0;
      if(cursor[0] > 1) cursor[0] = 1;
      if(cursor[1] < 0) cursor[1] = 0;
      if(cursor[1] > 1) cursor[1] = 1;
   }

   
   update();
}

//
// Set cursor down (growing)
//
void opengl::cursorDown()
{
   if(isFirstPerson) return;

   doOrtho(bufSize,bufSize);

   //  Swap buffers
   bufId = !bufId;

   //  Set output buffer
   framebuf[bufId]->bind();

   //  Enable shader
   S_ADD_LUMP->bind();
   S_ADD_LUMP->setUniformValue("cursorRad",cursorRad);
   S_ADD_LUMP->setUniformValue("cursor",cursor);
   //S_ADD_LUMP->setUniformValue("sub",-1);

   //  Bind previous buffer as input texture
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,framebuf[!bufId]->texture());
   
   //  Draw image
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex2f(-1,-1);
   glTexCoord2f(1,0); glVertex2f(+1,-1);
   glTexCoord2f(1,1); glVertex2f(+1,+1);
   glTexCoord2f(0,1); glVertex2f(-1,+1);
   glEnd();

   //  Release shader
   S_ADD_LUMP->release();

   //  Release output buffer
   framebuf[bufId]->release();

   QImage fboImage = framebuf[bufId]->toImage();
   bufImg = QImage(fboImage.constBits(), fboImage.width(), fboImage.height(), QImage::Format_ARGB32);

   update();
}

//
// Change the cursor size
//
void opengl::setCursorSize(int rad)
{
   cursorRad = float(rad)/100.0;
   update();
}

void opengl::mousePressEvent(QMouseEvent* e)
{
   mouse = true;
   isGestureDrag = false;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse moved
//
void opengl::mouseMoveEvent(QMouseEvent* e)
{
   if (mouse)
   {
      QPoint d = e->pos()-pos;  //  Change in mouse location
      if(!isGestureDrag) {
         float dist = sqrt(d.x()*d.x() + d.y()*d.y());
         if(dist < 4.0) {
            return;
         } else {
            isGestureDrag = true;
         }
      }
      th = (th+d.x())%360;      //  Translate x movement to azimuth
      ph = (ph+d.y())%360;      //  Translate y movement to elevation
      if(ph < -90) ph = -90;
      if(ph > 90) ph = 90;
      pos = e->pos();           //  Remember new location
      update();                 //  Request redisplay
   } else if(!isFirstPerson) {
      int w = width()*devicePixelRatio();
      int h = height()*devicePixelRatio();
      int x = e->pos().x()*devicePixelRatio();
      int y = h - e->pos().y()*devicePixelRatio();

      if(pickbuf) delete pickbuf;
      pickbuf = new QOpenGLFramebufferObject(w,h);

      pickbuf->bind();

      //  Clear screen and Z-buffer
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      //  Apply projection
      doModelViewProjection();

      //  Apply pick shader
      S_PICK->bind();

      // Pass in lump map
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,framebuf[bufId]->texture());
      S_PICK->setUniformValue("lumpMap", 0);

      //  Draw land plane only
      landPlane->display();

      //  Release pick shader
      S_PICK->release();

      // get color under the mouse from pick buffer
      float pixel[4];
      glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);

      // release pick buffer
      pickbuf->release();

      glDisable(GL_DEPTH_TEST);

      // move cursor to color, if successfully picked from the plane
      if(pixel[2]) {
         cursor[0] = pixel[0];
         cursor[1] = pixel[1];
         update();
      }
   }
}

//
//  Mouse released
//  If gesture was a click, do a pick render to decide where the cursor goes.
//
void opengl::mouseReleaseEvent(QMouseEvent*)
{
   mouse = false;
   if(!isGestureDrag)
      cursorDown();
}

//
// Add a texture
//
QOpenGLTexture* opengl::addTexture(QString filename)
{
   QOpenGLTexture* t = new QOpenGLTexture(QImage(filename).mirrored());
   t->setMinificationFilter(QOpenGLTexture::Linear);
   t->setMagnificationFilter(QOpenGLTexture::Linear);
   return t;
}

//
// Reset lump map
//
void opengl::reset()
{
   setPrecision(precision);
}

//
// Set first person mode
//
void opengl::setFirstPerson(bool f)
{
   isFirstPerson = f;
   if(isFirstPerson)
   {
      ph = 0;
      firstPersonVelocity[0] = firstPersonVelocity[1] = 0;
      updateFirstPersonLumpData();
   }
   else
   {
      ph = 30;
   }
   
   update();
}
