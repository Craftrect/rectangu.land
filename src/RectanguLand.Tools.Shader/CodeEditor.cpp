#include "StdAfx.h"
#include "CodeEditor.h"

String CodeEditor::gKeywords = 
	L"AppendStructuredBuffer asm asm_fragment BlendState bool break Buffer ByteAddressBuffer "
	L"cbuffer centroid class clipplanes column_major compile compile_Fragment CompileShader const continue "
	L"ComputeShader ConsumeStrcturedBuffer DepthStencilState DepthStencilView discard do double DomainShader dword "
	L"else extern false float for fxgroup GeometryShader groupshared half Hullshader if in inline inout InputPatch int "
	L"interface line lineadj linear LineStream matrix min16float min10float min16int min12int min16uint namespace "
	L"nointerpolation noperspective NULL out OutputPatch packoffset pass pixelfragment PixelShader point PointStream precise "
	L"RasterizerState RenderTargetView return register row_major RWBuffer RWByteAddressBuffer RWStructuredBuffer RWTexture1D "
	L"RWTexture1DArray RWTexture2D RWTexture2DArray RWTexture3D sample sampler SamplerState SamplerComparisonState shared snorm "
	L"stateblock stateblock_state static string struct switch StructuredBuffer tbuffer technique technique10 technique11 texture "
	L"Texture1D Texture1DArray Texture2D Texture2DArray Texture2DMS Texture2DMSArray Texture3D TextureCube TextureCubeArray true "
	L"typedef triangle triangleadj TriangleStream uniform unorm uint vector vertexfragment VertexShader void volatile while "
	L"float1 float2 float3 float4 float1x1 float1x2 float1x3 float1x4 float2x1 float2x2 float2x3 float2x4 float3x1 float3x2 "
	L"float3x3 float3x4 float4x1 float4x2 float4x3 float4x4 "
	L"int1 int2 int3 int4 int1x1 int1x2 int1x3 int1x4 int2x1 int2x2 int2x3 int2x4 int3x1 int3x2 "
	L"int3x3 int3x4 int4x1 int4x2 int4x3 int4x4 "
	L"uint1 uint2 uint3 uint4 uint1x1 uint1x2 uint1x3 uint1x4 uint2x1 uint2x2 uint2x3 uint2x4 uint3x1 uint3x2 "
	L"uint3x3 uint3x4 uint4x1 uint4x2 uint4x3 uint4x4 "
	L"bool1 bool2 bool3 bool4 bool1x1 bool1x2 bool1x3 bool1x4 bool2x1 bool2x2 bool2x3 bool2x4 bool3x1 bool3x2 "
	L"bool3x3 bool3x4 bool4x1 bool4x2 bool4x3 bool4x4 "
	L"min10float1 min10float2 min10float3 min10float4 min10float1x1 min10float1x2 min10float1x3 min10float1x4 min10float2x1 min10float2x2 min10float2x3 min10float2x4 min10float3x1 min10float3x2 "
	L"min10float3x3 min10float3x4 min10float4x1 min10float4x2 min10float4x3 min10float4x4 "
	L"min16float1 min16float2 min16float3 min16float4 min16float1x1 min16float1x2 min16float1x3 min16float1x4 min16float2x1 min16float2x2 min16float2x3 min16float2x4 min16float3x1 min16float3x2 "
	L"min16float3x3 min16float3x4 min16float4x1 min16float4x2 min16float4x3 min16float4x4 "
	L"min12int1 min12int2 min12int3 min12int4 min12int1x1 min12int1x2 min12int1x3 min12int1x4 min12int2x1 min12int2x2 min12int2x3 min12int2x4 min12int3x1 min12int3x2 "
	L"min12int3x3 min12int3x4 min12int4x1 min12int4x2 min12int4x3 min12int4x4 "
	L"min16int1 min16int2 min16int3 min16int4 min16int1x1 min16int1x2 min16int1x3 min16int1x4 min16int2x1 min16int2x2 min16int2x3 min16int2x4 min16int3x1 min16int3x2 "
	L"min16int3x3 min16int3x4 min16int4x1 min16int4x2 min16int4x3 min16int4x4 "
	L"min16uint1 min16uint2 min16uint3 min16uint4 min16uint1x1 min16uint1x2 min16uint1x3 min16uint1x4 min16uint2x1 min16uint2x2 min16uint2x3 min16uint2x4 min16uint3x1 min16uint3x2 "
	L"min16uint3x3 min16uint3x4 min16uint4x1 min16uint4x2 min16uint4x3 min16uint4x4";

String intrinsics =
	L"abort abs acos all AllMemoryBarrier AllMemoryBarrierWithGroupSync any asdouble asfloat asin asint asuint atan atan2 ceil clamp clip cos cosh countbits cross "
	L"D3DCOLORtoUBYTE4 ddx ddx_coarse ddx_fine ddy ddy_coarse ddy_fine degrees determinant DeviceMemoryBarrier DeviceMemoryBarrierWithGroupSync distance dot dst "
	L"errorf EvaluateAttributeAtCentroid EvaluateAttributeAtSample EvaluateAttributeSnapped exp exp2 f16tof32 f32tof16 faceforward firstbithigh firstbitlow floor "
	L"fma fmod frac frexp fwidth GetRenderTargetSampleCount GetRenderTargetSamplePosition GroupMemoryBarrier GroupMemoryBarrierWithGroupSync InterlockedAdd InterlockedAnd "
	L"InterlockedCompareExchange InterlockedCompareStore InterlockedExchange InterlockedMax InterlockedMin InterlockedOr InterlockedXor isfinite isinf isnan ldexp length "
	L"lerp lit log log10 log2 mad max min modf msad4 mul noise normalize pow printf Process2DQuadTessFactorsAvg Process2DQuadTessFactorsMax Process2DQuadTessFactorsMin "
	L"ProcessIsolineTessFactors ProcessQuadTessFactorsAvg ProcessQuadTessFactorsMax ProcessQuadTessFactorsMin ProcessTriTessFactorsAvg ProcessTriTessFactorsMax ProcessTriTessFactorsMin "
	L"radians rcp reflect refract reversebits round rsqrt saturate sign sin sincos sinh smoothstep sqrt step tan tanh tex1D tex1Dbias tex1Dgrad tex1Dlod tex1Dproj tex2D "
	L"tex2Dbias tex2Dgrad tex2Dlod tex2Dproj tex3D tex3Dbias tex3Dgard tex3Dlod tex3Dproj texCUBE texCUBEbias texCUBEgrad texCUBElod texCUBEproj transpose trunc "
	L"POSITION0 POSITION1 POSITION2 POSITION3 POSITION4 POSITION5 POSITION6 POSITION7 POSITION8 POSITION9 POSITIONT TEXCOORD0 TEXCOORD1 TEXCOORD2 TEXCOORD3 TEXCOORD4 "
	L"TEXCOORD5 TEXCOORD6 TEXCOORD7 TEXCOORD8 TEXCOORD9 COLOR0 COLOR1 COLOR2 COLOR3 COLOR4 COLOR5 COLOR6 COLOR7 COLOR8 COLOR9 FOG NORMAL0 NORMAL1 NORMAL2 NORMAL3 NORMAL4 NORMAL5 "
	L"NORMAL6 NORMAL7 NORMAL8 NORMAL9 SV_POSITION SV_TARGET";

CodeEditor::CodeEditor(HWND hWindow, EditorWindowWeakPtr editor) : mWindow(hWindow), mEditor(editor) {

}

void CodeEditor::sendStyle(int style, COLORREF fore, COLORREF back, int32 size, const String& face) {
	send(SCI_STYLESETFORE, style, fore);
	send(SCI_STYLESETBACK, style, back);

	if (size >= 1) {
		send(SCI_STYLESETSIZE, style, size);
	}

	if (face.empty() == false) {
		send(SCI_STYLESETFONT, style, (LPARAM) face.toMultibyte().c_str());
	}
}

void CodeEditor::resize(uint32 w, uint32 h) {
	SetWindowPos(mWindow, nullptr, 250, 0, w, h - 150, SWP_NOZORDER);
}

String CodeEditor::getCode() {
	uint32 len = SendMessage(mWindow, SCI_GETTEXTLENGTH, 0, 0);
	std::vector<char> str(len + 2);
	SendMessage(mWindow, SCI_GETTEXT, len + 1, (LPARAM)str.data());
	return String(str.data());
}

void CodeEditor::setAnnotations(const std::list<ShaderCompiler::ShaderError>& errors) {
	send(SCI_ANNOTATIONCLEARALL);

	for (auto& err : errors) {
		if (err.line == 0) {
			continue;
		}

		send(SCI_ANNOTATIONSETTEXT, err.line - 1, (LPARAM) err.message.toMultibyte().c_str());
		send(SCI_ANNOTATIONSETSTYLE, err.line - 1, StyleErrorAnnotation);
	}
}

void CodeEditor::initDefaultStyle() {
	send(SCI_SETLEXER, SCLEX_CPP);
	send(SCI_SETSTYLEBITS, 7);
	send(SCI_SETWRAPMODE, SC_WRAP_WORD);

	send(SCI_SETKEYWORDS, 0, (LPARAM) gKeywords.toMultibyte().c_str());
	send(SCI_SETKEYWORDS, 1, (LPARAM) intrinsics.toMultibyte().c_str());
	sendStyle(STYLE_DEFAULT, 0, RGB(0xFF, 0xFF, 0xFF) , 11, L"Consolas");
	send(SCI_STYLECLEARALL);

	sendStyle(SCE_C_COMMENTLINE, RGB(0x22, 0x8B, 0x22), RGB(0xFF, 0xFF, 0xFF), 11, L"Palatino Linotype");
	sendStyle(SCE_C_COMMENT, RGB(0x22, 0x8B, 0x22), RGB(0xFF, 0xFF, 0xFF), 11, L"Palatino Linotype");
	send(SCI_STYLESETITALIC, SCE_C_COMMENTLINE, TRUE);
	send(SCI_STYLESETITALIC, SCE_C_COMMENT, TRUE);
	send(SCI_STYLESETBOLD, SCE_C_COMMENTLINE, TRUE);
	send(SCI_STYLESETBOLD, SCE_C_COMMENT, TRUE);

	send(SCI_STYLESETBOLD, SCE_C_NUMBER, TRUE);

	sendStyle(SCE_C_WORD, RGB(0x00, 0x00, 0x8B));
	send(SCI_STYLESETBOLD, SCE_C_WORD, TRUE);

	send(SCI_STYLESETBOLD, SCE_C_WORD2, TRUE);
	send(SCI_STYLESETITALIC, SCE_C_IDENTIFIER, TRUE);
	//send(SCI_SETCARETLINEVISIBLE, TRUE);
	//send(SCI_SETCARETLINEBACK, RGB(0xCF, 0xCF, 0xCF));

	send(SCI_STYLESETSIZE, STYLE_LINENUMBER, 11);
	send(SCI_STYLESETBOLD, STYLE_LINENUMBER, TRUE);

	send(SCI_STYLESETFORE, 66, RGB(0xFF, 0x00, 0x00));
	send(SCI_STYLESETBACK, 66, RGB(0xFF, 0xC1, 0xC1));
	send(SCI_STYLESETBOLD, 66, TRUE);

	send(SCI_SETTEXT, 0, (LPARAM)"float3 CameraPosition;\n\nfloat4 pixelShaderMain(float4 pos : POSITION0) {\n\tfloat3 dist = distance(pos.xyz, CameraPosition);\n\treturn lerp(float4(0, 0, 0, 0), float4(1, 1, 1, 1), length(dist));\n}");
	send(SCI_ANNOTATIONSETVISIBLE, ANNOTATION_BOXED);

	send(SCI_SETMARGINWIDTHN, 0, 0);
	send(SCI_SETMARGINWIDTHN, 1, 0);
	send(SCI_SETMARGINWIDTHN, 2, 0);

	send(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
	send(SCI_SETMARGINWIDTHN, 1, 5);
	send(SCI_SETMARGINWIDTHN, 2, 5);

	int margin_size = send(SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM) "_9");
	send(SCI_SETMARGINWIDTHN, 0, margin_size);
}

/*
float3 CameraPosition;

float4 pixelShaderMain(float4 pos : POSITION0) {
float3 dist = distance(pos.xyz - CameraPositioni);
return lerp(float4(0, 0, 0, 0), float4(1, 1, 1, 1), length(dist));
}

*/