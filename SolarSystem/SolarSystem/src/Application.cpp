#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>

#include "Sphere.h"
#include "Shader.h"
#include "Camera.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float dist_x = 0;
float dist_y = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
bool previousState = false;
bool mouseIsVisible = false;

std::vector<float> vertices;
std::vector<unsigned int> indices;
std::vector<unsigned int> textures(9, 0);

std::vector<std::string> texture_paths = {
    "res/textures/sun.jpg",
    "res/textures/mercury.jpg",
    "res/textures/venus_surface.jpg",
    "res/textures/venus_atmosphere.jpg",
    "res/textures/earth_daymap.jpg",
    "res/textures/earth_clouds.jpg",
    "res/textures/mars.jpg",
    "res/textures/jupiter.jpg",
    "res/textures/saturn.jpg",
    "res/textures/uranus.jpg",
    "res/textures/neptune.jpg",

};


Sphere sphere1((float)69.6340, 36, 18, 0, indices, vertices);

void processInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void scaleModelMatrices(glm::mat4* planetsModelMatrices);

void computeRotation(glm::mat4* planetsModelMatrices);

void initializeTextureArray(unsigned int& textureArray);

void addTextureToTextureArrays(std::vector<std::string> texture_paths, unsigned int& textureArray);

void generateTextureArraysMipMaps(unsigned int& textureArray);

void allignPlanets(glm::mat4* planetsModelMatrices);


#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall())

static void GLClearError() {

    while (glGetError() != GL_NO_ERROR);

}

static bool GLLogCall() {

    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] ( " << error << " )\n";

        return false;
    }

    return true;

}



int main(void){

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);

    if (!window){
        glfwTerminate();
        std::cout << "FAILED TO CREATE GLFW WINDOW!\n";
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // enable vsync

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::cout << "FAILED TO INITIALIZE GLEW\n";
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // mouse capturing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //unsigned int earth_texture = sphere1->loadTexture("res/textures/sun.jpg", true);

    // ------- IMGUI -------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    Shader shader("res/shaders/vertex_shader.shader", "res/shaders/fragment_shader.shader");

    shader.use();


    int stride = 32;

    unsigned int planetsVAO;
    glGenVertexArrays(1, &planetsVAO);

    glBindVertexArray(planetsVAO);

    // Vertex buffer of the planets:
    unsigned int planetsVBO;
    glGenBuffers(1, &planetsVBO);

    glBindBuffer(GL_ARRAY_BUFFER, planetsVBO);
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    

    // position attributes:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // normal attributes:
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Index buffer of the planets:
    unsigned int planetsEBO;
    glGenBuffers(1, &planetsEBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetsEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


    // translate every planet to its right starting coordinate (basically their distance from the Sun)
    glm::mat4* planetsModelMatrices = new glm::mat4[9];

    planetsModelMatrices[0] = glm::mat4(1.0f);
    //computeRotation(planetsModelMatrices);

    allignPlanets(planetsModelMatrices);
    

    unsigned int planetsModelMatrixBuffer;
    glGenBuffers(1, &planetsModelMatrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planetsModelMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(glm::mat4), &planetsModelMatrices[0], GL_STATIC_DRAW);
    

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);


    glBindVertexArray(0);


    unsigned int textureArray;

    initializeTextureArray(textureArray);

    addTextureToTextureArrays(texture_paths, textureArray);

    generateTextureArraysMipMaps(textureArray);


    while (!glfwWindowShouldClose(window))
    {
        // delta time:
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
                
        // input:
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

        glUniform1i(glGetUniformLocation(shader.ID, "planetTexture"), 0);

        //computeRotation(planetsModelMatrices);

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 10000.0f);

        unsigned int view_loc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int proj_loc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(planetsVAO);

        glBindBuffer(GL_ARRAY_BUFFER, planetsVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetsEBO);
        glBindBuffer(GL_ARRAY_BUFFER, planetsModelMatrixBuffer);

        //glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(glm::mat4), &planetsModelMatrices[0], GL_STATIC_DRAW);

        glDrawElementsInstanced(GL_TRIANGLES, unsigned int(indices.size()), GL_UNSIGNED_INT, 0, 9);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        {

            ImGui::Begin("Solar System Menu");                          
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // render:
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // swap buffers and poll events:
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &planetsVAO);                                   
    glDeleteBuffers(1, &planetsVBO);                     
    glDeleteBuffers(1, &planetsEBO);                                   


    // Cleanup                   
    ImGui_ImplOpenGL3_Shutdown();                                                               
    ImGui_ImplGlfw_Shutdown();                                                                                                                       
    ImGui::DestroyContext();                             
                     
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseIsVisible = false;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseIsVisible = true;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !mouseIsVisible)
        camera.ProcessKeyboard(DOWN, deltaTime);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);


    if (!mouseIsVisible) {

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        if (previousState) {
            dist_x = xpos - lastX;
            dist_y = ypos - lastY;
            previousState = false;
        }


        float xoffset = (xpos - dist_x) - lastX;
        float yoffset = lastY - (ypos - dist_y);

        lastX = xpos - dist_x;
        lastY = ypos - dist_y;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else {
        previousState = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    if (!mouseIsVisible) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}


void scaleModelMatrices(glm::mat4* planetsModelMatrices) {

    planetsModelMatrices[1] = glm::scale(planetsModelMatrices[1], glm::vec3(0.009503f));

    planetsModelMatrices[2] = glm::scale(planetsModelMatrices[2], glm::vec3(0.0146908f));

    planetsModelMatrices[3] = glm::scale(planetsModelMatrices[3], glm::vec3(0.0151492f));

    planetsModelMatrices[4] = glm::scale(planetsModelMatrices[4], glm::vec3(0.010867f));

    planetsModelMatrices[5] = glm::scale(planetsModelMatrices[5], glm::vec3(0.103988f));

    planetsModelMatrices[6] = glm::scale(planetsModelMatrices[6], glm::vec3(0.08362581f));

    planetsModelMatrices[7] = glm::scale(planetsModelMatrices[7], glm::vec3(0.036421f));

    planetsModelMatrices[8] = glm::scale(planetsModelMatrices[8], glm::vec3(0.035359f));

}

void computeRotation(glm::mat4* planetsModelMatrices) {

    glm::vec3 pos(0.0f);

    pos.x = (float)sin(glfwGetTime());
    pos.z = (float)cos(glfwGetTime());

    planetsModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (3.5 + 69.6340 + 0.24397 + 25),   0.0f, pos.z * (3.5 + 69.6340 + 0.24397 + 25)));

    planetsModelMatrices[2] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (6.7 + 69.6340 + 0.60518 + 25),   0.0f, pos.z * (6.7 + 69.6340 + 0.60518 + 25)));

    planetsModelMatrices[3] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (9.3f + 69.6340 + 0.6371 + 25),   0.0f, pos.z * (9.3f + 69.6340 + 0.6371 + 25)));

    planetsModelMatrices[4] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (14.2f + 69.6340 + 0.33895 + 25), 0.0f, pos.z * (14.2f + 69.6340 + 0.33895 + 25)));

    planetsModelMatrices[5] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (48.4f + 69.6340 + 6.9911f + 25), 0.0f, pos.z * (48.4f + 69.6340 + 6.9911f + 25)));

    planetsModelMatrices[6] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (88.9f + 69.6340 + 5.8232 + 25),  0.0f, pos.z * (88.9f + 69.6340 + 5.8232 + 25)));

    planetsModelMatrices[7] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (179.0f + 69.6340 + 2.5362 + 25), 0.0f, pos.z * (179.0f + 69.6340 + 2.5362 + 25)));

    planetsModelMatrices[8] = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * (288.0f + 69.6340 + 2.4622 + 25), 0.0f, pos.z * (288.0f + 69.6340 + 2.4622 + 25)));

    scaleModelMatrices(planetsModelMatrices);

}

void allignPlanets(glm::mat4* planetsModelMatrices) {

    planetsModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3((3.5 + 69.6340 + 0.24397 + 25), 0.0f, (3.5 + 69.6340 + 0.24397 + 25)));

    planetsModelMatrices[2] = glm::translate(glm::mat4(1.0f), glm::vec3((6.7 + 69.6340 + 0.60518 + 25), 0.0f, (6.7 + 69.6340 + 0.60518 + 25)));

    planetsModelMatrices[3] = glm::translate(glm::mat4(1.0f), glm::vec3((9.3f + 69.6340 + 0.6371 + 25), 0.0f, (9.3f + 69.6340 + 0.6371 + 25)));

    planetsModelMatrices[4] = glm::translate(glm::mat4(1.0f), glm::vec3((14.2f + 69.6340 + 0.33895 + 25), 0.0f, (14.2f + 69.6340 + 0.33895 + 25)));

    planetsModelMatrices[5] = glm::translate(glm::mat4(1.0f), glm::vec3((48.4f + 69.6340 + 6.9911f + 25), 0.0f, (48.4f + 69.6340 + 6.9911f + 25)));

    planetsModelMatrices[6] = glm::translate(glm::mat4(1.0f), glm::vec3((88.9f + 69.6340 + 5.8232 + 25), 0.0f, (88.9f + 69.6340 + 5.8232 + 25)));

    planetsModelMatrices[7] = glm::translate(glm::mat4(1.0f), glm::vec3((179.0f + 69.6340 + 2.5362 + 25), 0.0f, (179.0f + 69.6340 + 2.5362 + 25)));

    planetsModelMatrices[8] = glm::translate(glm::mat4(1.0f), glm::vec3((288.0f + 69.6340 + 2.4622 + 25), 0.0f, (288.0f + 69.6340 + 2.4622 + 25)));

    scaleModelMatrices(planetsModelMatrices);

}



void initializeTextureArray(unsigned int& textureArray) {

    std::cout << "Initializing texture array\n";


    GLCall(glGenTextures(1, &textureArray));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray));

    GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGB8, 2048, 1024, 12));

    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    std::cout << "Done initializing texture array\n";
}

void addTextureToTextureArrays(std::vector<std::string> texture_paths, unsigned int& textureArray) {

    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    for (unsigned int i = 0; i <= 10; i++) {

        unsigned int texture;

        GLCall(glGenTextures(1, &texture));

        glBindTexture(GL_TEXTURE_3D, texture); 

        int width = 0, height = 0, nrChannels = 0;

        GLCall(stbi_set_flip_vertically_on_load(true));
        unsigned char* data = stbi_load(texture_paths[i].c_str(), &width, &height, &nrChannels, 0);

        std::cout << "width " << width << "height " << height << "\n";

        if (data){
            GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data));
        }
        else{
            std::cout << "Failed to load " << texture_paths[i] << "\n";
        }

        stbi_image_free(data);
       
    }

}

void generateTextureArraysMipMaps(unsigned int& textureArray) {

    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

    GLCall(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

}


