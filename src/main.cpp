/*
  Compilation on Linux
  g++ -std=c++17 ./src/* -o prog -I ./include/ -I ./thirdparty/glm-master/ -lSDL2 -ldl -lassimp
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ Standard Template Library (STL)
#include <iostream>

// Personal libraries
#include <Shader.hpp>
#include <Camera.hpp>
#include <model.h>

// #################### vvv Globals vvv ####################
// Globals are prefixed with 'g'

// Screen dimensions
int gScreenWidth = 800;
int gScreenHeight = 600;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Main loop flag
bool gQuit = false; // If this is true then the program terminates
// #################### ^^^ Globals ^^^ ####################

// #################### vvv Error handling routines vvv ####################
static void GLClearAllErrors()
{
  while(glGetError() != GL_NO_ERROR)
  {
    
  }
}

// Returns true if we have an error
static bool GLCheckErrorStatus(const char* function, int line)
{
  while (GLenum error = glGetError())
  {
    std::cout << "OpenGL Error:" << error
	      << "\tLine: " << line
	      << "\tfunction: " << function << std::endl;
    return true;
  }

  return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);
// #################### ^^^ Error handling routines ^^^ ####################

/*
  Helper function to get OpenGL Version Information
*/
void GetOpenGLVersionInfo()
{
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

/*
  Initialization of the graphics application. Typically this will involve setting up a window
  and the OpenGL Context (with the appropriate version)
  
  @return void
*/
void InitializeProgram()
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cout << "SDL2 could not initialize video subsystem" << std::endl;
    exit(1);
  }
  // Setup the OpenGL Context
  // Use OpenGL 4.1 core or greater
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  // We want to request a double buffer for smooth updating
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Create an application window using OpenGL that supports SDL
  gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  // Check if Window did not create
  if (gGraphicsApplicationWindow == nullptr)
  {
    std::cout << "SDL Window was not able to be created" << std::endl;
    exit(1);
  }

  // Create an OpenGL Graphics Context
  gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

  if (gOpenGLContext == nullptr)
  {
    std::cout << "OpenGL context could not be created" << std::endl;
    exit(1);
  }

  // Initialize the Glad Library
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
  {
    std::cout << "glad was not initialized" << std::endl;
    exit(1);
  }

  GetOpenGLVersionInfo();
}

/*
  // Function called in the main application loop to handle user input
  
  @return void
*/
void Input()
{
  // Event handler that handles various events in SDL
  // that are related to input and output

  SDL_Event e;
  // Handle events on queue
  while(SDL_PollEvent(&e) != 0)
  {
    // If user posts an event to quit
    // An example is hitting the "x" in the corner of the window
    switch (e.type)
    {
      case SDL_QUIT:
      std::cout << "Goodbye!" << std::endl;
      gQuit = true;
      break;

      case SDL_MOUSEWHEEL:
        camera.ProcessMouseScroll(e.wheel.y);
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) 
        {
          SDL_RaiseWindow(gGraphicsApplicationWindow);
          SDL_SetRelativeMouseMode(SDL_TRUE);
        }    
        break;

      case SDL_MOUSEMOTION:
        float mouseX = (float) e.motion.xrel;
        float mouseY = (float) e.motion.yrel;
        camera.ProcessMouseMovement(mouseX, -mouseY);
        break;
      }
   }

  const Uint8* state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
  if (state[SDL_SCANCODE_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (state[SDL_SCANCODE_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
  if (state[SDL_SCANCODE_A]) camera.ProcessKeyboard(LEFT, deltaTime);
}

void CleanUp()
{
  SDL_DestroyWindow(gGraphicsApplicationWindow);
  SDL_Quit();
}

/*
  The entry point into a program
  @return program status
*/
int main( int argc, char* args[] )
{
  
  // 1. Setup the graphics program
  InitializeProgram();

  stbi_set_flip_vertically_on_load(true);

  // configure opengl global state
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  Shader ourShader("../../shaders/model-loading-vs.glsl", "../../shaders/model-loading-fs.glsl");
  Shader lightCubeShader("../../shaders/light-cube-vs.glsl","../../shaders/light-cube-fs.glsl");

  // load models
  std::string path = "../../resources/objects/backpack/backpack.obj";
  Model ourModel(path);

  // render loop
  while (!gQuit)
  {
    
    float currentFrame = static_cast<float>(SDL_GetTicks());
    std::cout << "GET TICKS: " << currentFrame << std::endl;
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    // Handle input 
    Input();

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)gScreenWidth / (float)gScreenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix(); 
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    // render loaded model
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    // Update screen of our specified window
    SDL_GL_SwapWindow(gGraphicsApplicationWindow);


  }

  // 5. Call the cleanup funcion when our program terminates
  CleanUp();

  return 0;
  
}
