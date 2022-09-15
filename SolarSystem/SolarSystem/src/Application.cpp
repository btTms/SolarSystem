#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

Sphere* sphere1 = new Sphere(1.0f, 44, 30);

void processInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

    // ------- IMGUI -------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    Shader shader("res/shaders/vertex_shader.shader", "res/shaders/fragment_shader.shader");

    unsigned int sphereVAO;
    glGenVertexArrays(1, &sphereVAO);

    unsigned int sphereVBO;
    glGenBuffers(1, &sphereVBO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere1->getInterleavedVertexSize(), sphere1->getInterleavedVertices(), GL_STATIC_DRAW);

    glBindVertexArray(sphereVAO);

    int stride = sphere1->getInterleavedStride();

    // position attribute:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // normal attributes:
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinates:
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int sphereEBO; // EBO for the sphere itself
    glGenBuffers(1, &sphereEBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere1->getIndexSize(), sphere1->getIndices(), GL_STATIC_DRAW);


    bool show_demo_window = true;
    bool show_another_window = false;
    bool show_sphere = true;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    float x_rot = 0.0f;
    float y_rot = 1.0f;
    float z_rot = 0.0f;
    float speed = 50.0f;

    unsigned int earth_texture = sphere1->loadTexture("res/textures/earth_daymap.jpg", true);

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

        unsigned int color_loc = glGetUniformLocation(shader.ID, "color");
        glUniform3fv(color_loc, 1, glm::value_ptr(color));

        // view /& projection transformations:
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // world transfgormation:
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(speed), glm::vec3(x_rot, y_rot, z_rot));

        glm::mat4 mvp = projection * view * model;

        unsigned int mvp_loc = glGetUniformLocation(shader.ID, "mvp");
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindTexture(GL_TEXTURE_2D, earth_texture);

        glBindVertexArray(sphereVAO);

        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);

        if(show_sphere)
            glDrawElements(GL_TRIANGLES, sphere1->getIndexCount(), GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        if (mouseIsVisible && show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {


            ImGui::Begin("Solar System Menu");                          

            ImGui::Checkbox("Demo Window", &show_demo_window);    
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::Checkbox("Show sphere?", &show_sphere);

            ImGui::SliderFloat("x_rot", &x_rot, -1.0f, 1.0f);           
            ImGui::SliderFloat("y_rot", &y_rot, -1.0f, 1.0f);           
            ImGui::SliderFloat("z_rot", &z_rot, -1.0f, 1.0f);           
            ImGui::SliderFloat("speed", &speed, 50.0f, 200.0f);           
            ImGui::ColorEdit3("clear color", (float*)&color); // Edit 3 floats representing a color

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

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);

    sphere1 = NULL;

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
