//-----------------------------------------------------------------------------
// File: HDRCubeMap.fx
//
// Desc: Effect file for high dynamic range cube mapping sample.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
float4x4 g_world;
float4x4 g_view;
float4x4 g_proj;
float4x4 g_wvp;

float3 g_eye;
float4 g_lightPos;
float4 g_lightDir;
float4x4 g_lightMatrix;

float g_shaftPos;
float g_noiseOffset;
float4x4 g_invView;

bool g_useNoise;
bool g_useCubeEnv;
bool g_useSpecular;
int  g_specFactor;
float g_shaftOpacity;
//
float4 g_clipPlanes[6];

//
texture g_diffuseTexture;
texture g_noiseTexture1;
texture g_noiseTexture2;
texture g_shadowTexture;

float4x4 g_textureMatrix;
sampler2D g_diffuseSampler =
sampler_state
{
    Texture = <g_diffuseTexture>;
    MinFilter = Anisotropic;
    MagFilter = Anisotropic;
    MipFilter = Anisotropic;
};

sampler2D g_shadowSampler =
sampler_state
{
    Texture = <g_shadowTexture>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler2D g_cookieSampler =
sampler_state
{
    Texture = <g_diffuseTexture>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler2D g_noiseSampler1 =
sampler_state
{
    Texture = <g_noiseTexture1>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Mirror;
    AddressV = Mirror;
};

sampler2D g_noiseSampler2 =
sampler_state
{
    Texture = <g_noiseTexture2>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Mirror;
    AddressV = Mirror;
};

texture g_cubeTexture;
samplerCUBE g_cubeSampler =
sampler_state
{
    Texture = <g_cubeTexture>;
    MinFilter = Anisotropic;
    MagFilter = Anisotropic;
    MipFilter = Anisotropic;
};

//blinn shading
void SphereVertexShader(float4 iPos : POSITION,
						float3 iNormal: NORMAL,
                        float2 iTex: TEXCOORD0,
						out float4 oPos : POSITION,
						out float3 oNormal: TEXCOORD0,
						out float2 oTex : TEXCOORD1,
                        out float3 oLight : TEXCOORD2,
                        out float3 oH : TEXCOORD3,
                        out float3 oReflect : TEXCOORD4)
{
    float4 transformedPos = mul(iPos, g_world);
    float3 vEye = (float3)transformedPos - (float3)g_eye;
    oNormal = mul(iNormal, (float3x3)g_world);
    oTex = iTex;
    oLight = (float3)(g_lightPos - transformedPos);
    oH =  -vEye;
    oReflect = normalize(reflect(vEye, oNormal));
    oPos = mul(transformedPos, g_view);
    oPos = mul(oPos, g_proj);
}

float cosInnerAngle = 0.98f;
float cosOuterAngle = 0.96f;


float4 computeEnvColor(float3 tex)
{
    float4 color = float4(1.0, 1.0, 1.0, 1.0);
    if (g_useCubeEnv)
    {
        color = texCUBE(g_cubeSampler, tex);
    }
    return color;
}

float4 SpherePixelShader(float4 iPos : POSITION,
                         float3 iNormal : TEXCOORD0,
                         float2 iTex : TEXCOORD1,
                         float3 iLight : TEXCOORD2,
                         float3 iH : TEXCOORD3,
                         float3 iReflect : TEXCOORD4) : COLOR
{
    
    float3 hn = normalize(iH);
    float3 ln = normalize(iLight);
    float3 nn = normalize(iNormal);
    float3 r = normalize(reflect(-hn, nn));

    float4 ambientColor = float4(0.25, 0.25, 0.25, 1.0);
    float4 specularColor = float4(0, 0, 0, 0);

    if (g_useSpecular)
    {
        specularColor = pow(saturate(dot(ln, r)), g_specFactor);
    }

    float spotAtt = 0.0f;
    float currCosAngle = dot(ln, g_lightDir);
    if (currCosAngle > cosInnerAngle)
    {
        spotAtt = 1.0f;
    }
    else if (currCosAngle > cosOuterAngle)
    {
        spotAtt = max((currCosAngle - cosOuterAngle) / (cosInnerAngle - cosOuterAngle), 0);
    }
    spotAtt *= (1.f - length(iLight) / 10.f);
 
    float NdL = max(dot(nn, ln), 0);

    float4 envColor = computeEnvColor(iReflect);
    float4 diffuseColor = (spotAtt * NdL + ambientColor * envColor) * tex2D(g_diffuseSampler, iTex);
    //
	return (diffuseColor + specularColor * NdL);
}


//blinn shading
void RenderSceneVS(float4 iPos : POSITION,
						float3 iNormal: NORMAL,
                        float2 iTex: TEXCOORD0,
						out float4 oPos : POSITION,
						out float3 oNormal: TEXCOORD0,
						out float2 oTex : TEXCOORD1,
                        out float3 oLight : TEXCOORD2,
                        out float4 oLightPos : TEXCOORD3)
{
    float4 transformedPos = mul(iPos, g_world);
    float3 vEye = (float3)transformedPos - (float3)g_eye;
    oNormal = mul(iNormal, (float3x3)g_world);
    oTex = iTex;
    oLight = (float3)(g_lightPos - transformedPos);
    oPos = mul(transformedPos, g_view);
    oPos = mul(oPos, g_proj);
    oLightPos = mul(iPos, g_lightMatrix);
}

float4 RenderScenePS(float3 iNormal : TEXCOORD0,
                     float2 iTex : TEXCOORD1,
                     float3 iLight : TEXCOORD2,
                     float4 oLightPos : TEXCOORD3) : COLOR
{
    float3 ln = normalize(iLight);
    float3 nn = normalize(iNormal);

    float4 ambientColor = float4(0.2, 0.2, 0.2, 1.0);
    float spotAtt = 0.0f;
    float currCosAngle = dot(ln, g_lightDir);
    if (currCosAngle > cosInnerAngle)
    {
        spotAtt = 1.0f;
    }
    else if (currCosAngle > cosOuterAngle)
    {
        spotAtt = max((currCosAngle - cosOuterAngle) / (cosInnerAngle - cosOuterAngle), 0);
    }
    spotAtt *= (1.f - length(iLight) / 10.f);
 
    float NdL = max(dot(nn, ln), 0);
    float2 texShadow = oLightPos.xy / oLightPos.w *0.5f + float2(0.5f, 0.5f);
    texShadow.y = 1.f - texShadow.y;
    float shadow = (oLightPos.z / oLightPos.w) > tex2D(g_shadowSampler, texShadow) + 0.00005f ? 0.0f : 1.0f;
    float4 diffuseColor = (spotAtt * NdL * shadow + ambientColor) * tex2D(g_diffuseSampler, iTex);
    //
	return (diffuseColor);
}

technique RenderSceneTech
{
    pass p0
    {
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader = compile ps_2_0  RenderScenePS();
    }
}

technique RenderSphereTech
{
    pass p0
    {
        VertexShader = compile vs_2_0 SphereVertexShader();
        PixelShader = compile ps_2_0 SpherePixelShader();
    }
}

void SkyboxVS(float4 iPos : POSITION,
              float4 iNormal : NORMAL,
              float2 iTex : TEXCOORD0,
              out float4 oPos : POSITION,
              out float2 oTex : TEXCOORD0)
{
    oPos = mul(iPos, g_wvp);
    oTex = iTex * 10;
}

float4 SkyboxPS(float2 iTex : TEXCOORD0) : COLOR
{
    return tex2D(g_diffuseSampler, iTex) * float4(0.4, 0.4, 0.4, 1.0);
}

technique RenderSkyboxTech
{
    pass p0
    {
        VertexShader = compile vs_2_0 SkyboxVS();
        PixelShader = compile ps_2_0 SkyboxPS();
    }
}

void ShaftsVS(float4 iPos : POSITION,
              out float4 oPos : POSITION,
              out float4 oTex : TEXCOORD0,
              out float4 oViewCoords : TEXCOORD1,
              out float4 oClipPos : TEXCOORD2)
{
    float4 shiftedPos = iPos + float4(0, 0, g_shaftPos, 0);
    oPos = mul(shiftedPos, g_proj);
    oTex = mul(shiftedPos, g_textureMatrix);
    oViewCoords = mul(shiftedPos, g_invView);
    oClipPos = oPos;
}

void testClipPoint(float4 pt)
{
    clip(dot(g_clipPlanes[0], pt));
    clip(dot(g_clipPlanes[1], pt));
    clip(dot(g_clipPlanes[2], pt));
    clip(dot(g_clipPlanes[3], pt));
    clip(dot(g_clipPlanes[4], pt));
    clip(dot(g_clipPlanes[5], pt));
}

float4 ShaftsPS(float4 iTex : TEXCOORD0,
                float4 iViewPos : TEXCOORD1,
                float4 iClipPos : TEXCOORD2) : COLOR
{

    testClipPoint(iViewPos);

    float2 tex = 0.5f * iTex.xy / iTex.w + float2(0.5, 0.5);
    tex.y = 1.0f - tex.y;
    float inShadow = ((iTex.z / iTex.w) > tex2D(g_shadowSampler, tex) + 0.00005f) ? 0.0 : 1;

    float4 noiseColor = float4(1.0, 1.0, 1.0, 1.0);
    //taking noise in account
    if (g_useNoise)
    {
        noiseColor = tex2D(g_noiseSampler1, tex + float2(g_noiseOffset, 0)) *
                     tex2D(g_noiseSampler2, tex + float2(0, g_noiseOffset)) + float4(0.2, 0.2, 0.2, 1.0f);
    }

    return tex2D(g_cookieSampler, tex) *
           noiseColor *
           inShadow *
           (8.0f / dot(iTex.xyz, iTex.xyz)) *
           float4(g_shaftOpacity, g_shaftOpacity, g_shaftOpacity, 1.0);
}

technique RenderShaftsTech
{
    pass p0
    {
        VertexShader = compile vs_2_0 ShaftsVS();
        PixelShader = compile ps_3_0 ShaftsPS();
    }
}

void ShadowVS(float4 iPos : POSITION,
              out float4 oPos : POSITION,
              out float4 oTex : TEXCOORD0)
{
    oPos = mul(iPos, g_wvp);
    oTex = oPos;
}

float4 ShadowPS(float4 iTex : TEXCOORD0) : COLOR
{
    return iTex.z / iTex.w;
}

technique RenderShadowTech
{
    pass p0
    {
        VertexShader = compile vs_2_0 ShadowVS();
        PixelShader = compile ps_2_0 ShadowPS();
    }
}