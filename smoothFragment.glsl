#version 150

in vec4 gl_FragCoord;
in vec2 fragUV;
in vec3 vColor;
uniform int is_dice;
uniform sampler2D myTextureSampler;

out vec3 color;

void main()
{
   if(is_dice == 1)
      color = vColor * texture(myTextureSampler, fragUV).rgb;
   else
      color = vColor;

}//main
