/**
 * @file ltc_matrix.h
 * @brief  Linearly Transformed Cosines matrix class.
 * @version 1.0.0 (2023-03-31)
 * @date 2023-03-31
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 *
 * @see The precomputed cosines are from https://github.com/zz92118/Learn-Opengl-zz/blob/b3cbc576082c45e51abf0af1e723fbefb4178b41/8.guest/2022/7.area_lights/ltc_matrix.hpp
 */

#ifndef BGQ_OPENGL_CLASSES_LTC_H_
#define BGQ_OPENGL_CLASSES_LTC_H_

#include "ltc_matrix_data.h"

#include <string>

#include "GL/glew.h"

namespace bgq_opengl {

    /**
     * @brief Implements a LTC matris as a texture.
     *
     * Implements a LTC matris as a texture.
     *
     * @author Borja García Quiroga <garcaqub@tcd.ie>
     */
    class LTCMatrix {
    
        public:

            /**
             * @brief Creates a texture from an image.
             *
             * Creates a textures and passes it to OpenGL.
             *
             * @param mat The data to be loaded.
             * @param name Texture name in the shader.
             * @param slot Texture slot.
             */
            LTCMatrix(int mat, const char* name, GLuint slot);

            /**
             * @brief Get the ID of the texture.
             *
             * Get the ID of the texture.
             *
             * @returns The ID of the texture.
             */
            GLuint getID();

            /**
             * @brief Get the slot of the texture.
             *
             * Get the slot of the texture.
             *
             * @returns The slot of the texture.
             */
            GLuint getSlot();
        
            /**
             * @brief Gets the texture name.
             *
             * Gets the texture name as a char array.
             *
             * @returns A char string containing the name name of the texture.
             */
            std::string getName();

            /**
             * @brief Binds the texture.
             *
             * Binds the texture.
             */
            void bind();

            /**
             * @brief Removes the texture from OpenGL.
             *
             *  Removes the texture from OpenGL.
             */
            void remove();

            /**
             * @brief Unbinds the texture.
             *
             * Unbinds the texture.
             */
            void unbind();

        private:

            GLuint ID;          /// Texture OpenGL ID.
            GLuint slot;        /// Stores the texture slot number.
            std::string name;   /// Texture name.

    };

}

#endif  //!BGQ_OPENGL_CLASSES_LTC_H_

