#define GLEW_STATIC

#include <GL/glew.h>
#include <easylogging++.h>
#include "Window.h"
#include "EmptyScene.h"
#include "GLFWHandler.h"
#include "ReloadLocator.h"
#include "AppleLiveReloadManager.h"
#include "GL_Logger.h"
#include <GLFW/glfw3.h>
#include "ToolScene.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

INITIALIZE_EASYLOGGINGPP

int main(int argC, char ** argV)
{
   /**
    * Setup the current environment
    */
   START_EASYLOGGINGPP(argC, argV);
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   GLFWwindow * window = glfwCreateWindow(800,600, "CDTI Display", nullptr, nullptr);

   if (window == nullptr)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glViewport(0, 0, width, height);

   glewExperimental = GL_TRUE;
   if(glewInit() != GLEW_OK)
   {
      std::cout << "Failed to initialize GLEW" << std::endl;
      return -1;
   }
   GL_Logger::LogError("Error in GLEW startup (Safe to ignore)", glGetError());

   ImGui_ImplGlfwGL3_Init(window,false); //Initialize ImGui

   glfwSetKeyCallback(window, GLFWHandler::key_callback);
   glfwSetCursorPosCallback(window, GLFWHandler::mousePositionCallback);
   glfwSetMouseButtonCallback(window, GLFWHandler::mouseButtonCallback);
   glfwSetScrollCallback(window, GLFWHandler::scrollWheelCallback);
   glfwSetCharCallback(window,GLFWHandler::characterCallback);

   FileSystem::ReloadLocator::Initialize();
   FileSystem::ReloadLocator::provide(new AppleReloadManager());


   Context * ctx = new Context(window);
   //Init scene of choice here
   Scene * scene = new ToolScene(ctx);
   scene->setContext(ctx);

   //Create a window for the program to run in
   Window renderWindow(window);

   renderWindow.loadScene(scene);
   renderWindow.run();

   delete scene;
   delete ctx;
   return 0;
}
