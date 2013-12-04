#version 150

in vec4 gl_FragCoord;
in vec2 fragUV;
in vec3 vColor;
uniform int is_dice;
uniform sampler2D myTextureSampler;

out vec3 color;

void main()
{
  color = vColor * texture(myTextureSampler, fragUV).rgb;

}//main
