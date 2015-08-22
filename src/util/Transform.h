#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
   Transform();
   /**
    * Set the position of the transform
    * @param pos position to go to
    */
   void setPosition(const glm::vec3 & pos);

   /**
    * Copy a transform from another transofrm
    * @param other the transform to copy.
    */
   void copy(Transform & other);

   /**
    * Set the rotation of the transform
    * @param eulerAngles the angles to rotate to
    */
   void setRotation(const glm::vec3 & eulerAngles);

   /**
    * Set the rotation using Angle-Axis
    * @param angle the angle
    * @param axis  the axis to rotate on
    */
   void setRotation(float angle, const glm::vec3 & axis);
   /**
    * Move by a vector
    * @param pos the delta vector
    */
   void translate(const glm::vec3 & pos);

   /**
    * Rotate by a set of angles
    * @param eulerAngles the new orientation to rotate to
    */
   void rotate(const glm::vec3 eulerAngles);

    /**
    * Rotate by a set of angles
    * @param eulerAngles the new orientation to rotate to
    */
   void rotate(float angle, const glm::vec3 & axis);
   
   /**
    * Orient the transform to point towards a target
    * @param target the target position
    */
   void lookAt(glm::vec3 target);

   /**
    * Scale the transform
    */
   void setScale(glm::vec3 scale);
   /**
    * Get the current position expressed as a 3d vector
    */
   glm::vec3 getPosition() const;

   /**
    * Get the current rotation expressed as a quaternion
    * @return [description]
    */
   glm::quat getRotation() const;

   /**
    * Get a 4x4 transform matrix representing this transform
    */
   glm::mat4 getMatrix();

   /**
    * Get the local up facing vector
    */
   glm::vec3 up() const;
   /**
    * Get the local right facing vector
    */
   glm::vec3 right() const;
   /**
    * Get the local forward facing vector
    */
   glm::vec3 forward() const;



private:
   void updateFrame();
   glm::vec3 position;
   glm::vec4 localUp;
   glm::vec4 localRight;
   glm::vec4 localForward;
   glm::vec3 scale;
   bool isDirty;
   glm::mat4 currentMatrix;
   glm::quat rotation;

};
#endif