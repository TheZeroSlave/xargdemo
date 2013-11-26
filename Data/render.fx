float4x4 g_worldMatrix;
float4x4 g_viewMatrix;
float4x4 g_projMatrix;

float g_time;
float g_scale;
texture g_baseTexture;
texture g_detailTexture0;

sampler2D g_diffuseSampler =
sampler_state
{
    Texture = <g_baseTexture>;
    MinFilter = Anisotropic;
    MagFilter = Anisotropic;
    MipFilter = Anisotropic;
};

sampler2D g_detailSampler0 =
sampler_state
{
    Texture = <g_detailTexture0>;
    MinFilter = Anisotropic;
    MagFilter = Anisotropic;
    MipFilter = Anisotropic;
};

void ShipVS(float4 iPos : POSITION,
			float4 iColor : COLOR0,
			float4 iTex : TEXCOORD0,
            out float4 oPos : POSITION,
			out float4 oCol : COLOR0,
			out float4 oTex : TEXCOORD0)
{
	oPos = mul(iPos, g_worldMatrix);
	oPos = mul(oPos, g_viewMatrix);
	oPos = mul(oPos, g_projMatrix);
	oCol = iColor;
	oTex = iTex;
}

float4 ShipPS(float4 iPos : POSITION, float4 iTex : TEXCOORD0) : COLOR0
{
	return tex2D(g_diffuseSampler, iTex.xy);
}


float4 EnginePS(float4 iPos : POSITION, float4 iTex : TEXCOORD0) : COLOR0
{
	float2 origTex = iTex;
	origTex.y += g_scale;
	iTex.y *= g_time;
	return tex2D(g_diffuseSampler, iTex.xy)* /* tex2D(g_detailSampler0, origTex) */ (float4(86/ 255.0f, 193/ 255.0f, 220/ 255.0f, 0) );
}

float4 StarPS(float4 iPos : POSITION, float4 col : COLOR0, float4 iTex : TEXCOORD0) : COLOR0
{
	return tex2D(g_diffuseSampler, iTex.xy) * col;
}



technique RenderShip
{
    pass p0
    {
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 ShipVS();
        PixelShader = compile ps_2_0 ShipPS();
    }
	pass p1
    {
		AlphaBlendEnable = true;
		SrcBlend = One;
        DestBlend = One;
        VertexShader = compile vs_2_0 ShipVS();
        PixelShader = compile ps_2_0 EnginePS();
    }
}

technique RenderStar
{
    pass p0
    {
		AlphaBlendEnable = true;
		SrcBlend = One;
        DestBlend = One;
		ZEnable = false;
        VertexShader = compile vs_2_0 ShipVS();
        PixelShader = compile ps_2_0  StarPS();
    }
}