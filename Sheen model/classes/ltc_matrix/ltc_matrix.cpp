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

#include "ltc_matrix.h"

#include <assert.h>

#include "GL/glew.h"

namespace bgq_opengl {

    LTCMatrix::LTCMatrix(int mat, const char* name, GLuint slot) {
        
        // The slot has to be a positive number because OpenGL does weird stuff on macOS else.
        if (slot < 1) assert(false);

        // Generate a texture in OpenGL and store the parameters in the attributes.
        glGenTextures(1, &this->ID);
        this->name = std::string(name);
        this->slot = slot;
    
        // Set the slot for the texture.
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, this->ID);
        
        // Configure the magnifying algorithm, minifying algorithm and repetition.
        // This tells OpenGL how to apply intrpolation.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Load the 'image' to OpenGL.
        if (mat == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, mat_ltc_1);
        else if (mat == 2)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, mat_ltc_2);
        else if (mat == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGB, GL_FLOAT, mat_ltc_sheen);
        else
            assert(false);

        // Unbinds the OpenGL Texture.
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    GLuint LTCMatrix::getID() {

        return this->ID;

    }

    GLuint LTCMatrix::getSlot() {

        return this->slot;

    }

    std::string LTCMatrix::getName() {

        return this->name;

    }

    void LTCMatrix::bind() {

        // Activate the texture and bind it.
        glActiveTexture(GL_TEXTURE0 + this->slot);
        glBindTexture(GL_TEXTURE_2D, this->ID);

    }

    void LTCMatrix::remove() {

        glDeleteTextures(1, &this->ID);

    }

    void LTCMatrix::unbind() {

        glBindTexture(GL_TEXTURE_2D, 0);

    }

} // namespace bgq_opengl
