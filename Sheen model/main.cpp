/**
 * @file main.cpp
 * @brief Main aladdin 3d file.
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

#include "main.h"

#include <math.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/common.hpp"
#include "glm/gtx/string_cast.hpp"

#include "classes/camera/camera.h"
#include "classes/light/light.h"
#include "classes/object/object.h"
#include "classes/shader/shader.h"
#include "classes/ltc_matrix/ltc_matrix.h"
#include "structs/bounding_box/bounding_box.h"

void clean() {

	// Delete all the shaders.
    shader_ltc->remove();
    
    // Terminate ImGUI.
    ImGui_ImplGlfwGL3_Shutdown();
    
    // Close GL context and any other GLFW resources.
    glfwTerminate();

}

void clear() {
    
    // Specify the color of the background
    glClearColor(background.x, background.y, background.z, background.w);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void displayElements() {
    
    if (selected_scene == 1) {
        
         // Get info from the model.
         bgq_opengl::BoundingBox bb = scene_1[0].getBoundingBox();
         glm::vec3 centre = (bb.min + bb.max) / 2.0f;
         glm::vec3 size = bb.max - bb.min;
         float max_dim = std::max(size.x, std::max(size.y, size.z));
         float scale_rat = NORM_SIZE / max_dim * 0.75f;
         
         // Pass the parameters to the shaders.
         shader_ltc->activate();
         
         for (int i = 0; i < scene_1.size(); i++) {
             
             // Rotate, center the object and get it in the right position, resize it to normalize it.
             scene_1[i].resetTransforms();
             scene_1[i].rotate(0.0, 1.0, 0.0, internal_time * 5.0);
             scene_1[i].scale(scale_rat, scale_rat, scale_rat);
             scene_1[i].translate(-centre.x, -centre.y, -centre.z);
             
         }
         
        // SPHERE

        // Pass variables to the shaders.
        shader_ltc->passFloat("mainLight.intensity", light_intensity);
        shader_ltc->passVec("mainLight.color", glm::vec3(1.0f));
        shader_ltc->passVec("mainLight.points[0]", glm::vec3(3.0f + 0.125, 3.0f + 0.4, 3.0f - 0.575));
        shader_ltc->passVec("mainLight.points[1]", glm::vec3(3.0f + 0.575, 3.0f - 0.4, 3.0f - 0.175));
        shader_ltc->passVec("mainLight.points[2]", glm::vec3(3.0f - 0.125, 3.0f - 0.4, 3.0f + 0.575));
        shader_ltc->passVec("mainLight.points[3]", glm::vec3(3.0f - 0.575, 3.0f + 0.4, 3.0f + 0.175));
        shader_ltc->passBool("useAlt", true);
        shader_ltc->passVec("materialalt.diffuse", glm::vec3(0.2f));
        shader_ltc->passVec("materialalt.normalmap", glm::vec3(0.0f, 0.0f, 1.0f));
        shader_ltc->passVec("materialalt.specular", glm::vec3(0.3f));
        shader_ltc->passFloat("materialalt.roughness", 0.01f);
        shader_ltc->passFloat("alpha", alpha);
        shader_ltc->passFloat("beta", beta);
        shader_ltc->passFloat("Csheen", csheen);
        shader_ltc->passInt("sheenType", sheenType);
        shader_ltc->passBool("dust", false);

        // Pass the LTC.
        shader_ltc->passLTC(*ltc_1);
        shader_ltc->passLTC(*ltc_2);
        shader_ltc->passLTC(*ltc_sheen);

        // Draw the object.
        scene_1[1].draw(*shader_ltc, *camera);

        // CLOTH

        // Pass variables to the shaders.
        shader_ltc->passFloat("mainLight.intensity", light_intensity);
        shader_ltc->passVec("mainLight.color", glm::vec3(1.0f));
        shader_ltc->passVec("mainLight.points[0]", glm::vec3(3.0f + 0.125, 3.0f + 0.4, 3.0f - 0.575));
        shader_ltc->passVec("mainLight.points[1]", glm::vec3(3.0f + 0.575, 3.0f - 0.4, 3.0f - 0.175));
        shader_ltc->passVec("mainLight.points[2]", glm::vec3(3.0f - 0.125, 3.0f - 0.4, 3.0f + 0.575));
        shader_ltc->passVec("mainLight.points[3]", glm::vec3(3.0f - 0.575, 3.0f + 0.4, 3.0f + 0.175));
        shader_ltc->passBool("useAlt", true);
        shader_ltc->passVec("materialalt.diffuse", fabric_color);
        shader_ltc->passVec("materialalt.normalmap", glm::vec3(0.0f, 0.0f, 1.0f));
        shader_ltc->passVec("materialalt.specular", glm::vec3(fabric_specular));
        shader_ltc->passFloat("materialalt.roughness", fabric_roughness);
        shader_ltc->passFloat("material.roughness", fabric_roughness);
        shader_ltc->passFloat("alpha", alpha);
        shader_ltc->passFloat("beta", beta);
        shader_ltc->passFloat("Csheen", csheen);
        shader_ltc->passInt("sheenType", sheenType);
        shader_ltc->passBool("dust", false);

        // Pass the LTC.
        shader_ltc->passLTC(*ltc_1);
        shader_ltc->passLTC(*ltc_2);
        shader_ltc->passLTC(*ltc_sheen);
        
        // Pass the textures.
        // This one does not have any.

        // Draw the object.
        scene_1[0].draw(*shader_ltc, *camera);
        
    } else if (selected_scene == 2) {
        
        // Get info from the model.
        bgq_opengl::BoundingBox bb = scene_2[1].getBoundingBox();
        glm::vec3 centre = (bb.min + bb.max) / 2.0f;
        glm::vec3 size = bb.max - bb.min;
        float max_dim = std::max(size.x, std::max(size.y, size.z));
        float scale_rat = NORM_SIZE / max_dim;
        
        // Pass the parameters to the shaders.
        shader_ltc->activate();
        
        for (int i = 0; i < scene_2.size(); i++) {
            
            // Rotate, center the object and get it in the right position, resize it to normalize it.
            scene_2[i].resetTransforms();
            scene_2[i].rotate(0.0, 1.0, 0.0, internal_time * 5.0);
            scene_2[i].scale(scale_rat, scale_rat, scale_rat);
            scene_2[i].translate(-centre.x, -centre.y, -centre.z);
            
        }
        
        // FABRIC
        
        // Pass variables to the shaders.
        shader_ltc->passFloat("mainLight.intensity", light_intensity);
        shader_ltc->passVec("mainLight.color", glm::vec3(1.0f));
        shader_ltc->passVec("mainLight.points[0]", glm::vec3(3.0f + 0.125, 3.0f + 0.4, 3.0f - 0.575));
        shader_ltc->passVec("mainLight.points[1]", glm::vec3(3.0f + 0.575, 3.0f - 0.4, 3.0f - 0.175));
        shader_ltc->passVec("mainLight.points[2]", glm::vec3(3.0f - 0.125, 3.0f - 0.4, 3.0f + 0.575));
        shader_ltc->passVec("mainLight.points[3]", glm::vec3(3.0f - 0.575, 3.0f + 0.4, 3.0f + 0.175));
        shader_ltc->passBool("useAlt", true);
        shader_ltc->passVec("materialalt.diffuse", fabric_color);
        shader_ltc->passVec("materialalt.normalmap", glm::vec3(0.0f, 0.0f, 1.0f));
        shader_ltc->passVec("materialalt.specular", glm::vec3(fabric_specular));
        shader_ltc->passFloat("materialalt.roughness", fabric_roughness);
        shader_ltc->passFloat("material.roughness", fabric_roughness);
        shader_ltc->passFloat("alpha", alpha);
        shader_ltc->passFloat("beta", beta);
        shader_ltc->passFloat("Csheen", csheen);
        shader_ltc->passInt("sheenType", sheenType);
        shader_ltc->passBool("dust", false);

        // Pass the LTC.
        shader_ltc->passLTC(*ltc_1);
        shader_ltc->passLTC(*ltc_2);
        shader_ltc->passLTC(*ltc_sheen);
        
        // Pass the textures.
        // This one does not have any.

        // Draw the object.
        scene_2[0].draw(*shader_ltc, *camera);
        
        // TABLE
        
        // Pass variables to the shaders.
        shader_ltc->passFloat("mainLight.intensity", light_intensity);
        shader_ltc->passVec("mainLight.color", glm::vec3(1.0f));
        shader_ltc->passVec("mainLight.points[0]", glm::vec3(3.0f + 0.125, 3.0f + 0.4, 3.0f - 0.575));
        shader_ltc->passVec("mainLight.points[1]", glm::vec3(3.0f + 0.575, 3.0f - 0.4, 3.0f - 0.175));
        shader_ltc->passVec("mainLight.points[2]", glm::vec3(3.0f - 0.125, 3.0f - 0.4, 3.0f + 0.575));
        shader_ltc->passVec("mainLight.points[3]", glm::vec3(3.0f - 0.575, 3.0f + 0.4, 3.0f + 0.175));
        shader_ltc->passBool("useAlt", false);
        shader_ltc->passFloat("material.roughness", 0.4f);
        shader_ltc->passFloat("material.specular_mult", 0.3f);
        shader_ltc->passFloat("alpha", alpha);
        shader_ltc->passFloat("beta", beta);
        shader_ltc->passFloat("Csheen", csheen);
        shader_ltc->passInt("sheenType", sheenType);
        shader_ltc->passBool("dust", false);

        // Pass the LTC.
        shader_ltc->passLTC(*ltc_1);
        shader_ltc->passLTC(*ltc_2);
        shader_ltc->passLTC(*ltc_sheen);
        
        // Pass the textures.
        shader_ltc->passTexture(textures[0]);
        shader_ltc->passTexture(textures[1]);
        shader_ltc->passTexture(textures[2]);

        // Draw the object.
        scene_2[1].draw(*shader_ltc, *camera);
        
        // SEWING MACHINE
        
        // Pass variables to the shaders.
        shader_ltc->passFloat("mainLight.intensity", light_intensity);
        shader_ltc->passVec("mainLight.color", glm::vec3(1.0f));
        shader_ltc->passVec("mainLight.points[0]", glm::vec3(3.0f + 0.125, 3.0f + 0.4, 3.0f - 0.575));
        shader_ltc->passVec("mainLight.points[1]", glm::vec3(3.0f + 0.575, 3.0f - 0.4, 3.0f - 0.175));
        shader_ltc->passVec("mainLight.points[2]", glm::vec3(3.0f - 0.125, 3.0f - 0.4, 3.0f + 0.575));
        shader_ltc->passVec("mainLight.points[3]", glm::vec3(3.0f - 0.575, 3.0f + 0.4, 3.0f + 0.175));
        shader_ltc->passBool("useAlt", false);
        shader_ltc->passFloat("material.roughness", 0.01f);
        shader_ltc->passFloat("material.specular_mult", 0.2f);
        shader_ltc->passFloat("alpha", alpha);
        shader_ltc->passFloat("beta", beta);
        shader_ltc->passFloat("Csheen", csheen);
        shader_ltc->passInt("sheenType", sheenType);
        shader_ltc->passBool("dust", true);

        // Pass the LTC.
        shader_ltc->passLTC(*ltc_1);
        shader_ltc->passLTC(*ltc_2);
        shader_ltc->passLTC(*ltc_sheen);
        
        // Pass the textures.
        shader_ltc->passTexture(textures[3]);
        shader_ltc->passTexture(textures[4]);
        shader_ltc->passTexture(textures[5]);

        // Draw the object.
        scene_2[2].draw(*shader_ltc, *camera);
        
    }
        
}

void displayGUI() {
    
    // Init ImGUI for rendering.
    ImGui_ImplGlfwGL3_NewFrame();
    
    // Begin the new widget.
    ImGui::Begin("Parameters");
    
    // Pass these values to the shaders.
    ImGui::Text("Scene");
    ImGui::RadioButton("Scene 1", &selected_scene, 1);
    ImGui::RadioButton("Scene 2", &selected_scene, 2);
    
    ImGui::Text("Intensity");
    ImGui::SliderFloat("Light", &light_intensity, 0.0f, 50.0f);

    ImGui::Text("Fabric color");
    ImGui::SliderFloat("Red", &fabric_color.x, 0.0f, 1.0f);
    ImGui::SliderFloat("Green", &fabric_color.y, 0.0f, 1.0f);
    ImGui::SliderFloat("Blue", &fabric_color.z, 0.0f, 1.0f);
    
    ImGui::Text("Material");
    ImGui::SliderFloat("Rough", &fabric_roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular", &fabric_specular, 0.0f, 10.0f);
    
    ImGui::Text("Sheen");
    ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f);
    ImGui::SliderFloat("Beta", &beta, -10.0f, 10.0f);
    ImGui::SliderFloat("CSheen", &csheen, 0.0f, 20.0f);
    ImGui::RadioButton("Zeltner", &sheenType, 1);
    ImGui::RadioButton("Cosine-based", &sheenType, 2);
    ImGui::RadioButton("None", &sheenType, 0);

    ImGui::End();
    
    // Render ImGUI.
    ImGui::Render();
    
}

void handleKeyEvents() {
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('W')) {

        // Move forward.
        camera->moveFront();
        
    }

    // Key S will move camera 0 backwards.
    if (ImGui::IsKeyPressed('S')) {

        // Move backwards.
        camera->moveBack();

    }

    // Key D will move camera 0 to the right.
    if (ImGui::IsKeyPressed('D')) {

        // Move right.
        camera->moveRight();

    }

    // Key A will move camera 0 to the right.
    if (ImGui::IsKeyPressed('A')) {

        // Move to the left.
        camera->moveLeft();

    }

    // Key R will move camera up.
    if (ImGui::IsKeyPressed('R')) {

        // Move up.
        camera->moveUp();

    }

    // Key F will move camera down.
    if (ImGui::IsKeyPressed('F')) {

        // Move up.
        camera->moveDown();

    }

    // Key E will rotate camera left.
    if (ImGui::IsKeyPressed('E')) {

        // Move up.
        camera->rotateLeft();

    }

    // Key Q will rotate the camera right.
    if (ImGui::IsKeyPressed('Q')) {

        // Move up.
        camera->rotateRight();

    }

    // Key T will bring the camera up.
    if (ImGui::IsKeyPressed('T')) {

        // Move up.
        camera->rotateUp();

    }

    // Key G will bring the camera down.
    if (ImGui::IsKeyPressed('G')) {

        // Move up.
        camera->rotateDown();

    }
    
}

void initElements() {
    
    // Init the shader.
    shader_ltc = new bgq_opengl::Shader("ltc.vert", "ltc.frag");
    
	// Creates the first camera object
    camera = new bgq_opengl::Camera(glm::vec3(0.0f, 0.5f, 1.4f), glm::vec3(0.0f, -0.25f, -1.0f), 45.0f, 0.1f, 300.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Load the LTC matrixes.
    ltc_1 = new bgq_opengl::LTCMatrix(1, "LTC1", 1);
    ltc_2 = new bgq_opengl::LTCMatrix(2, "LTC2", 2);
    ltc_sheen = new bgq_opengl::LTCMatrix(3, "SHEENCOEFFS", 3);

    // Load the textures.
    textures.push_back(bgq_opengl::Texture("table_basecolor.png", "material.diffuse", 4, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
    textures.push_back(bgq_opengl::Texture("table_normal.png", "material.normalmap", 5, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
    textures.push_back(bgq_opengl::Texture("table_specular.png", "material.specular", 6, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
    textures.push_back(bgq_opengl::Texture("machine_basecolor.png", "material.diffuse", 4, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
    textures.push_back(bgq_opengl::Texture("machine_normal.png", "material.normalmap", 5, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
    textures.push_back(bgq_opengl::Texture("machine_specular.png", "material.specular", 6, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR));

    // Load the objects.
    scene_1.push_back(bgq_opengl::Object("cloth.glb", "Assimp"));
    scene_1.push_back(bgq_opengl::Object("sphere.glb", "Assimp"));

    scene_2.push_back(bgq_opengl::Object("fabric_front.glb", "Assimp"));
    scene_2.push_back(bgq_opengl::Object("table.glb", "Assimp"));
    scene_2.push_back(bgq_opengl::Object("sewing.glb", "Assimp"));

}

void initEnvironment(int argc, char** argv) {
    
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        exit(1);

    }
    
    // MacOS initializations.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create the window.
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME, NULL, NULL);
    if (!window) {
        
        std::cerr << "Error 121-1001 - Failed to create the window." << std::endl;
        glfwTerminate();
        exit(1);

    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW and OpenGL.
    GLenum res = glewInit();

    // Check for any errors.
    if (res != GLEW_OK) {

        std::cerr << "Error 121-1002 - GLEW could not be initialized:" << glewGetErrorString(res) << std::endl;
        exit(1);

    }
    
    // Get version info.
    std::cerr << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "GLSL version supported " << (char *) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
}

int main(int argc, char** argv) {

	// Initialise the environment.
    initEnvironment(argc, argv);
    
	// Initialise the objects and elements.
	initElements();

	// Main loop.
    while(!glfwWindowShouldClose(window)) {
        
        // Clear the scene.
        clear();
        
        // Get the window size.
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        camera->setWidth(width);
        camera->setHeight(height);
        
        // Get the current time.
        internal_time = glfwGetTime();
        
        // If the loop has been restarted, set the starting time.
        if (fps_counted == 0) {
            
            fps_time = internal_time;
            
        }

        // Handle key events.
        handleKeyEvents();
        
        // Display the scene.
        displayElements();
        
        // Make the things to print everything.
        displayGUI();
        
        // Increment the FPS count in one.
        fps_counted++;
        
        if (fps_counted >= FPS_STEP) {
            
            // Get the elapsed time.
            double fps_elapsed_time = internal_time - fps_time;
            
            // Calculate the fps.
            fps = fps_counted / fps_elapsed_time;
            
            // Restart the loop.
            fps_counted = 0;
            
            // Print it.
            std::cout << "FPS: " << fps << std::endl;
            
        }
        
        // Update other events like input handling.
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }

	// Clean everything and terminate.
	clean();

}
