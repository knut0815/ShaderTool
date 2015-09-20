#version 330 core
in vec2 fragTexCoords;
out vec4 color;

uniform sampler2D screenTexture;
const float offset = 1.0 / 300;  
void main()
{
   vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f,    offset),  // top-center
        vec2(offset,  offset),  // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offset,  0.0f),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f,    -offset), // bottom-center
        vec2(offset,  -offset)  // bottom-right    
    );
   float kernel[9] = float[](
    0.0 , 0.0 , 0.0 ,
    0.0 , 1.0 , 0.0 ,
    0.0 , 0.0 , 0.0   
);
    
   vec3 sampleTex[9];
   for(int i = 0; i < 9; i++)
   {
      vec2 sinOffset = vec2(0.0,sin(fragTexCoords.s * 5)/4.0);
      sampleTex[i] = vec3(texture(screenTexture, fragTexCoords.st + sinOffset +  offsets[i]));
   }
    
   vec3 col = vec3(0.0);
   for(int i = 0; i < 9; i++)
   {
      col += sampleTex[i] * kernel[i];
   }
   color = vec4(col,1.0);
}