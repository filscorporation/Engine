#include <GLAD/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "BuiltInShaders.h"
#include "Screen.h"
#include "SpriteRenderer.h"
#include "../Core/Application.h"

const int RENDER_CALL_DATA_SIZE = 10;
const int MAX_RENDER_CALLS = 10000;
const int MAX_TEXTURE_SLOTS = 32;

Shader* shader;

int renderCallsCount;
int texturesCount;
glm::vec4 defaultVertexPositions[4];
glm::vec2 defaultTextureCoords[4];
GLfloat* vertexBufferData;
GLuint textureIDs[MAX_TEXTURE_SLOTS];
GLuint indexBufferID, vertexDataBufferID;
GLuint viewProjectionUniform;
GLuint drawModeUniform;

DrawModes::DrawMode Renderer::currentDrawMode = DrawModes::Normal;
bool Renderer::DrawWireframe = false;
int Renderer::DrawCallsStats = 0;
int Renderer::VerticesStats = 0;

void Renderer::Init()
{
    Log::LogInfo("Begin renderer init");

    if(!gladLoadGL())
    {
        Log::LogError("Error loading OpenGL");
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create shader and initialize it's texture slots variable
    shader = new Shader(BuiltInShaders::VertexShader, BuiltInShaders::FragmentShader);
    shader->Use();
    GLint textureSlots[MAX_TEXTURE_SLOTS];
    for (GLint i = 0; i < MAX_TEXTURE_SLOTS; i++)
        textureSlots[i] = i;
    glUniform1iv(glGetUniformLocation(shader->Program, "images"), MAX_TEXTURE_SLOTS, textureSlots);

    Log::LogInfo("Shader created");

    // Camera transformation uniform
    viewProjectionUniform = glGetUniformLocation(shader->Program, "view_projection");
    // Draw mode
    drawModeUniform = glGetUniformLocation(shader->Program, "draw_mode");

    Log::LogInfo("Uniforms saved");

    // Generate default vertex positions
    defaultVertexPositions[0] = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    defaultVertexPositions[1] = glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    defaultVertexPositions[2] = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    defaultVertexPositions[3] = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);

    // Generate default texture coords
    defaultTextureCoords[0] = glm::vec2(1.0f, 0.0f);
    defaultTextureCoords[1] = glm::vec2(1.0f, 1.0f);
    defaultTextureCoords[2] = glm::vec2(0.0f, 0.0f);
    defaultTextureCoords[3] = glm::vec2(0.0f, 1.0f);

    // Reserve space for all vertex data
    vertexBufferData = new GLfloat[MAX_RENDER_CALLS * RENDER_CALL_DATA_SIZE * 4];
    glGenBuffers(1, &vertexDataBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexDataBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAX_RENDER_CALLS * RENDER_CALL_DATA_SIZE * 4, nullptr, GL_DYNAMIC_DRAW);

    Log::LogInfo("Vertices data generated");

    // Generate all vertex indices
    auto indexes = new GLuint[MAX_RENDER_CALLS * 6];
    int indexOffset = 0;
    for (int i = 0; i < MAX_RENDER_CALLS * 6; i += 6)
    {
        indexes[i + 0] = indexOffset + 0;
        indexes[i + 1] = indexOffset + 1;
        indexes[i + 2] = indexOffset + 2;

        indexes[i + 3] = indexOffset + 1;
        indexes[i + 4] = indexOffset + 2;
        indexes[i + 5] = indexOffset + 3;

        indexOffset += 4;
    }
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MAX_RENDER_CALLS * 6, indexes, GL_STATIC_DRAW);
    delete[] indexes;

    Log::LogInfo("Indexes generated");

    Log::LogInfo("Renderer initialized");
}

void Renderer::Terminate()
{
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &vertexDataBufferID);

    delete[] vertexBufferData;
    delete shader;
}

void Renderer::OnBeforeRender(Camera& camera)
{
    // Set camera transformation
    glm::mat4 viewProjection = camera.GetViewProjection();
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));

    SetDrawMode(DrawModes::Normal);

    // Start first batch
    StartBatch();

    DrawCallsStats = 0;
    VerticesStats = 0;
}

void Renderer::OnAfterRender()
{
    EndBatch();
}

void Renderer::Clear(glm::vec3 color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::PrepareUIRender()
{
    EndBatch();
    StartBatch();

    glm::mat4 uiViewProjection = Screen::GetUIViewProjection();
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(uiViewProjection));
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawQuad(glm::vec3 vertices[4], glm::vec2 textureCoords[4], const glm::vec4& color, uint32_t textureID)
{
    int textureIDIndex = FindTextureSlot(textureID);

    int offset = renderCallsCount * RENDER_CALL_DATA_SIZE * 4;
    for (int i = 0; i < 4; ++i)
    {
        vertexBufferData[offset++] = vertices[i][0];
        vertexBufferData[offset++] = vertices[i][1];
        vertexBufferData[offset++] = vertices[i][2];
        vertexBufferData[offset++] = color[0];
        vertexBufferData[offset++] = color[1];
        vertexBufferData[offset++] = color[2];
        vertexBufferData[offset++] = color[3];
        vertexBufferData[offset++] = textureCoords[i][0];
        vertexBufferData[offset++] = textureCoords[i][1];
        vertexBufferData[offset++] = (GLfloat)textureIDIndex;
    }

    renderCallsCount++;
    VerticesStats += 4;
}

void Renderer::SetDrawMode(DrawModes::DrawMode drawMode)
{
    if (currentDrawMode == drawMode)
        return;

    currentDrawMode = drawMode;
    glUniform1i(drawModeUniform, (GLint)currentDrawMode);
}

void Renderer::StartBatch()
{
    renderCallsCount = 0;
    texturesCount = 0;
}

void Renderer::EndBatch()
{
    if (renderCallsCount == 0)
        return;

    DrawCallsStats ++;
    DrawBatchedData();

    if (DrawWireframe)
    {
        // Wireframe mode
        auto drawModeBackup = currentDrawMode;
        SetDrawMode(DrawModes::Wireframe);

        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        DrawBatchedData();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);

        SetDrawMode(drawModeBackup);
    }
}

void Renderer::DrawBatchedData()
{
    // Generate vertex array
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Set vertices data
    glBindBuffer(GL_ARRAY_BUFFER, vertexDataBufferID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * renderCallsCount * RENDER_CALL_DATA_SIZE * 4, vertexBufferData);

    // Bind indexes buffer and define attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * RENDER_CALL_DATA_SIZE, (GLvoid*)nullptr);
    glEnableVertexAttribArray(0);
    // Vertex color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * RENDER_CALL_DATA_SIZE, (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * RENDER_CALL_DATA_SIZE, (GLvoid*)(7 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // Texture ID
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * RENDER_CALL_DATA_SIZE, (GLvoid*)(9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind textures
    for (int i = 0; i < texturesCount; ++i)
    {
        GLenum activeTexture = GL_TEXTURE0 + i;
        glActiveTexture(activeTexture);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
    }

    // Draw
    glDrawElements(GL_TRIANGLES, renderCallsCount * 6, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteVertexArrays(1, &vertexArrayID);
}

int Renderer::FindTextureSlot(uint32_t textureID)
{
    if (renderCallsCount >= MAX_RENDER_CALLS)
    {
        EndBatch();
        StartBatch();
    }

    int textureIDIndex = -1;
    for (int i = 0; i < texturesCount; ++i)
    {
        if (textureIDs[i] == textureID)
        {
            textureIDIndex = i;
            break;
        }
    }

    if (textureIDIndex == -1)
    {
        if (texturesCount >= MAX_TEXTURE_SLOTS)
        {
            EndBatch();
            StartBatch();
        }

        textureIDIndex = texturesCount;
        textureIDs[textureIDIndex] = textureID;
        texturesCount++;
    }

    return textureIDIndex;
}
