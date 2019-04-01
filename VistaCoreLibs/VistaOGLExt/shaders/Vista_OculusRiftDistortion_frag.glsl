static const char* PostProcessPixelShaderSrc =
    "Texture2D Texture : register(t0);\n"
    "SamplerState Linear : register(s0);\n"
    "float2 LensCenter;\n"
    "float2 ScreenCenter;\n"
    "float2 Scale;\n"
    "float2 ScaleIn;\n"
    "float4 HmdWarpParam;\n"
    "\n"

    // Scales input texture coordinates for distortion.
    // ScaleIn maps texture coordinates to Scales to ([-1, 1]), although top/bottom will be
    // larger due to aspect ratio.
    "float2 HmdWarp(float2 in01)\n"
    "{\n"
    "   float2 theta = (in01 - LensCenter) * ScaleIn;\n" // Scales to [-1, 1]
    "   float  rSq= theta.x * theta.x + theta.y * theta.y;\n"
    "   float2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + "
    "                   HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
    "   return LensCenter + Scale * theta1;\n"
    "}\n"

    "float4 main(in float4 oPosition : SV_Position, in float4 oColor : COLOR,\n"
    " in float2 oTexCoord : TEXCOORD0) : SV_Target\n"
    "{\n"
    "   float2 tc = HmdWarp(oTexCoord);\n"
    "   if (any(clamp(tc, ScreenCenter-float2(0.25,0.5), ScreenCenter+float2(0.25, 0.5)) - tc))\n"
    "       return 0;\n"
    "   return Texture.Sample(Linear, tc);\n"
    "}\n";