#include "ToolScene.h"
#include "FramebufferConfiguration.h"
#include "Mouse.h"
ToolScene::ToolScene(Context * ctx):
   Scene(ctx),
   grid(4.f,4.f,6,6),
   modelMgr("assets/models"),
   programMgr("assets/shaders")

{
   /**Create the camera*/
   camera = std::unique_ptr<TabletopCamera>( new TabletopCamera(
         ctx->getWindowWidth(),
         ctx->getWindowHeight()));
   modelMgr.load("assets/models/bunny.obj");
   glEnable(GL_DEPTH_TEST);
   
   /**
    * Initialize the G-Buffer + depth map for
    * easy querying.
    */
   FramebufferConfiguration cfg(ctx->getWindowWidth(),ctx->getWindowHeight());
   TextureConfig posBfrConfig("position",GL_RGB16F,GL_RGB,GL_FLOAT);
   posBfrConfig.setTextureFilter(GL_NEAREST);
   TextureAttachment posBfr(posBfrConfig,GL_COLOR_ATTACHMENT0);

   TextureConfig norBfrConfig("normal",GL_RGB16F,GL_RGB,GL_FLOAT);
   posBfrConfig.setTextureFilter(GL_NEAREST);

   TextureAttachment norBfr(norBfrConfig,GL_COLOR_ATTACHMENT1);

   TextureConfig texcoordConfig("texCoords",GL_RGB16F,GL_RGB,GL_FLOAT);
   posBfrConfig.setTextureFilter(GL_NEAREST);
   TextureAttachment texcoordBfr(texcoordConfig,GL_COLOR_ATTACHMENT2);


   cfg.addTexturebuffer(posBfr);
   cfg.addTexturebuffer(norBfr);
   cfg.addTexturebuffer(texcoordBfr);
   cfg.addTexturebuffer(
         TextureAttachment(
            TextureConfig("depth",GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT),
            GL_DEPTH_ATTACHMENT));

   gBuffer.init(cfg);
   gBuffer.bindFrameBuffer();
   glEnable(GL_DEPTH_TEST);
   gBuffer.unbindFrameBuffer();



   //Set the default renderer
 }

 ToolScene::~ToolScene()
 {

 }

void ToolScene::initPrograms()
{
   Program * defaultProg = new Program("Default Render");
   programMgr.setProgram(defaultProg);
   programMgr.setVertexShader("assets/shaders/phong_vert.vs");
   programMgr.setFragmentShader("assets/shaders/phong_frag.fs");
   programMgr.reload();
   //Create the base grid program.
   baseRenderProgram = createProgram("Grid Renderer");
   baseRenderProgram->addVertexShader("assets/shaders/debug_vert.vs");
   baseRenderProgram->addFragmentShader("assets/shaders/debug_frag.fs");

   deferredRenderProgram = createProgram("Tool Deferred Renderer");
   deferredRenderProgram->addVertexShader("assets/shaders/shadertool_framebufferVertex.vs");
   deferredRenderProgram->addFragmentShader("assets/shaders/shadertool_framebufferFragment.fs");

   managerWindow.setProgramManager(&(this->programMgr));
   managerWindow.setModelManager(&(this->modelMgr));



}

void ToolScene::initialBind()
{
   baseRenderProgram->enable();
   GL_Logger::LogError("Enable");
   baseRenderProgram->getUniform("P").bind(camera->getPerspectiveMatrix());
   GL_Logger::LogError("Bind P");

   baseRenderProgram->getUniform("M").bind(glm::mat4(1.0));
   GL_Logger::LogError("Bind M");

   baseRenderProgram->getUniform("debugColor").bind(glm::vec3(0.7));
    GL_Logger::LogError("Bind color");

   deferredRenderProgram->enable();
   deferredRenderProgram->getUniform("P").bind(camera->getPerspectiveMatrix());
   GL_Logger::LogError("Bind P");



   managerWindow.refresh();
   glClearColor(0.2,0.2,0.2,1.0);

}

void ToolScene::renderDeferred()
{
   //Render the defferred parts of the program.
   deferredRenderProgram->enable();
   deferredRenderProgram->getUniform("V").bind(camera->getViewMatrix());
   std::shared_ptr<UniformObjectController> ctrl = programMgr.getModelUniformController();
   if(ctrl != nullptr)
   {
      glm::mat4 mMtx, nMtx;
      if(ctrl->getType() == GLSLType::GLSLmat4)
      {
         mMtx = std::static_pointer_cast<TransformController>(ctrl)->getTransform().getMatrix();
      }
      else
      {
         mMtx = glm::mat4(1.0);
      }
      nMtx = glm::transpose(glm::inverse(mMtx));
      deferredRenderProgram->getUniform("M").bind(mMtx);
      deferredRenderProgram->getUniform("N").bind(nMtx);
   }
   GL_Logger::LogError("Bind M");

   gBuffer.bindFrameBuffer();
   glClearColor(0.0,0.0,0.0,0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   GL_Logger::LogError("Error Binding deferred program");
   modelMgr.render(deferredRenderProgram);
   GL_Logger::LogError("Error Rendering deferred program");
   gBuffer.unbindFrameBuffer();
   deferredRenderProgram->disable();
}
void ToolScene::extractRenderInfo(RenderInfoController & ctrl)
{
   glm::vec3 position;
   glm::vec3 normal;
   glm::vec3 texCoord;
   gBuffer.bindFrameBuffer();
   glReadBuffer(GL_COLOR_ATTACHMENT0);
   int mouseX = Mouse::getX();
   int mouseY = getContext()->getWindowHeight() - Mouse::getY();
   glReadPixels(mouseX, mouseY, 1, 1, GL_RGB, GL_FLOAT, glm::value_ptr(position));
   GL_Logger::LogError("Read pixels from GBuffer Position");
   glReadBuffer(GL_COLOR_ATTACHMENT1);
   glReadPixels(mouseX, mouseY, 1, 1, GL_RGB, GL_FLOAT, glm::value_ptr(normal));
   GL_Logger::LogError("Read pixels from GBuffer Normals");
   
   glReadBuffer(GL_COLOR_ATTACHMENT2);
   glReadPixels(mouseX, mouseY, 1, 1, GL_RGB, GL_FLOAT, glm::value_ptr(texCoord));
   GL_Logger::LogError("Read pixels from GBuffer TexCoords");
   
   gBuffer.unbindFrameBuffer();
   glReadBuffer(GL_BACK);
   
   ctrl.setPosition(position);
   ctrl.setNormal(normal);
   ctrl.setTexCoord(texCoord);

}
void ToolScene::render()
{
   glClearColor(0.2,0.2,0.2,1.0);
   glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
   //Render grid
   baseRenderProgram->enable();
   baseRenderProgram->getUniform("V").bind(camera->getViewMatrix());

   grid.render(*baseRenderProgram);
   baseRenderProgram->disable();

   
   renderDeferred();
   extractRenderInfo(renderInfoController);
   

   std::shared_ptr<Program> prog = programMgr.getActiveProgram();
   //If MVP, bind.
   prog->enable();
   programMgr.bindDefaultVariables(
                                  camera->getViewMatrix(),
                                  camera->getPerspectiveMatrix(),
                                  glfwGetTime());
   modelMgr.render(prog);
   managerWindow.render();
   
   ImGui::Begin("Buffers");
   ImGui::Image((void *)gBuffer.getTextureID("position"),ImVec2(128,128), ImVec2(0,1), ImVec2(1,0));
   ImGui::Image((void *)gBuffer.getTextureID("normal"),ImVec2(128,128),ImVec2(0,1), ImVec2(1,0));
   ImGui::Image((void *)gBuffer.getTextureID("texCoords"),ImVec2(128,128),ImVec2(0,1), ImVec2(1,0));
   ImGui::Image((void *)gBuffer.getTextureID("depth"),ImVec2(128,128),ImVec2(0,1), ImVec2(1,0));
   
   ImGui::End();

   ImGui::Begin("Info");
   ImGui::Text("Position:\t<%f %f %f>\nNormal:\t<%f %f %f>\nTexCoord:\t<%f %f>",
         renderInfoController.getPosition().x, renderInfoController.getPosition().y, renderInfoController.getPosition().z,
         renderInfoController.getNormal().x, renderInfoController.getNormal().y, renderInfoController.getNormal().z,
         renderInfoController.getTexCoord().x, renderInfoController.getTexCoord().y);
   ImGui::End();
         


}




void ToolScene::update()
{
   camera->update(getContext());

}
void ToolScene::cleanup()
{

}

