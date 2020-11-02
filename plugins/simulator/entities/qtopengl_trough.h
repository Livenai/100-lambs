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

      virtual void Draw(const CTroughEntity& c_entity);

   private:

      void MakeBody();

   private:

      GLuint m_unBaseList;
      GLuint m_unBodyList;
      GLuint m_unVertices;

   };

}

#endif
