#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Input.h"
#include "Scene.h"

#include "SpriteRenderer.h"

bool isRunning;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLuint WIDTH = 800, HEIGHT = 600;

// TODO: don't Use globals
double lastFrameTime = 0;
float deltaTime = 0.0f;

GLFWwindow* window;

Scene scene;

Application::Application()
{
    std::cout << "Application initialized" << std::endl;
}

void Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    Input::Init(window);

    gladLoadGL();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    Object object;
    auto *sr = object.AddComponent<SpriteRenderer>();
    sr->SpriteName = "../resources/test.png";
    scene.Objects.push_back(object);
}

void Application::Run()
{
    std::cout << "Running application" << std::endl;

    Init();

    isRunning = true;

    while (isRunning)
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering objects in scene

        for (auto &object : scene.Objects)
        {
            for (auto &component : object.Components())
            {
                component->OnUpdate();
                component->OnRender();
            }
        }

        glfwSwapBuffers(window);

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        if (Input::IsKeyPressed(Escape))
            isRunning = false;

        if (glfwWindowShouldClose(window))
            isRunning = false;
    }
    glfwTerminate();
}

void Application::AddObjectToScene(Object object)
{
    scene.Objects.push_back(object);
}