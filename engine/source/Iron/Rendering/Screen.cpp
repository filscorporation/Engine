#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Screen.h"
#include "../Core/Application.h"
#include "../Core/Input.h"
#include "../Core/Log.h"

GLFWwindow* _window;
bool needToUpdateViewport = false;
bool screenSizeDirty = false;

Framebuffer* Screen::_framebuffer = nullptr;
int Screen::_width, Screen::_height;
int Screen::_xPosition, Screen::_yPosition;
glm::vec3 Screen::_color;
glm::mat4 Screen::_viewProjection = glm::mat4(1.0f);
bool Screen::_fullscreen;
bool Screen::_isMinimized = false;
bool Screen::_doubleBuffer = true;
bool Screen::isInResizeCallback = false;
bool Screen::_isEmulated = false;
int Screen::_fakeWidth, Screen::_fakeHeight;

// TODO: currently window will not update if you held it still
// (possible solution is to separate poll_events and rendering in different threads)
void ResizeCallback(GLFWwindow* window, int cx, int cy);
void PositionCallback(GLFWwindow* window, int cx, int cy);

Framebuffer* Screen::ScreenFramebuffer()
{
    return _framebuffer;
}

void Screen::Apply()
{
    if (_fullscreen)
    {
        glfwGetWindowPos(_window, &_xPosition, &_yPosition);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        glfwSetWindowMonitor(_window, nullptr, _xPosition, _yPosition, _width, _height, 0);
    }

    needToUpdateViewport = true;
}

int Screen::GetWidth()
{
    if (_isEmulated)
        return _fakeWidth;

    if (_fullscreen)
    {
        int tempWidth, tempHeight;
        glfwGetFramebufferSize(_window, &tempWidth, &tempHeight);

        return tempWidth;
    }

    return _width;
}

void Screen::SetWidth(int width)
{
    if (_width == width)
        return;

    _width = width;
    EnterCallback();
    Apply();
    ExitCallback();
}

int Screen::GetHeight()
{
    if (_isEmulated)
        return _fakeHeight;

    if (_fullscreen)
    {
        int tempWidth, tempHeight;
        glfwGetFramebufferSize(_window, &tempWidth, &tempHeight);

        return tempHeight;
    }

    return _height;
}

void Screen::SetHeight(int height)
{
    if (_height == height)
        return;

    _height = height;
    EnterCallback();
    Apply();
    ExitCallback();
}

bool Screen::GetFullscreen()
{
    return _fullscreen;
}

void Screen::SetFullscreen(bool fullscreen)
{
    if (_fullscreen == fullscreen)
        return;

    _fullscreen = fullscreen;
    EnterCallback();
    Apply();
    ExitCallback();
}

bool Screen::GetIsMinimized()
{
    return _isMinimized;
}

glm::vec3 Screen::GetColor()
{
    return _color;
}

void Screen::SetColor(glm::vec3 color)
{
    if (_color == color)
        return;

    _color = color;
}

glm::mat4 Screen::GetUIViewProjection()
{
    return _viewProjection;
}

bool Screen::IsScreenSizeDirty()
{
    return screenSizeDirty;
}

void Screen::UpdateUIViewProjection(int width, int height)
{
    glm::mat4 projection = glm::ortho(
            0.0f, (float)width,
            0.0f, (float)height,
            -1.0f, UI_MAX_DISTANCE
    );

    _viewProjection = projection;
}

void Screen::Init(int width, int height, glm::vec3 color, bool fullscreen, bool doubleBuffer)
{
    _fullscreen = fullscreen;
    _doubleBuffer = doubleBuffer;
    _color = color;

    if (!glfwInit())
    {
        Log::LogError("Error initializing GLFW");
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, _doubleBuffer);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    if (_fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        _window = glfwCreateWindow(mode->width, mode->height, "Application", monitor, nullptr);
    }
    else
    {
        _window = glfwCreateWindow(width, height, "Application", nullptr, nullptr);
    }
    glfwMakeContextCurrent(_window);

    glfwGetWindowPos(_window, &_xPosition, &_yPosition);
    glfwGetFramebufferSize(_window, &_width, &_height);
    glViewport(0, 0, _width, _height);

    glfwSetWindowSizeCallback(_window, ResizeCallback);
    glfwSetWindowPosCallback(_window, PositionCallback);

    UpdateUIViewProjection(_width, _height);

    _framebuffer = new Framebuffer();

    Log::LogDebug("Initialized screen with size {0}:{1}", _width, _height);

    Input::Init(_window);
}

void ResizeCallback(GLFWwindow* window, int cx, int cy)
{
    if (!Screen::IsInCallback())
    {
        needToUpdateViewport = true;
        screenSizeDirty = true;
        Screen::EnterCallback();
        Application::Instance->RunUpdate();
        Screen::ExitCallback();
    }
}

void PositionCallback(GLFWwindow* window, int cx, int cy)
{
    if (!Screen::IsInCallback())
    {
        needToUpdateViewport = true;
        Screen::EnterCallback();
        Input::ReleaseAllEvents();
        Application::Instance->RunUpdate();
        Screen::ExitCallback();
    }
}

void Screen::UpdateSize()
{
    if (needToUpdateViewport)
    {
        needToUpdateViewport = false;
        int tempWidth, tempHeight;
        int tempX, tempY;
        glfwGetFramebufferSize(_window, &tempWidth, &tempHeight);
        glfwGetWindowPos(_window, &tempX, &tempY);
        if (!_fullscreen)
        {
            _width = tempWidth;
            _height = tempHeight;
            _xPosition = tempX;
            _yPosition = tempY;

            _isMinimized = _width == 0 && _height == 0;
        }
        glViewport(0, 0, tempWidth, tempHeight);

        Input::ReleaseAllEvents();
        UpdateUIViewProjection(_width, _height);
        Application::Instance->GetCurrentScene()->GetMainCamera().UpdateSize();
    }
}

void Screen::SwapBuffers()
{
    if (_doubleBuffer)
        glfwSwapBuffers(_window);
    else
        glFlush();

    screenSizeDirty = false;
}

bool Screen::WindowShouldClose()
{
    return glfwWindowShouldClose(_window);
}

void Screen::Terminate()
{
    delete _framebuffer;

    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Screen::StartEmulate(int width, int height)
{
    _fakeWidth = width;
    _fakeHeight = height;
    glViewport(0, 0, _fakeWidth, _fakeHeight);
    UpdateUIViewProjection(_fakeWidth, _fakeHeight);
    _isEmulated = true;
}

void Screen::StopEmulate()
{
    glViewport(0, 0, _width, _height);
    UpdateUIViewProjection(_width, _height);
    _isEmulated = false;
}

glm::vec2 Screen::GetRealSize()
{
    if (_fullscreen)
    {
        int tempWidth, tempHeight;
        glfwGetFramebufferSize(_window, &tempWidth, &tempHeight);

        return { tempWidth, tempHeight };
    }

    return { _width, _height };
}

bool Screen::IsInCallback()
{
    return isInResizeCallback;
}

void Screen::EnterCallback()
{
    isInResizeCallback = true;
}

void Screen::ExitCallback()
{
    isInResizeCallback = false;
}
