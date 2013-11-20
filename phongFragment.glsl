#version 150

in vec4 fragmentPosition;
in vec4 gl_FragCoord;
in vec3 vColor;
in vec4 v;
in vec4 n;
in vec2 fragUV;

uniform vec4 light;
uniform float intensity;
uniform float shine;
uniform sampler2D myTextureSampler;
uniform int is_dice;

out vec3 color;

void main()
{
   const float ka = 1;
   const float kd = 1;
   const float ks = 5;

   //transformed normal
   //light direction
   vec4 lightVector = (light - fragmentPosition);
   vec4 l = normalize(lightVector);
   vec4 r = normalize(reflect(l, n));
   float d = length(lightVector);

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

   if(is_dice == 1)
      color = i * texture(myTextureSampler, fragUV).rgb;
   else
      color = i * vColor;

}//main
