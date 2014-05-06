#include <cstdlib>
#include <cstring>
#include "ImageLoader.h"
#include <png.h>
#include "Texture.h"

namespace gl{

bool loadPngToGlTex2D(std::string filepath, gl::GlTexture2D* tex) {
	 png_image image;
	 memset(&image, 0, (sizeof image));
	 image.version = PNG_IMAGE_VERSION;
	 if (png_image_begin_read_from_file(&image, filepath.c_str()))
     {
		 png_bytep buffer;
		 image.format = PNG_FORMAT_RGBA;

		 buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));
		 if (buffer != NULL &&
            png_image_finish_read(&image, NULL/*background*/, buffer,
               0/*row_stride*/, NULL/*colormap*/))
         {
			 tex->setImage(GL_RGBA,glm::ivec2(image.width,image.height),GL_UNSIGNED_BYTE,buffer);
			 free(buffer);
			 return true;
		 }
		 else
		 {
			 /* Calling png_image_free is optional unless the simplified API was
             * not run to completion.  In this case if there wasn't enough
             * memory for 'buffer' we didn't complete the read, so we must free
             * the image:
             */
            if (buffer == NULL)
               png_image_free(&image);

            else
               free(buffer);

			return false;
      }
	 }
	 return false;
}

bool imageDataFromPngFile(std::string filepath, glm::ivec2* size, char** data, int* bufflen){
	png_image image;
	 memset(&image, 0, (sizeof image));
	 image.version = PNG_IMAGE_VERSION;
	 if(png_image_begin_read_from_file(&image, filepath.c_str())){
		 png_bytep buffer;
		 image.format = PNG_FORMAT_RGBA;

		 buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));
		 if (buffer != NULL &&
            png_image_finish_read(&image, NULL/*background*/, buffer,
               0/*row_stride*/, NULL/*colormap*/))
         {
			 *data = (char*)buffer;
			 *bufflen = PNG_IMAGE_SIZE(image);
			 *size = glm::ivec2(image.width,image.height);
			 return true;
		 }
		 else
		 {
			 /* Calling png_image_free is optional unless the simplified API was
             * not run to completion.  In this case if there wasn't enough
             * memory for 'buffer' we didn't complete the read, so we must free
             * the image:
             */
            if (buffer == NULL)
               png_image_free(&image);

            else
               free(buffer);

			return false;
      }
	 }
	 return false;
}

bool loadGlTex2dFromPngBytestream(gl::GlTexture2D* tex, char* bytes, int length){
	 png_image image;
	 memset(&image, 0, (sizeof image));
	 image.version = PNG_IMAGE_VERSION;
	 if(png_image_begin_read_from_memory(&image,(void*)bytes,length)){
		 png_bytep buffer;
		 image.format = PNG_FORMAT_RGBA;

		 buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));
		 if (buffer != NULL &&
            png_image_finish_read(&image, NULL/*background*/, buffer,
               0/*row_stride*/, NULL/*colormap*/))
         {
			 tex->setImage(GL_RGBA,glm::ivec2(image.width,image.height),GL_UNSIGNED_BYTE,buffer);
			 return true;
		 }
		 else
		 {
			 /* Calling png_image_free is optional unless the simplified API was
             * not run to completion.  In this case if there wasn't enough
             * memory for 'buffer' we didn't complete the read, so we must free
             * the image:
             */
            if (buffer == NULL)
               png_image_free(&image);

            else
               free(buffer);

			return false;
      }
	 }
	 return false;
}

} //namespace gl
