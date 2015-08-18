#ifndef __Keyboard_H__
#define __Keyboard_H__
#define NUM_KEYS 1024
class Keyboard
{
private:
   //Could store addl data such as modification
   static short keyStatus[NUM_KEYS];
   static short bfrKeyStatus[NUM_KEYS];
public:
   /**
    * Test if a key is held
    * @param  scanCode the scancode to check for
    * @return          true if key is held
    */
   static bool isKeyDown(int key);
   /**
    * Check if a key is up
    * @param  key the key to check for
    * @return          true if the key is up
    */
   static bool isKeyUp(int key);
   /**
    * Check if a key was hit, but not released
    * @param  key the key to check for
    * @return          true if the key was up
    */
   static bool key(int key);

   /**
    * Set a key status for lookup later
    * @param key    [description]
    * @param action [description]
    */
   static void setKeyStatus(int key, int action);

   /**
    * Set all active keys to 'held'
    */
   static void update();
   

};

#endif