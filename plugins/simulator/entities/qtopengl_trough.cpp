#include "qtopengl_trough.h"
#include <argos3/core/utility/math/vector2.h>
#include "trough_entity.h"
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_widget.h>

namespace argos {

   /****************************************/
   /****************************************/

   const GLfloat WATER_COLOR[]    = { 0.49f, 0.9f, 0.96f, 1.0f };
   const GLfloat FOOD_COLOR[] = { 0.46f, 0.42f, 0.03f, 1.0f };
   const GLfloat SPECULAR[]         = { 0.0f, 0.0f, 0.0f, 1.0f };
   const GLfloat SHININESS[]        = { 0.0f                   };
   const GLfloat EMISSION[]         = { 0.0f, 0.0f, 0.0f, 1.0f };

   /****************************************/
   /****************************************/

    CQTOpenGLTrough::CQTOpenGLTrough() :
       m_unVertices(20){

       /* Reserve the needed display lists */
       m_unBaseList = glGenLists(1);
       m_unBodyList = m_unBaseList;

       glNewList(m_unBodyList, GL_COMPILE);
       MakeBody();
       glEndList();

    }

   /****************************************/
   /****************************************/

   CQTOpenGLTrough::~CQTOpenGLTrough() {
      glDeleteLists(m_unBaseList, 1);
   }

   /****************************************/
   /****************************************/


   void CQTOpenGLTrough::Draw(const CTroughEntity& c_entity) {
      /* Draw the body */
      if(c_entity.GetType() == WATER) {
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WATER_COLOR);
      }
      else {
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, FOOD_COLOR);
      }
      glPushMatrix();
      glScalef(c_entity.GetSize().GetX(), c_entity.GetSize().GetY(), c_entity.GetSize().GetZ());
      glCallList(m_unBodyList);
      glPopMatrix();
   }

   /****************************************/
   /****************************************/

   void CQTOpenGLTrough::MakeBody() {
	     /* Since this shape can be stretched,
	         make sure the normal vectors are unit-long */
	      glEnable(GL_NORMALIZE);

	      /* Set the material */
	      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SPECULAR);
	      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, SHININESS);
	      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, EMISSION);

	      /* Let's start the actual shape */

	      /* This part covers the top and bottom faces (parallel to XY) */
	      glBegin(GL_QUADS);
	      /* Bottom face */
	      glNormal3f(0.0f, 0.0f, -1.0f);
	      glVertex3f( 0.5f,  0.5f, 0.0f);
	      glVertex3f( 0.5f, -0.5f, 0.0f);
	      glVertex3f(-0.5f, -0.5f, 0.0f);
	      glVertex3f(-0.5f,  0.5f, 0.0f);
	      /* Top face */
	      glNormal3f(0.0f, 0.0f, 1.0f);
	      glVertex3f(-0.5f, -0.5f, 1.0f);
	      glVertex3f( 0.5f, -0.5f, 1.0f);
	      glVertex3f( 0.5f,  0.5f, 1.0f);
	      glVertex3f(-0.5f,  0.5f, 1.0f);
	      glEnd();
	      /* This part covers the faces (South, East, North, West) */
	      glBegin(GL_QUADS);
	      /* South face */
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, 1.0f);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f, -0.5f, 1.0f);
        /* East face */
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 0.5f, -0.5f, 1.0f);
        glVertex3f( 0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f,  0.5f, 0.0f);
        glVertex3f( 0.5f,  0.5f, 1.0f);
        /* North face */
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 0.5f,  0.5f, 1.0f);
        glVertex3f( 0.5f,  0.5f, 0.0f);
        glVertex3f(-0.5f,  0.5f, 0.0f);
        glVertex3f(-0.5f,  0.5f, 1.0f);
        /* West face */
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f,  0.5f, 1.0f);
        glVertex3f(-0.5f,  0.5f, 0.0f);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glVertex3f(-0.5f, -0.5f, 1.0f);
	      glEnd();
	      /* The shape definitions is finished */

	      /* We don't need it anymore */
	      glDisable(GL_NORMALIZE);
   }

   /****************************************/
   /****************************************/

   class CQTOpenGLOperationDrawTroughNormal : public CQTOpenGLOperationDrawNormal {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CTroughEntity& c_entity) {
         static CQTOpenGLTrough m_cModel;
         c_visualization.DrawEntity(c_entity.GetEmbodiedEntity());
         m_cModel.Draw(c_entity);
      }
   };

   class CQTOpenGLOperationDrawTroughSelected : public CQTOpenGLOperationDrawSelected {
   public:
      void ApplyTo(CQTOpenGLWidget& c_visualization,
                   CTroughEntity& c_entity) {
         c_visualization.DrawBoundingBox(c_entity.GetEmbodiedEntity());
      }
   };

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawNormal, CQTOpenGLOperationDrawTroughNormal, CTroughEntity);

   REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawSelected, CQTOpenGLOperationDrawTroughSelected, CTroughEntity);

   /****************************************/
   /****************************************/

}
