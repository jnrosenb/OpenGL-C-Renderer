#version 330 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_LIGHTS 1

//Function declarations
float getFresnelTerm(float cosOi);
float getFresnelTerm2(float cosOi);
float getGeometryTerm(float MdotL, float MdotH, float VdotH, float MdotV);
float getGeometryTerm2(float MdotL, float MdotH, float VdotH, float MdotV);
float getDistributionTerm(float MdotH);
float getDistributionTerm2(float MdotH);

uniform float ks;
uniform float rgh;
uniform float perm;
uniform vec3 diffuseColor;
uniform vec4 lightColor[MAX_LIGHTS];
uniform int lightCount;
uniform sampler2D textureSampler;
uniform sampler2D shadowMap;

in vec4 normal_vector;
in vec4 view_vector;
in vec4 light_pos[MAX_LIGHTS];
in vec2 uv;
in vec4 shadowSpacePos;

out vec4 frag_color;

void main(void) 
{
  //Get the texture color from sampler
  vec3 textDiffuse = texture(textureSampler, uv).xyz;

  ///Ambient effect is calculated once, does not depend of num of lights
  //vec3 ambient = 0.25f * vec3(1, 1, 1) * diffuseColor;
  vec3 ambient = 0.25f * vec3(1, 1, 1) * textDiffuse;

  vec4 m = normalize(normal_vector);
  vec4 v = normalize(view_vector);
  float dwl = 1.0f;// / lightCount;

  vec3 radiance;
  //FOR EACH LIGHT
  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    float light_dist = length(light_pos[i]);
    //float dwl = 1 / (light_dist * light_dist); //Solid angle measure for point light
    vec4 l = light_pos[i] / light_dist; //Do it like this to avoid calculating length again
    vec4 h = normalize(v + l);
    float MdotL = max(dot(m,l), 0);
    float MdotV = max(dot(m,v), 0);
    float VdotH = max(dot(v,h), 0);
    float MdotH = max(dot(m,h), 0);

    //PBR terms
    float F = getFresnelTerm(VdotH);
    float diffContribution = 1.0f - F;
    float G = getGeometryTerm(MdotL, MdotH, VdotH, MdotV);
    float D = getDistributionTerm(MdotH);//*/
    
    /*float F = getFresnelTerm2(MdotL);
    float diffContribution = 1.0f - F;
    float G = getGeometryTerm2(MdotL, MdotH, VdotH, MdotV);
    float D = getDistributionTerm2(MdotH);//*/
    
    //INTERESTING EFFECT
    /*float F = 0.9f;
    float diffContribution = 1.0f - F;
    float G = 0.9f;
    float D = 1.0f;//*/

    //Remember special cases (zero denominator)
    float BRDF = (F * D * G) / (M_PI *MdotL * MdotV); //TODO: BRDF zero when MdotL zero or MdotV zero//
    if (MdotL <= 0.0f)
    {
      BRDF = 0.0f;
    }

    //Radiance contributed by this light
    radiance += (diffContribution * (textDiffuse / M_PI) + BRDF ) * MdotL * lightColor[i].xyz * dwl;
    //radiance += (diffContribution * (diffuseColor / M_PI) + BRDF ) * MdotL * lightColor[i].xyz * dwl;
  }
  vec3 finalColor = radiance  + ambient;
  frag_color = vec4(finalColor ,1.0f);
}


//For now, this gets us the fresnel term
float getFresnelTerm(float cosOi)
{
  float nt = (1.0f + sqrt(ks)) / (1.0f - sqrt(ks)); //TODO: Ks cannot be 1//
  //float nt = (1.0f + sqrt(KS)) / (1.0f - sqrt(KS)); //TODO: Ks cannot be 1//
  float invNt = 1.0f / nt;
  //float mu = permeability;
  float mu = perm;
  float invNt2 = invNt * invNt;
  float cosOt = sqrt(1 - invNt2*(1 - cosOi*cosOi));

  float parallelF = (nt*cosOi - mu*cosOt) / (nt*cosOi + mu*cosOt);
  float perpendF = (mu*cosOi - nt*cosOt) / (mu*cosOi + nt*cosOt);
  float F = 0.5f*(parallelF*parallelF + perpendF*perpendF); 
  return max(F, 0.0f);
}

//For now, this gets us the fresnel term
float getFresnelTerm2(float VdotH)
{
  float p = pow((1-VdotH), 5);
  return ks + (1 - ks)*p;
  //return KS + (1 - KS)*p;
}

//For now, this gets us the geometry term
float getGeometryTerm(float MdotL, float MdotH, float VdotH, float MdotV)
{
  float g1 = 1.0f;
  float g2 = (2 * MdotH * MdotV) / VdotH;
  float g3 = (2 * MdotH * MdotL) / VdotH;

  float t =  min(g2, g3);
  return min(g1, t);
}


//For now, this gets us the geometry term
float getGeometryTerm2(float MdotL, float MdotH, float VdotH, float MdotV)
{
  return MdotL*MdotV;
}


//For now, this gets us the distribution term
float getDistributionTerm(float MdotH)
{
  float tanO2 = (1.0f / (MdotH*MdotH)) - 1.0f; //TODO: Rgh cannot be zero, and MdotH cannot be zero//
  //float rgh2 = ROUGHNESS * ROUGHNESS;
  float rgh2 = rgh * rgh;
  float cosO4 = MdotH*MdotH*MdotH*MdotH;

  float D = exp(-(tanO2/rgh2)) / (rgh2 * cosO4);
  return max(D, 0.0f);
}


//For now, this gets us the distribution term
float getDistributionTerm2(float MdotH)
{
  //float rgh2 = ROUGHNESS * ROUGHNESS;
  float rgh2 = rgh * rgh;
  float dot2 = MdotH*MdotH;
  float den = M_PI * pow((dot2*(rgh2 - 1) + 1), 2);
  float num = rgh2;
  return num/den;
}

