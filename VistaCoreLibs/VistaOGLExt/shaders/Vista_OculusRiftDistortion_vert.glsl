static const char* PostProcessVertexShaderSrc =
    "float4x4 View : register(c4);\n"
    "float4x4 Texm : register(c8);\n"
    "void main(in float4 Position : POSITION, in float4 Color : COLOR0, in float2 TexCoord : TEXCOORD0, in float2 TexCoord1 : TEXCOORD1,\n"
    "          out float4 oPosition : SV_Position, out float4 oColor : COLOR, out float2 oTexCoord : TEXCOORD0)\n"
    "{\n"
    "   oPosition = mul(View, Position);\n"
    "   oTexCoord = mul(Texm, float4(TexCoord,0,1));\n"
    "   oColor = Color;\n"
    "}\n";

// 'Compatibility' for gl_ModelViewProjection und gl_TextureMatrix
#version 330 compatibility

void main()
{
	
}
