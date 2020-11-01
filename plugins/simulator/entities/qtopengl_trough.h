#ifndef QTOPENGL_TROUGH_H
#define QTOPENGL_TROUGH_H

namespace argos {
   class CQTOpenGLTrough;
   class CTroughEntity;
}

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

namespace argos {

   class CQTOpenGLTrough {

   public:

      CQTOpenGLTrough();

      virtual ~CQTOpenGLTrough();

      virtual void DrawLEDs(CTroughEntity& c_entity);
      virtual void Draw(const CTroughEntity& c_entity);

   private:

      void MakeBody();
      void MakeLED();

   private:

      GLuint m_unBaseList;
      GLuint m_unBodyList;
      GLuint m_unLEDList;
      GLuint m_unVertices;

   };

}

#endif
