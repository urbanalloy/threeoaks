///////////////////////////////////////////////////////////////////////////////////////////////
//
// Valentine Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------

texture background; // This texture will be loaded by the application

//-----------------------------------------------------------------------------
// Colors
//-----------------------------------------------------------------------------

float3 LightColor = {1.0,0.6,0.8};
float3 DarkColor = {0.2,0.05,0};

float Desat = 0.2f;
float Toned = 0.2f;

sampler Sampler = sampler_state
{
    Texture   = (background);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Pixel Shader Input
struct PS_INPUT
{
	float2 texture0  : TEXCOORD0;
    float4 color	 : COLOR0;
};

// Pixel Shader Output
struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
PS_OUTPUT passthrough( PS_INPUT IN )
{
	PS_OUTPUT OUT;
	
	OUT.color = tex2D(Sampler, IN.texture0.xy);
	
	return OUT;
}

PS_OUTPUT colorize(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    
	float3 scnColor = LightColor * tex2D(Sampler, IN.texture0).xyz;
    float3 grayXfer = half3(0.3,0.59,0.11);
    float  gray     = dot(grayXfer,scnColor);
    float3 muted    = lerp(scnColor,gray.xxx,Desat);
    float3 sepia    = lerp(DarkColor,LightColor,gray);
    float3 result   = lerp(muted,sepia,Toned);
    OUT.color = float4(result,1);

    return OUT;
}

PS_OUTPUT blur(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    
    float4 color = tex2D(Sampler, IN.texture0.xy);
	color += tex2D(Sampler, IN.texture0.xy+0.001);
	color += tex2D(Sampler, IN.texture0.xy+0.002);
	color += tex2D(Sampler, IN.texture0.xy+0.003);
	
	OUT.color = color / 4;
    
    return OUT;
}

PS_OUTPUT blur_and_colorize(PS_INPUT IN)
{
	PS_OUTPUT OUT;
	
	float3 scnColor = LightColor * blur(IN).color;
    float3 grayXfer = half3(0.3,0.59,0.11);
    float  gray     = dot(grayXfer,scnColor);
    float3 muted    = lerp(scnColor,gray.xxx,Desat);
    float3 sepia    = lerp(DarkColor,LightColor,gray);
    float3 result   = lerp(muted,sepia,Toned);
    OUT.color = float4(result,1);
    
    return OUT;

}

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
technique PassThrough
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler);
		PixelShader  = compile ps_2_0 passthrough();
    }
}

technique Colorize
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler);
		PixelShader  = compile ps_2_0 colorize();
    }
}

technique Blur
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler);
		PixelShader  = compile ps_2_0 blur();
    }
}

technique Blur_and_Colorize
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler);
		PixelShader  = compile ps_2_0 blur_and_colorize();
    }
}

