/**
 * @file main.h
 * @brief Main header aladdin 3d file.
 * @version 1.0.0 (2022-10-21)
 * @date 2022-10-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_MAIN_H_
#define BGQ_OPENGL_MAIN_H_

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 800
#define GAME_NAME "Real-time animation"
#define NORM_SIZE 1.0
#define FPS_STEP 1000

#include <vector>
#include <string>
#include <ctime>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "classes/camera/camera.h"
#include "classes/object/object.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/turbulence/turbulence.h"
#include "classes/ltc_matrix/ltc_matrix.h"

std::vector<bgq_opengl::Object> scene_1;    /// Holds all the displayed objects in scene 1.
std::vector<bgq_opengl::Object> scene_2;    /// Holds all the displayed objects in scene 1.
bgq_opengl::Camera *camera;                 /// Holds all the existing cameras.
bgq_opengl::Shader *shader_ltc;             /// Holds the initialised
std::vector<bgq_opengl::Texture> textures;  /// The initialised textures.
bgq_opengl::LTCMatrix *ltc_1;               
bgq_opengl::LTCMatrix *ltc_2;
bgq_opengl::LTCMatrix *ltc_sheen;
int selected_scene = 1;
GLFWwindow *window = 0;						/// Window ID.
double internal_time = 0;					/// Time that will rule everything in the game.

// GUI Vars.
glm::vec3 fabric_color(0.30f, 0.65f, 0.46f);
float light_intensity = 10;
float fabric_roughness = 1.0f;
float fabric_specular = 0.0f;
float alpha = 0.5f;
float beta = 0.5f;
float csheen = 0.5f;
int sheenType = 2;

double fps = 0.0;
int fps_counted = 0;
double fps_time = 0.0;

const glm::vec4 background(82 / 255.0, 103 / 255.0, 125 / 255.0, 1.0);

/**
 * @brief Clean everything to end the program.
 *
 * Clean everything to end the program.
 */
void clean();

/**
 * @brief Clear the viewport.
 *
 * Clear the viewport before printing anything else.
 */
void clear();

/**
 * @brief Display the OpenGL elements.
 *
 * Display all the OpenGL elements in the scene.
 */
void displayElements();

/**
 * @brief Display the GUI.
 *
 * This function will display the GUI elements.
 */
void displayGUI();

/**
 * @brief Handles the key events.
 *
 * Handles the freeglut key events.
 */
void handleKeyEvents(unsigned char key, int x, int y);

/**
 * @brief Init the elements of the program
 *
 * Initialize the objects, elements and all.
 */
void initElements();

/**
 * @brief Init the environment.
 *
 * Initialize the OpenGL, Glew and Freeglut environments.
 */
void initEnvironment(int argc, char** argv);

/**
 * @brief Main function.
 * 
 * Main function.
 */
int main(int argc, char** argv);

#endif  //!BGQ_OPENGL_MAIN_H_
