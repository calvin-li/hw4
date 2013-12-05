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

out vec3 color;

void main()
{
   const float ka = 1;
   const float kd = 1;
   const float ks = 5;

	 vec3 vColor = texture(myTextureSampler, fragUV).rgb;

   //transformed normal
   //light direction
   vec4 lightVector = (light - fragmentPosition);
   vec4 l = normalize(lightVector);
   vec4 r = normalize(reflect(l, n));
   float d = length(lightVector);
	 float attenuation = 1 / (d*d);

	 float cosAlpha = max(dot(l, n), 0);
	 float cosTheta = max(dot(r, v), 0);

	 vec3 ambient =  ka * vColor;
	 vec3 diffuse =  kd * vColor * cosAlpha;
	 vec3 specular = ks * light.xyz * pow(cosTheta, shine);

   //vec3 i = ambient;
	 vec3 i = clamp(intensity * (ambient + attenuation*(diffuse + specular)), 0, 1);

   color = i;

}//main
