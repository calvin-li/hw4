#version 150

in vec4 vertexPos_modelspace;
in vec3 vertexColor;
in vec4 vertexNormal;
in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 normalMatrix;

uniform vec4 light;
uniform float intensity;
uniform float shine;

out vec2 fragUV;
out vec3 vColor;

void main()
{
   const float ka = 1;
   const float kd = 1;
   const float ks = 6;

   //transformed normal
   vec4 n = normalize(normalMatrix*vertexNormal);
   vec4 l = light - MVP*vertexPos_modelspace;
   //light direction
   vec4 v = normalize(-MVP*vertexPos_modelspace);
   vec4 r = normalize(reflect(normalize(l), n));

   float d = length(l); // distance
   l = normalize(l); //normalize light direction
   l = l; //transform light direction

   float i = (
         (1/(d*d) *
            (kd*intensity *
             max(dot(l, n), 0)) +
            (ks*intensity *
             max(pow(dot(r,v), shine), 0)
            )
         ) +
         ka*intensity
   );

   // Transform vertex using MVP
   gl_Position = MVP*vec4(vertexPos_modelspace);
   // Pass through color and texture coordinates
   fragUV = vertexUV;
   vColor = i * vertexColor;
}//main
