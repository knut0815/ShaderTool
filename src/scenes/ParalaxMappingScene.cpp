#include "ParalaxMappingScene.h"
#include <glm/gtc/type_ptr.hpp>
//Create a tangent relative to the triangle defined by pa, pb, and pc
glm::vec3 ParalaxMappingScene::getTangent(Point & pa, Point & pb, Point & pc)
{
   glm::vec3 tangent;
   glm::vec3 edge1 = pb.position - pa.position;
   glm::vec3 edge2 = pc.position - pa.position;

   glm::vec2 deltaUV1 = pb.texCoords - pa.texCoords;
   glm::vec2 deltaUV2 = pc.texCoords - pa.texCoords;

   float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

   tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
   tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
   tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

   tangent = glm::normalize(tangent);
   return tangent;

}
ParalaxMappingScene::ParalaxMappingScene(Context * ctx):
CameraScene(ctx),
diffuseMat("assets/textures/bricks2.jpg",32),
normalMap("assets/textures/bricks2_normal.jpg"),
displacementMap("assets/textures/bricks2_disp.jpg"),
light(glm::vec3(0.1),glm::vec3(0.6),glm::vec3(0.8),50)
{
   light.transform.setPosition(glm::vec3(1.0));
   std::vector<Point> pts;
   pts.push_back(Point());
   pts.push_back(Point());
   pts.push_back(Point());
   pts.push_back(Point());

   pts[0].position = glm::vec3(-1.0,  1.0, 0.0); //ul
   pts[1].position = glm::vec3(-1.0, -1.0, 0.0); //ll
   pts[2].position = glm::vec3(1.0, -1.0, 0.0);  //lr
   pts[3].position = glm::vec3(1.0, 1.0, 0.0);   //ur

   pts[0].normal = glm::vec3(0.0,0.0,1.0);
   pts[1].normal = glm::vec3(0.0,0.0,1.0);
   pts[2].normal = glm::vec3(0.0,0.0,1.0);
   pts[3].normal = glm::vec3(0.0,0.0,1.0);

   pts[0].texCoords = glm::vec2(0.0,1.0);
   pts[1].texCoords = glm::vec2(0.0,0.0);
   pts[2].texCoords = glm::vec2(1.0,0.0);
   pts[3].texCoords = glm::vec2(1.0,1.0);

   unsigned int inds[] = {1,2,0,3,0,2};
   for(int i = 0; i < 6; i+=3)
   {
      pts[inds[i]].tangent =
      pts[inds[i + 1]].tangent =
      pts[inds[i + 2]].tangent = getTangent(pts[inds[i]],pts[inds[i+1]],pts[inds[i+2]]);
   }

   ElementBufferObject ebo;
   ebo.setData(inds,6);
   VertexBuffer bfr;
   bfr.setData(pts);

   planeVAO.addElementArray(ebo);
   planeVAO.addAttribute(0,bfr,sizeof(Point)); // Add point attribute
   planeVAO.addAttribute(1,bfr,sizeof(Point),offsetof(Point,normal)); // Add point attribute
   planeVAO.addAttribute(2,bfr,sizeof(Point),offsetof(Point,texCoords),2); // Add point attribute
   planeVAO.addAttribute(3,bfr,sizeof(Point),offsetof(Point,tangent)); // Add point attribute

   paralaxMapProg = createProgram("Normal mapping program");
   frameDisplayProg = createProgram("Frame Display Program");

}

void ParalaxMappingScene::initPrograms()
{
   paralaxMapProg->addVertexShader("assets/shaders/tex_vert_normalMap.vs");
   paralaxMapProg->addFragmentShader("assets/shaders/phong_frag_paralaxMap.fs");

   frameDisplayProg->addVertexShader("assets/shaders/frame_vert.vs");
   frameDisplayProg->addGeometryShader("assets/shaders/frame_geom.gs");
   frameDisplayProg->addFragmentShader("assets/shaders/simple_geom_frag.fs");
     
}
void ParalaxMappingScene::initialBind()
{
   paralaxMapProg->addUniform("M");
   paralaxMapProg->addUniform("V");
   paralaxMapProg->addUniform("P");
   paralaxMapProg->addUniform("viewPos");
   paralaxMapProg->addUniformStruct("pointLight",Light::getStruct());
   paralaxMapProg->addUniformStruct("material",TexturedMaterial::getStruct());
   paralaxMapProg->addUniform("normalMap");
   paralaxMapProg->addUniform("depthMap");
   frameDisplayProg->addUniform("M");
   frameDisplayProg->addUniform("V");
   frameDisplayProg->addUniform("P");


   paralaxMapProg->enable();
   glm::mat4 P = camera.getPerspectiveMatrix();
   paralaxMapProg->getUniform("P").bind(P);
   light.bind(paralaxMapProg->getUniformStruct("pointLight"));
   diffuseMat.bind(paralaxMapProg->getUniformStruct("material"));
   normalMap.enable(paralaxMapProg->getUniform("normalMap").getID());
   displacementMap.enable(paralaxMapProg->getUniform("depthMap").getID());
   paralaxMapProg->disable();
   frameDisplayProg->enable();

   frameDisplayProg->getUniform("P").bind(P);
   frameDisplayProg->disable();


   glClearColor(0.0,0.0,0.0,1.0);
}
void ParalaxMappingScene::render()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   paralaxMapProg->enable();
   glm::mat4 V = camera.getViewMatrix();
   glm::vec3 vPos = camera.transform.getPosition();
   paralaxMapProg->getUniform("viewPos").bind(vPos);
   paralaxMapProg->getUniform("V").bind(V);
   paralaxMapProg->getUniform("M").bind(planeTransform.getMatrix());
   planeVAO.bind();
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   planeVAO.unbind();
   
   frameDisplayProg->enable();
   frameDisplayProg->getUniform("V").bind(V);
   frameDisplayProg->getUniform("M").bind(planeTransform.getMatrix());
   planeVAO.bind();
   glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, 0);
   planeVAO.unbind();
   frameDisplayProg->disable();



}

void ParalaxMappingScene::update()
{
   CameraScene::update();
   if(Keyboard::isKeyDown(GLFW_KEY_W))
   {
      planeTransform.rotate(0.02,glm::vec3(1.0,0.0,0.0),Space::LOCAL);
   }
   if(Keyboard::isKeyDown(GLFW_KEY_S))
   {
     planeTransform.rotate(-0.02,glm::vec3(1.0,0.0,0.0),Space::LOCAL);
   }
   if(Keyboard::isKeyDown(GLFW_KEY_A))
   {
      planeTransform.rotate(0.02,glm::vec3(0.0,1.0,0.0),Space::LOCAL);
   }
   if(Keyboard::isKeyDown(GLFW_KEY_D))
   {
     planeTransform.rotate(-0.02,glm::vec3(0.0,1.0,0.0),Space::LOCAL);
   }
}
void ParalaxMappingScene::cleanup()
{
   normalMap.disable();
   diffuseMat.unbind();

}