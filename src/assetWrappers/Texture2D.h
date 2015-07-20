#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__
#include "../buffers/TextureBuffer.h"
#include "../render/TextureUnitManager.h"
#include <GL/glew.h>
#include <string>
class Texture2D
{
public:
   /**
    * Create an RGB texture. (No alpha support, yet...)
    * Will also have support for loading texture metadata
    */
   Texture2D(std::string textureName);
   /**
    * Enable the texture on the shader
    * @param samplerID the id of the texture sampler
    */
   void enable(GLint samplerID);
   /**
    * Disable the texture on the shader
    */
   void disable();
   const std::string getName();
private:
   std::string textureName;
   int width;
   int height;
   TextureBuffer bfr;
   std::shared_ptr<TextureUnit> texUnit;

};


#endif