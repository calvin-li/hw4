#version 150

in vec4 vertexPos_modelspace;
in vec4 vertexNormal;
in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 normalMatrix;

uniform float red;
uniform float blue;
uniform float green;

uniform vec4 light;
uniform float intensity;
uniform float shine;

out vec4 fragmentPosition;
out vec3 vColor;
out vec4 v;
out vec4 n;
out float d;
out vec2 fragUV;

void main()
{
   //calculate variables
   vec4 transformedVertexPos = MVP * vertexPos_modelspace;
   v = normalize(transformedVertexPos) * -1;

   n = normalize(normalMatrix*vertexNormal);

   // Transform vertex using MVP
   gl_Position = MVP*vec4(vertexPos_modelspace);

   // Pass through variables
   fragmentPosition = transformedVertexPos;
   vColor = vec3(red, blue, green);

   fragUV = vertexUV;
}//main
