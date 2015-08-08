////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Source File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeRenderer.cpp
//  Version:     v1.00
//  Created:     13/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#include "VeMainPCH.h"
#include "_VeRendererGL.h"
#include "_VeRendererGLES2.h"
#include "_VeRendererD3D11.h"
#ifdef VE_NO_INLINE
#	include "VeRenderer.inl"
#endif

//--------------------------------------------------------------------------
VeRenderer::VeRenderer()
	: m_u32Width(0), m_u32Height(0)
	, m_f32Width(0), m_f32Height(0)
	, m_f32InvWidth(0), m_f32InvHeight(0)
	, m_bInited(false)
{
	g_pResourceManager->SetResourceCreateFunction(VeResource::VERTEX_SHADER,
		VeShaderResource::CreateVertexShaderResource);
	g_pResourceManager->SetResourceCreateFunction(VeResource::PIXEL_SHADER,
		VeShaderResource::CreatePixelShaderResource);

	m_kRenderObjectTypeParser["blendstate"] = VeRenderObject::TYPE_BLEND_STATE;
	m_kRenderObjectTypeParser["counter"] = VeRenderObject::TYPE_COUNTER;
	m_kRenderObjectTypeParser["depthstencilstate"] = VeRenderObject::TYPE_DEPTH_STENCIL_STATE;
	m_kRenderObjectTypeParser["inputlayout"] = VeRenderObject::TYPE_INPUT_LAYOUT;
	m_kRenderObjectTypeParser["predicate"] = VeRenderObject::TYPE_PREDICATE;
	m_kRenderObjectTypeParser["query"] = VeRenderObject::TYPE_QUERY;
	m_kRenderObjectTypeParser["rasterizerstate"] = VeRenderObject::TYPE_RASTERIZER_STATE;
	m_kRenderObjectTypeParser["samplerstate"] = VeRenderObject::TYPE_SAMPLER_STATE;
	m_kRenderObjectTypeParser["buffer"] = VeRenderObject::TYPE_BUFFER;
	m_kRenderObjectTypeParser["depthstencilview"] = VeRenderObject::TYPE_DEPTH_STENCIL_VIEW;
	m_kRenderObjectTypeParser["rendertargetview"] = VeRenderObject::TYPE_RENDER_TARGET_VIEW;
	m_kRenderObjectTypeParser["shaderresourceview"] = VeRenderObject::TYPE_SHADER_RESOURCE_VIEW;
	m_kRenderObjectTypeParser["texture1d"] = VeRenderObject::TYPE_TEXTURE_1D;
	m_kRenderObjectTypeParser["texture2d"] = VeRenderObject::TYPE_TEXTURE_2D;
	m_kRenderObjectTypeParser["texture3d"] = VeRenderObject::TYPE_TEXTURE_3D;
	m_kRenderObjectTypeParser["unorderedaccessview"] = VeRenderObject::TYPE_UNORDERED_ACCESS_VIEW;
	m_kRenderObjectTypeParser["classlinkage"] = VeRenderObject::TYPE_CLASS_LINKAGE;
	m_kRenderObjectTypeParser["computeshader"] = VeRenderObject::TYPE_COMPUTE_SHADER;
	m_kRenderObjectTypeParser["domainshader"] = VeRenderObject::TYPE_DOMAIN_SHADER;
	m_kRenderObjectTypeParser["geometryshader"] = VeRenderObject::TYPE_GEOMETRY_SHADER;
	m_kRenderObjectTypeParser["hullshader"] = VeRenderObject::TYPE_HULL_SHADER;
	m_kRenderObjectTypeParser["pixelshader"] = VeRenderObject::TYPE_PIXEL_SHADER;
	m_kRenderObjectTypeParser["vertexshader"] = VeRenderObject::TYPE_VERTEX_SHADER;

	m_kBlendParser["zero"] = BLEND_ZERO;
	m_kBlendParser["one"] = BLEND_ONE;
	m_kBlendParser["src_color"] = BLEND_SRC_COLOR;
	m_kBlendParser["inv_src_color"] = BLEND_INV_SRC_COLOR;
	m_kBlendParser["src_alpha"] = BLEND_SRC_ALPHA;
	m_kBlendParser["inv_src_alpha"] = BLEND_INV_SRC_ALPHA;
	m_kBlendParser["dst_alpha"] = BLEND_DEST_ALPHA;
	m_kBlendParser["inv_dst_alpha"] = BLEND_INV_DEST_ALPHA;
	m_kBlendParser["dst_color"] = BLEND_DEST_COLOR;
	m_kBlendParser["inv_dst_color"] = BLEND_INV_DEST_COLOR;
	m_kBlendParser["src_alpha_sat"] = BLEND_SRC_ALPHA_SAT;
	m_kBlendParser["blend_factor"] = BLEND_BLEND_FACTOR;
	m_kBlendParser["inv_blend_factor"] = BLEND_INV_BLEND_FACTOR;
	m_kBlendParser["src1_color"] = BLEND_SRC1_COLOR;
	m_kBlendParser["inv_src1_color"] = BLEND_INV_SRC1_COLOR;
	m_kBlendParser["src1_alpha"] = BLEND_SRC1_ALPHA;
	m_kBlendParser["inv_src1_alpha"] = BLEND_INV_SRC1_ALPHA;

	m_kBlendOpParser["add"] = BLEND_OP_ADD;
	m_kBlendOpParser["sub"] = BLEND_OP_SUBTRACT;
	m_kBlendOpParser["rev_sub"] = BLEND_OP_REV_SUBTRACT;
	m_kBlendOpParser["min"] = BLEND_OP_MIN;
	m_kBlendOpParser["max"] = BLEND_OP_MAX;

	m_kColorWriteParser["r"] = COLOR_WRITE_ENABLE_RED;
	m_kColorWriteParser["g"] = COLOR_WRITE_ENABLE_GREEN;
	m_kColorWriteParser["b"] = COLOR_WRITE_ENABLE_BLUE;
	m_kColorWriteParser["a"] = COLOR_WRITE_ENABLE_ALPHA;
	m_kColorWriteParser["all"] = COLOR_WRITE_ENABLE_ALL;

	m_kComparisonFuncParser["never"] = COMPARISON_NEVER;
	m_kComparisonFuncParser["less"] = COMPARISON_LESS;
	m_kComparisonFuncParser["equal"] = COMPARISON_EQUAL;
	m_kComparisonFuncParser["less_equal"] = COMPARISON_LESS_EQUAL;
	m_kComparisonFuncParser["greater"] = COMPARISON_GREATER;
	m_kComparisonFuncParser["not_equal"] = COMPARISON_NOT_EQUAL;
	m_kComparisonFuncParser["greater_equal"] = COMPARISON_GREATER_EQUAL;
	m_kComparisonFuncParser["always"] = COMPARISON_ALWAYS;

	m_kStencilOpParser["keep"] = STENCIL_OP_KEEP;
	m_kStencilOpParser["zero"] = STENCIL_OP_ZERO;
	m_kStencilOpParser["replace"] = STENCIL_OP_REPLACE;
	m_kStencilOpParser["inc_sat"] = STENCIL_OP_INCR_SAT;
	m_kStencilOpParser["dec_sat"] = STENCIL_OP_DECR_SAT;
	m_kStencilOpParser["invert"] = STENCIL_OP_INVERT;
	m_kStencilOpParser["inc"] = STENCIL_OP_INCR;
	m_kStencilOpParser["dec"] = STENCIL_OP_DECR;

	m_kSemanticParser["position"] = SE_POSITION;
	m_kSemanticParser["normal"] = SE_NORMAL;
	m_kSemanticParser["color"] = SE_COLOR;
	m_kSemanticParser["texcoord"] = SE_TEXCOORD;
	m_kSemanticParser["tangent"] = SE_TANGENT;
	m_kSemanticParser["binormal"] = SE_BINORMAL;
	m_kSemanticParser["blendindices"] = SE_BLENDINDICES;
	m_kSemanticParser["blendweight"] = SE_BLENDWEIGHT;
	m_kSemanticParser["pos"] = SE_POSITION;
	m_kSemanticParser["nor"] = SE_NORMAL;
	m_kSemanticParser["col"] = SE_COLOR;
	m_kSemanticParser["tex"] = SE_TEXCOORD;
	m_kSemanticParser["tan"] = SE_TANGENT;
	m_kSemanticParser["bin"] = SE_BINORMAL;

	m_kFormatParser["unknown"] = FMT_UNKNOWN;
	m_kFormatParser["r32g32b32a32_typeless"] = FMT_R32G32B32A32_TYPELESS;
	m_kFormatParser["r32g32b32a32_float"] = FMT_R32G32B32A32_FLOAT;
	m_kFormatParser["r32g32b32a32_uint"] = FMT_R32G32B32A32_UINT;
	m_kFormatParser["r32g32b32a32_sint"] = FMT_R32G32B32A32_SINT;
	m_kFormatParser["r32g32b32_typeless"] = FMT_R32G32B32_TYPELESS;
	m_kFormatParser["r32g32b32_float"] = FMT_R32G32B32_FLOAT;
	m_kFormatParser["r32g32b32_uint"] = FMT_R32G32B32_UINT;
	m_kFormatParser["r32g32b32_sint"] = FMT_R32G32B32_SINT;
	m_kFormatParser["r16g16b16a16_typeless"] = FMT_R16G16B16A16_TYPELESS;
	m_kFormatParser["r16g16b16a16_float"] = FMT_R16G16B16A16_FLOAT;
	m_kFormatParser["r16g16b16a16_unorm"] = FMT_R16G16B16A16_UNORM;
	m_kFormatParser["r16g16b16a16_uint"] = FMT_R16G16B16A16_UINT;
	m_kFormatParser["r16g16b16a16_snorm"] = FMT_R16G16B16A16_SNORM;
	m_kFormatParser["r16g16b16a16_sint"] = FMT_R16G16B16A16_SINT;
	m_kFormatParser["r32g32_typeless"] = FMT_R32G32_TYPELESS;
	m_kFormatParser["r32g32_float"] = FMT_R32G32_FLOAT;
	m_kFormatParser["r32g32_uint"] = FMT_R32G32_UINT;
	m_kFormatParser["r32g32_sint"] = FMT_R32G32_SINT;
	m_kFormatParser["r32g8x24_typeless"] = FMT_R32G8X24_TYPELESS;
	m_kFormatParser["d32_float_s8x24_uint"] = FMT_D32_FLOAT_S8X24_UINT;
	m_kFormatParser["r32_float_x8x24_typeless"] = FMT_R32_FLOAT_X8X24_TYPELESS;
	m_kFormatParser["x32_typeless_g8x24_uint"] = FMT_X32_TYPELESS_G8X24_UINT;
	m_kFormatParser["r10g10b10a2_typeless"] = FMT_R10G10B10A2_TYPELESS;
	m_kFormatParser["r10g10b10a2_unorm"] = FMT_R10G10B10A2_UNORM;
	m_kFormatParser["r10g10b10a2_uint"] = FMT_R10G10B10A2_UINT;
	m_kFormatParser["r11g11b10_float"] = FMT_R11G11B10_FLOAT;
	m_kFormatParser["r8g8b8a8_typeless"] = FMT_R8G8B8A8_TYPELESS;
	m_kFormatParser["r8g8b8a8_unorm"] = FMT_R8G8B8A8_UNORM;
	m_kFormatParser["r8g8b8a8_unorm_srgb"] = FMT_R8G8B8A8_UNORM_SRGB;
	m_kFormatParser["r8g8b8a8_uint"] = FMT_R8G8B8A8_UINT;
	m_kFormatParser["r8g8b8a8_snorm"] = FMT_R8G8B8A8_SNORM;
	m_kFormatParser["r8g8b8a8_sint"] = FMT_R8G8B8A8_SINT;
	m_kFormatParser["r16g16_typeless"] = FMT_R16G16_TYPELESS;
	m_kFormatParser["r16g16_float"] = FMT_R16G16_FLOAT;
	m_kFormatParser["r16g16_unorm"] = FMT_R16G16_UNORM;
	m_kFormatParser["r16g16_uint"] = FMT_R16G16_UINT;
	m_kFormatParser["r16g16_snorm"] = FMT_R16G16_SNORM;
	m_kFormatParser["r16g16_sint"] = FMT_R16G16_SINT;
	m_kFormatParser["r32_typeless"] = FMT_R32_TYPELESS;
	m_kFormatParser["d32_float"] = FMT_D32_FLOAT;
	m_kFormatParser["r32_float"] = FMT_R32_FLOAT;
	m_kFormatParser["r32_uint"] = FMT_R32_UINT;
	m_kFormatParser["r32_sint"] = FMT_R32_SINT;
	m_kFormatParser["r24g8_typeless"] = FMT_R24G8_TYPELESS;
	m_kFormatParser["d24_unorm_s8_uint"] = FMT_D24_UNORM_S8_UINT;
	m_kFormatParser["r24_unorm_x8_typeless"] = FMT_R24_UNORM_X8_TYPELESS;
	m_kFormatParser["x24_typeless_g8_uint"] = FMT_X24_TYPELESS_G8_UINT;
	m_kFormatParser["r8g8_typeless"] = FMT_R8G8_TYPELESS;
	m_kFormatParser["r8g8_unorm"] = FMT_R8G8_UNORM;
	m_kFormatParser["r8g8_uint"] = FMT_R8G8_UINT;
	m_kFormatParser["r8g8_snorm"] = FMT_R8G8_SNORM;
	m_kFormatParser["r8g8_sint"] = FMT_R8G8_SINT;
	m_kFormatParser["r16_typeless"] = FMT_R16_TYPELESS;
	m_kFormatParser["r16_float"] = FMT_R16_FLOAT;
	m_kFormatParser["d16_unorm"] = FMT_D16_UNORM;
	m_kFormatParser["r16_unorm"] = FMT_R16_UNORM;
	m_kFormatParser["r16_uint"] = FMT_R16_UINT;
	m_kFormatParser["r16_snorm"] = FMT_R16_SNORM;
	m_kFormatParser["r16_sint"] = FMT_R16_SINT;
	m_kFormatParser["r8_typeless"] = FMT_R8_TYPELESS;
	m_kFormatParser["r8_unorm"] = FMT_R8_UNORM;
	m_kFormatParser["r8_uint"] = FMT_R8_UINT;
	m_kFormatParser["r8_snorm"] = FMT_R8_SNORM;
	m_kFormatParser["r8_sint"] = FMT_R8_SINT;
	m_kFormatParser["a8_unorm"] = FMT_A8_UNORM;
	m_kFormatParser["r1_unorm"] = FMT_R1_UNORM;
	m_kFormatParser["r9g9b9e5_sharedexp"] = FMT_R9G9B9E5_SHAREDEXP;
	m_kFormatParser["r8g8_b8g8_unorm"] = FMT_R8G8_B8G8_UNORM;
	m_kFormatParser["g8r8_g8b8_unorm"] = FMT_G8R8_G8B8_UNORM;
	m_kFormatParser["bc1_typeless"] = FMT_BC1_TYPELESS;
	m_kFormatParser["bc1_unorm"] = FMT_BC1_UNORM;
	m_kFormatParser["bc1_unorm_srgb"] = FMT_BC1_UNORM_SRGB;
	m_kFormatParser["bc2_typeless"] = FMT_BC2_TYPELESS;
	m_kFormatParser["bc2_unorm"] = FMT_BC2_UNORM;
	m_kFormatParser["bc2_unorm_srgb"] = FMT_BC2_UNORM_SRGB;
	m_kFormatParser["bc3_typeless"] = FMT_BC3_TYPELESS;
	m_kFormatParser["bc3_unorm"] = FMT_BC3_UNORM;
	m_kFormatParser["bc3_unorm_srgb"] = FMT_BC3_UNORM_SRGB;
	m_kFormatParser["bc4_typeless"] = FMT_BC4_TYPELESS;
	m_kFormatParser["bc4_unorm"] = FMT_BC4_UNORM;
	m_kFormatParser["bc4_snorm"] = FMT_BC4_SNORM;
	m_kFormatParser["bc5_typeless"] = FMT_BC5_TYPELESS;
	m_kFormatParser["bc5_unorm"] = FMT_BC5_UNORM;
	m_kFormatParser["bc5_snorm"] = FMT_BC5_SNORM;
	m_kFormatParser["b5g6r5_unorm"] = FMT_B5G6R5_UNORM;
	m_kFormatParser["b5g5r5a1_unorm"] = FMT_B5G5R5A1_UNORM;
	m_kFormatParser["b8g8r8a8_unorm"] = FMT_B8G8R8A8_UNORM;
	m_kFormatParser["b8g8r8x8_unorm"] = FMT_B8G8R8X8_UNORM;
	m_kFormatParser["r10g10b10_xr_bias_a2_unorm"] = FMT_R10G10B10_XR_BIAS_A2_UNORM;
	m_kFormatParser["b8g8r8a8_typeless"] = FMT_B8G8R8A8_TYPELESS;
	m_kFormatParser["b8g8r8a8_unorm_srgb"] = FMT_B8G8R8A8_UNORM_SRGB;
	m_kFormatParser["b8g8r8x8_typeless"] = FMT_B8G8R8X8_TYPELESS;
	m_kFormatParser["b8g8r8x8_unorm_srgb"] = FMT_B8G8R8X8_UNORM_SRGB;
	m_kFormatParser["bc6h_typeless"] = FMT_BC6H_TYPELESS;
	m_kFormatParser["bc6h_uf16"] = FMT_BC6H_UF16;
	m_kFormatParser["bc6h_sf16"] = FMT_BC6H_SF16;
	m_kFormatParser["bc7_typeless"] = FMT_BC7_TYPELESS;
	m_kFormatParser["bc7_unorm"] = FMT_BC7_UNORM;
	m_kFormatParser["bc7_unorm_srgb"] = FMT_BC7_UNORM_SRGB;

	m_kInputCalssParser["vertex_data"] = PER_VERTEX_DATA;
	m_kInputCalssParser["instance_data"] = PER_INSTANCE_DATA;

	m_kFillModeParser["wireframe"] = FILL_WIREFRAME;
	m_kFillModeParser["solid"] = FILL_SOLID;

	m_kCullModeParser["none"] = CULL_NONE;
	m_kCullModeParser["front"] = CULL_FRONT;
	m_kCullModeParser["back"] = CULL_BACK;

	m_kDepthBiasModeParser["none"] = DBIAS_NONE;

	m_kFilterParser["min_mag_mip_point"] = FILTER_MIN_MAG_MIP_POINT;
	m_kFilterParser["min_mag_point_mip_linear"] = FILTER_MIN_MAG_POINT_MIP_LINEAR;
	m_kFilterParser["min_point_mag_linear_mip_point"] = FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	m_kFilterParser["min_point_mag_mip_linear"] = FILTER_MIN_POINT_MAG_MIP_LINEAR;
	m_kFilterParser["min_linear_mag_mip_point"] = FILTER_MIN_LINEAR_MAG_MIP_POINT;
	m_kFilterParser["min_linear_mag_point_mip_linear"] = FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	m_kFilterParser["min_mag_linear_mip_point"] = FILTER_MIN_MAG_LINEAR_MIP_POINT;
	m_kFilterParser["min_mag_mip_linear"] = FILTER_MIN_MAG_MIP_LINEAR;
	m_kFilterParser["anisotropic"] = FILTER_ANISOTROPIC;
	m_kFilterParser["comparison_min_mag_mip_point"] = FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	m_kFilterParser["comparison_min_mag_point_mip_linear"] = FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	m_kFilterParser["comparison_min_point_mag_linear_mip_point"] = FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
	m_kFilterParser["comparison_min_point_mag_mip_linear"] = FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
	m_kFilterParser["comparison_min_linear_mag_mip_point"] = FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	m_kFilterParser["comparison_min_linear_mag_point_mip_linear"] = FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	m_kFilterParser["comparison_min_mag_linear_mip_point"] = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	m_kFilterParser["comparison_min_mag_mip_linear"] = FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	m_kFilterParser["comparison_anisotropic"] = FILTER_COMPARISON_ANISOTROPIC;

	m_kTexAddressModeParser["wrap"] = TEXTURE_ADDRESS_WRAP;
	m_kTexAddressModeParser["mirror"] = TEXTURE_ADDRESS_MIRROR;
	m_kTexAddressModeParser["clamp"] = TEXTURE_ADDRESS_CLAMP;
	m_kTexAddressModeParser["border"] = TEXTURE_ADDRESS_BORDER;
	m_kTexAddressModeParser["mirror_once"] = TEXTURE_ADDRESS_MIRROR_ONCE;

	m_kRenderTickTypeParser["iaindexbuffer"] = VeRenderTick::IA_INDEX_BUFFER;
	m_kRenderTickTypeParser["iainputlayout"] = VeRenderTick::IA_INPUT_LAYOUT;
	m_kRenderTickTypeParser["iaprimitivetopology"] = VeRenderTick::IA_PRIMITIVE_TOPOLOGY;
	m_kRenderTickTypeParser["iavertexbuffer"] = VeRenderTick::IA_VERTEX_BUFFER;
	m_kRenderTickTypeParser["vscbuffer"] = VeRenderTick::VS_CONSTANT_BUFFER;
	m_kRenderTickTypeParser["vssampler"] = VeRenderTick::VS_SAMPLER;
	m_kRenderTickTypeParser["vsshader"] = VeRenderTick::VS_SHADER;
	m_kRenderTickTypeParser["vsshaderres"] = VeRenderTick::VS_SHADER_RESOURCE;
	m_kRenderTickTypeParser["hscbuffer"] = VeRenderTick::HS_CONSTANT_BUFFER;
	m_kRenderTickTypeParser["hssampler"] = VeRenderTick::HS_SAMPLER;
	m_kRenderTickTypeParser["hsshader"] = VeRenderTick::HS_SHADER;
	m_kRenderTickTypeParser["hsshaderres"] = VeRenderTick::HS_SHADER_RESOURCE;
	m_kRenderTickTypeParser["dscbuffer"] = VeRenderTick::DS_CONSTANT_BUFFER;
	m_kRenderTickTypeParser["dssampler"] = VeRenderTick::DS_SAMPLER;
	m_kRenderTickTypeParser["dsshader"] = VeRenderTick::DS_SHADER;
	m_kRenderTickTypeParser["dsshaderres"] = VeRenderTick::DS_SHADER_RESOURCE;
	m_kRenderTickTypeParser["gscbuffer"] = VeRenderTick::GS_CONSTANT_BUFFER;
	m_kRenderTickTypeParser["gssampler"] = VeRenderTick::GS_SAMPLER;
	m_kRenderTickTypeParser["gsshader"] = VeRenderTick::GS_SHADER;
	m_kRenderTickTypeParser["gsshaderres"] = VeRenderTick::GS_SHADER_RESOURCE;
	m_kRenderTickTypeParser["sotarget"] = VeRenderTick::SO_TARGET;
	m_kRenderTickTypeParser["rsscissor"] = VeRenderTick::RS_SCISSOR_RECT;
	m_kRenderTickTypeParser["rsstate"] = VeRenderTick::RS_STATE;
	m_kRenderTickTypeParser["rsviewport"] = VeRenderTick::RS_VIEWPORT;
	m_kRenderTickTypeParser["pscbuffer"] = VeRenderTick::PS_CONSTANT_BUFFER;
	m_kRenderTickTypeParser["pssampler"] = VeRenderTick::PS_SAMPLER;
	m_kRenderTickTypeParser["psshader"] = VeRenderTick::PS_SHADER;
	m_kRenderTickTypeParser["psshaderres"] = VeRenderTick::PS_SHADER_RESOURCE;
	m_kRenderTickTypeParser["omblendstate"] = VeRenderTick::OM_BLEND_STATE;
	m_kRenderTickTypeParser["omdepthstencilstate"] = VeRenderTick::OM_DEPTH_STENCIL_STATE;
	m_kRenderTickTypeParser["omrendertargetview"] = VeRenderTick::OM_RENDER_TARGET_VIEW;
	m_kRenderTickTypeParser["omdepthstencilview"] = VeRenderTick::OM_DEPTH_STENCIL_VIEW;
	m_kRenderTickTypeParser["omunorderedaccessview"] = VeRenderTick::OM_UNORDERED_ACCESS_VIEW;

	m_kPrimitiveTopologyParser["undefined"] = VeIAPrimitiveTopology::TOPOLOGY_UNDEFINED;
	m_kPrimitiveTopologyParser["point_list"] = VeIAPrimitiveTopology::TOPOLOGY_POINTLIST;
	m_kPrimitiveTopologyParser["line_list"] = VeIAPrimitiveTopology::TOPOLOGY_LINELIST;
	m_kPrimitiveTopologyParser["line_strip"] = VeIAPrimitiveTopology::TOPOLOGY_LINESTRIP;
	m_kPrimitiveTopologyParser["triangle_list"] = VeIAPrimitiveTopology::TOPOLOGY_TRIANGLELIST;
	m_kPrimitiveTopologyParser["triangle_strip"] = VeIAPrimitiveTopology::TOPOLOGY_TRIANGLESTRIP;
}
//--------------------------------------------------------------------------
VeRenderer::~VeRenderer()
{
	g_pResourceManager->SetResourceCreateFunction(
		VeResource::VERTEX_SHADER, NULL);
	g_pResourceManager->SetResourceCreateFunction(
		VeResource::PIXEL_SHADER, NULL);
}
//--------------------------------------------------------------------------
void VeRenderer::CreateRenderer(API eType)
{
	VE_ASSERT(!GetSingletonPtr());
	switch(eType)
	{
	case API_D3D11:
#ifdef VE_ENABLE_D3D11
		VE_NEW VeRendererD3D11();
#endif
		break;
	case API_OGL:
#ifdef VE_ENABLE_GL
		VE_NEW VeRendererGL();
#endif
		break;
	case API_OGLES2:
#ifdef VE_ENABLE_GLES2
		VE_NEW VeRendererGLES2();
#endif
		break;
	default:
		break;
	}
	VE_ASSERT(GetSingletonPtr());
}
//--------------------------------------------------------------------------
void VeRenderer::DestoryRenderer()
{
	VE_ASSERT(GetSingletonPtr());
	Destory();
	VE_ASSERT(!GetSingletonPtr());
}
//--------------------------------------------------------------------------
void VeRenderer::AddObject(const VeRenderObjectPtr& spObject)
{
	VeRenderObject::Type eType = spObject->GetType();
	VE_ASSERT(eType < VeRenderObject::TYPE_MAX);
	VE_ASSERT(m_akObjectMap[eType].Find(spObject->GetName()) == NULL);
	m_akObjectMap[eType][spObject->GetName()] = m_kObjectArray.Size();
	m_kObjectArray.PushBack(spObject);
	spObject->OnAdd();
}
//--------------------------------------------------------------------------
void VeRenderer::DelObject(const VeRenderObjectPtr& spObject)
{
	VeRenderObject::Type eType = spObject->GetType();
	VE_ASSERT(eType < VeRenderObject::TYPE_MAX);
	VeStringMap<VeUInt32>& kMap = m_akObjectMap[eType];
	VeUInt32* pu32Index = kMap.Find(spObject->GetName());
	if(pu32Index)
	{
		VE_ASSERT(m_kObjectArray[*pu32Index] == spObject);
		if((*pu32Index) < (m_kObjectArray.Size() - 1))
		{
			VeStringMap<VeUInt32>& kBackMap =
				m_akObjectMap[m_kObjectArray.Back()->GetType()];
			kBackMap[m_kObjectArray.Back()->GetName()] = *pu32Index;
			m_kObjectArray[*pu32Index] = m_kObjectArray.Back();
		}
		kMap.Remove(spObject->GetName());
		spObject->OnDel();
		m_kObjectArray.PopBack();
	}
}
//--------------------------------------------------------------------------
void VeRenderer::AddNodeData(const VeRenderNodeDataPtr& spNodeData)
{
	VE_ASSERT(!m_kNodeDataMap.Find(spNodeData->m_kName));
	m_kNodeDataMap[spNodeData->m_kName] = m_kNodeDataArray.Size();
	m_kNodeDataArray.PushBack(spNodeData);
}
//--------------------------------------------------------------------------
void VeRenderer::DelNodeData(const VeRenderNodeDataPtr& spNodeData)
{
	VeUInt32* pu32Index = m_kNodeDataMap.Find(spNodeData->m_kName);
	if(pu32Index)
	{
		VE_ASSERT(m_kNodeDataArray[*pu32Index] == spNodeData);
		if((*pu32Index) < (m_kNodeDataArray.Size() - 1))
		{
			m_kNodeDataMap[m_kNodeDataArray.Back()->m_kName] = *pu32Index;
			m_kNodeDataArray[*pu32Index] = m_kNodeDataArray.Back();
		}
		m_kNodeDataMap.Remove(spNodeData->m_kName);
		m_kNodeDataArray.PopBack();
	}
}
//--------------------------------------------------------------------------
void VeRenderer::AddProcess(const VeRenderProcessPtr& spProcess)
{
	m_kProcessMap[spProcess->GetName()] = m_kProcessArray.Size();
	m_kProcessArray.PushBack(spProcess);
	spProcess->OnAdd();
}
//--------------------------------------------------------------------------
void VeRenderer::DelProcess(const VeRenderProcessPtr& spProcess)
{
	VeUInt32* pu32Index = m_kProcessMap.Find(spProcess->GetName());
	VE_ASSERT(pu32Index);
	VE_ASSERT(m_kProcessArray[*pu32Index] == spProcess);
	if((*pu32Index) < (m_kProcessArray.Size() - 1))
	{
		m_kProcessMap[m_kProcessArray.Back()->GetName()] = *pu32Index;
		m_kProcessArray[*pu32Index] = m_kProcessArray.Back();
	}
	m_kProcessMap.Remove(spProcess->GetName());
	spProcess->OnDel();
	m_kProcessArray.PopBack();
}
//--------------------------------------------------------------------------
const VeRenderObjectPtr& VeRenderer::GetRenderObject(
	VeRenderObject::Type eType, const VeChar8* pcName)
{
	VE_ASSERT(eType < VeRenderObject::TYPE_MAX);
	VeStringMap<VeUInt32>& kMap = m_akObjectMap[eType];
	VeUInt32* pu32Index = kMap.Find(pcName);
	if(pu32Index)
	{
		return m_kObjectArray[*pu32Index];
	}
	else
	{
		return VeRenderObjectPtr::ms_NULL;
	}
}
//--------------------------------------------------------------------------
const VeRenderNodeDataPtr& VeRenderer::GetRenderNodeData(
	const VeChar8* pcName)
{
	VeUInt32* pu32Index = m_kNodeDataMap.Find(pcName);
	if(pu32Index)
	{
		return m_kNodeDataArray[*pu32Index];
	}
	else
	{
		return VeRenderNodeDataPtr::ms_NULL;
	}
}
//--------------------------------------------------------------------------
VeUInt32 VeRenderer::GetMultisampleLevelCount()
{
	return 0;
}
//--------------------------------------------------------------------------
void VeRenderer::EnumMultisampleLevel(VeUInt32 u32Index,
	VeUInt32& u32Count, VeUInt32& u32Quality)
{
	VE_ASSERT(!"Unknown error!");
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateVertexShader(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROVertexShaderPtr* pspShader)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreatePixelShader(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROPixelShaderPtr* pspShader)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateBlendState(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROBlendStatePtr* pspBlendState)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateDepthStencilState(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeRODepthStencilStatePtr* pspDepthStencilState)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateInputLayout(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROInputLayoutPtr* pspLayout)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateRasterizerState(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeRORasterizeStatePtr* pspRasterizeState)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateBuffer(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROBufferPtr* pspBuffer)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateSamplerState(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROSamplerStatePtr* pspSamplerState)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateTexture2D(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeROTexture2DPtr* pspTexture)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateProcessByType(VeRenderProcess::Type eType,
	const VeChar8* pcName, VeBinaryIStream& kStream,
	VeRenderProcessPtr* pspProcess)
{
	return false;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateRenderNodeData(const VeChar8* pcName,
	VeBinaryIStream& kStream, VeRenderNodeDataPtr* pspNodeData)
{
	VeRenderNodeDataPtr spData = VE_NEW VeRenderNodeData(pcName);
	VeUInt32 u32Num;
	kStream >> u32Num;
	for(VeUInt32 i(0); i < u32Num; ++i)
	{
		VeRenderTick* pkTick = CreateRenderTick(kStream);
		if(!pkTick)
		{
			return false;
		}
		spData->AddRenderTick(*pkTick);
	}
	kStream >> u32Num;
	for(VeUInt32 i(0); i < u32Num; ++i)
	{
		VeRenderCall* pkCall = CreateRenderCall(kStream);
		if(!pkCall)
		{
			return false;
		}
		spData->AddRenderCall(*pkCall);
	}
	AddNodeData(spData);
	if(pspNodeData)
	{
		*pspNodeData = spData;
	}
	return true;
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateRenderNodeData(VeXMLElement* pkElement,
	VeRenderNodeDataPtr* pspNodeData)
{
	const VeChar8* pcName = pkElement->Value();
	VeRenderNodeDataPtr spData = VE_NEW VeRenderNodeData(pcName);
	VeXMLElement* pkTickElement = pkElement->FirstChildElement();
	while(pkTickElement)
	{
		VeRenderTick* pkTick = CreateRenderTick(pkTickElement);
		if(!pkTick)
		{
			return false;
		}
		spData->AddRenderTick(*pkTick);
		pkTickElement = pkTickElement->NextSiblingElement();
	}
	AddNodeData(spData);
	if(pspNodeData)
	{
		*pspNodeData = spData;
	}
	return true;
}
//--------------------------------------------------------------------------
VeRenderTick* VeRenderer::CreateRenderTick(VeBinaryIStream& kStream)
{
	VeRenderTick::Type eType;
	kStream >> eType;
	switch(eType)
	{
	case VeRenderTick::IA_INDEX_BUFFER:
		{
			VeStringA kName;
			VeUInt32 u32Format;
			kStream.GetStringAligned4(kName);
			kStream >> u32Format;
			return VE_NEW VeIAIndexBuffer(kName, u32Format);
		}
		return NULL;
	case VeRenderTick::IA_INPUT_LAYOUT:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			return VE_NEW VeIAInputLayout(kName);
		}
	case VeRenderTick::IA_PRIMITIVE_TOPOLOGY:
		{
			VeIAPrimitiveTopology::Topology eTopology;
			kStream >> eTopology;
			return VE_NEW VeIAPrimitiveTopology(eTopology);
		}
	case VeRenderTick::IA_VERTEX_BUFFER:
		{
			VeStringA kName;
			VeUInt16 u16Slot, u16Stride;
			kStream.GetStringAligned4(kName);
			kStream >> u16Slot;
			kStream >> u16Stride;
			return VE_NEW VeIAVertexBuffer(kName, u16Stride, u16Slot);
		}
	case VeRenderTick::VS_CONSTANT_BUFFER:
		{
			VeStringA kName;
			VeUInt32 u32Slot;
			kStream.GetStringAligned4(kName);
			kStream >> u32Slot;
			return VE_NEW VeVSCBuffer(kName, u32Slot);
		}
	case VeRenderTick::VS_SAMPLER:
		return NULL;
	case VeRenderTick::VS_SHADER:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			return VE_NEW VeVSShader(kName);
		}
	case VeRenderTick::VS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::HS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::HS_SAMPLER:
		return NULL;
	case VeRenderTick::HS_SHADER:
		return NULL;
	case VeRenderTick::HS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::DS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::DS_SAMPLER:
		return NULL;
	case VeRenderTick::DS_SHADER:
		return NULL;
	case VeRenderTick::DS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::GS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::GS_SAMPLER:
		return NULL;
	case VeRenderTick::GS_SHADER:
		return NULL;
	case VeRenderTick::GS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::SO_TARGET:
		return NULL;
	case VeRenderTick::RS_SCISSOR_RECT:
		{
			VeUInt32 u32Slot;
			VeUInt16 x, y, w, h;
			kStream >> u32Slot;
			kStream >> x;
			kStream >> y;
			kStream >> w;
			kStream >> h;
			return VE_NEW VeRSScissorRectTick(x, y, w, h, u32Slot);
		}
		return NULL;
	case VeRenderTick::RS_STATE:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			return VE_NEW VeRSState(kName);
		}
	case VeRenderTick::RS_VIEWPORT:
		{
			VeUInt32 u32Slot;
			kStream >> u32Slot;
			VeVector4D kViewport;
			kStream >> kViewport;
			return VE_NEW VeRSViewportTick(kViewport, u32Slot);
		}
	case VeRenderTick::PS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::PS_SAMPLER:
		{
			VeUInt32 u32Slot;
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			kStream >> u32Slot;
			return VE_NEW VePSSampler(kName, u32Slot);
		}
	case VeRenderTick::PS_SHADER:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			return VE_NEW VePSShader(kName);
		}
	case VeRenderTick::PS_SHADER_RESOURCE:
		{
			VeUInt32 u32Slot;
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			kStream >> u32Slot;
			return VE_NEW VePSShaderResource(kName, u32Slot);
		}
	case VeRenderTick::OM_BLEND_STATE:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			VeOMBlendStateTick* pkBlendState = VE_NEW VeOMBlendStateTick(kName);
			kStream >> (pkBlendState->m_af32Factor[0]);
			kStream >> (pkBlendState->m_af32Factor[1]);
			kStream >> (pkBlendState->m_af32Factor[2]);
			kStream >> (pkBlendState->m_af32Factor[3]);
			kStream >> (pkBlendState->m_u32SampleMask);
			return pkBlendState;
		}
	case VeRenderTick::OM_DEPTH_STENCIL_STATE:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			VeUInt32 u32StencilRef;
			kStream >> u32StencilRef;
			return VE_NEW VeOMDepthStencilStateTick(kName, u32StencilRef);
		}

	case VeRenderTick::OM_RENDER_TARGET_VIEW:
		{
			VeUInt32 u32Slot;
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			kStream >> u32Slot;
			return VE_NEW VeOMRenderTargetViewTick(kName, u32Slot);
		}
	case VeRenderTick::OM_DEPTH_STENCIL_VIEW:
		{
			VeStringA kName;
			kStream.GetStringAligned4(kName);
			return VE_NEW VeOMDepthStencilViewTick(kName);
		}
	case VeRenderTick::OM_UNORDERED_ACCESS_VIEW:
		return NULL;
	default:
		return NULL;
	}
}
//--------------------------------------------------------------------------
VeRenderTick* VeRenderer::CreateRenderTick(VeXMLElement* pkElement)
{
	VeRenderTick::Type eType = Parse(pkElement->Value(), VeRenderTick::TYPE_MAX);
	switch(eType)
	{
	case VeRenderTick::IA_INDEX_BUFFER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				Format eFormat = Parse(pkElement->GetAttribute("format"), FMT_R16_UINT);
				return VE_NEW VeIAIndexBuffer(pcName, eFormat);
			}
		}
	case VeRenderTick::IA_INPUT_LAYOUT:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				return VE_NEW VeIAInputLayout(pcName);
			}
		}
	case VeRenderTick::IA_PRIMITIVE_TOPOLOGY:
		{
			VeIAPrimitiveTopology::Topology eTopology = Parse(pkElement->GetAttribute(""), VeIAPrimitiveTopology::TOPOLOGY_UNDEFINED);
			return VE_NEW VeIAPrimitiveTopology(eTopology);
		}
	case VeRenderTick::IA_VERTEX_BUFFER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
				VeUInt32 u32Stride = pkElement->Attribute("stride", 0);
				return VE_NEW VeIAVertexBuffer(pcName, u32Stride, u32Slot);
			}
		}
	case VeRenderTick::VS_CONSTANT_BUFFER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
				return VE_NEW VeVSCBuffer(pcName, u32Slot);
			}
		}
	case VeRenderTick::VS_SAMPLER:
		return NULL;
	case VeRenderTick::VS_SHADER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				return VE_NEW VeVSShader(pcName);
			}
		}
	case VeRenderTick::VS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::HS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::HS_SAMPLER:
		return NULL;
	case VeRenderTick::HS_SHADER:
		return NULL;
	case VeRenderTick::HS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::DS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::DS_SAMPLER:
		return NULL;
	case VeRenderTick::DS_SHADER:
		return NULL;
	case VeRenderTick::DS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::GS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::GS_SAMPLER:
		return NULL;
	case VeRenderTick::GS_SHADER:
		return NULL;
	case VeRenderTick::GS_SHADER_RESOURCE:
		return NULL;
	case VeRenderTick::SO_TARGET:
		return NULL;
	case VeRenderTick::RS_SCISSOR_RECT:
		{
			VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
			VeVector4D kRect = pkElement->Attribute("rect", VeVector4D::ZERO);
			return VE_NEW VeRSScissorRectTick(VeUInt16(kRect.x), VeUInt16(kRect.y), VeUInt16(kRect.z), VeUInt16(kRect.w), u32Slot);
		}
	case VeRenderTick::RS_VIEWPORT:
		{
			VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
			VeVector4D kViewport = pkElement->Attribute("value", VeVector4D::ZERO);
			return VE_NEW VeRSViewportTick(kViewport, u32Slot);
		}
	case VeRenderTick::PS_CONSTANT_BUFFER:
		return NULL;
	case VeRenderTick::PS_SAMPLER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
				return VE_NEW VePSSampler(pcName, u32Slot);
			}
		}
	case VeRenderTick::PS_SHADER:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				return VE_NEW VePSShader(pcName);
			}
		}
	case VeRenderTick::PS_SHADER_RESOURCE:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
				return VE_NEW VePSShaderResource(pcName, u32Slot);
			}
		}
	case VeRenderTick::OM_BLEND_STATE:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeOMBlendStateTick* pkBlendState = VE_NEW VeOMBlendStateTick(pcName);
				VeColorA kFactor = pkElement->Attribute("factor", VeColorA::BLACK);
				VeMemoryCopy(pkBlendState->m_af32Factor, kFactor, sizeof(VeColorA));
				return pkBlendState;
			}
		}
	case VeRenderTick::OM_DEPTH_STENCIL_STATE:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32StencilRef = pkElement->Attribute("stencil_ref", 0);
				return VE_NEW VeOMDepthStencilStateTick(pcName, u32StencilRef);
			}
		}
	case VeRenderTick::OM_RENDER_TARGET_VIEW:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				VeUInt32 u32Slot = pkElement->Attribute("slot", 0);
				return VE_NEW VeOMRenderTargetViewTick(pcName, u32Slot);
			}
		}
	case VeRenderTick::OM_DEPTH_STENCIL_VIEW:
		{
			const VeChar8* pcName = pkElement->GetAttribute("name");
			if(pcName)
			{
				return VE_NEW VeOMDepthStencilViewTick(pcName);
			}
		}
	case VeRenderTick::OM_UNORDERED_ACCESS_VIEW:
		return NULL;
	default:
		return NULL;
	}
	return NULL;
}
//--------------------------------------------------------------------------
VeRenderCall* VeRenderer::CreateRenderCall(VeBinaryIStream& kStream)
{
	VeRenderCall::Type eType;
	kStream >> eType;
	switch(eType)
	{
	case VeRenderCall::CLEAR:
		{
			VeClearCall* pkCall = VE_NEW VeClearCall();
			kStream >> pkCall->m_bClearColor;
			kStream >> pkCall->m_bClearDepth;
			kStream >> pkCall->m_bClearStencil;
			kStream >> pkCall->m_u8Stencil;
			kStream >> pkCall->m_f32Depth;
			VeUInt32 u32ColorCount(0);
			kStream >> u32ColorCount;
			pkCall->m_kColorArray.Resize(u32ColorCount);
			for(VeUInt32 i(0); i < u32ColorCount; ++i)
			{
				kStream >> pkCall->m_kColorArray[i];
			}
			return pkCall;
		}
	case VeRenderCall::DRAW:
		{
			VeUInt32 u32Count, u32Location;
			kStream >> u32Count;
			kStream >> u32Location;
			return VE_NEW VeDrawCall(u32Count, u32Location);
		}
	case VeRenderCall::DRAW_INDEXED:
		{
			VeUInt32 u32IndexCount, u32StartIndex;
			kStream >> u32IndexCount;
			kStream >> u32StartIndex;
			return VE_NEW VeDrawIndexedCall(u32IndexCount, u32StartIndex);
		}
	case VeRenderCall::DRAW_INSTANCED:
	case VeRenderCall::DRAW_INDEXED_INSTANCED:
	case VeRenderCall::DRAW_INSTANCED_INDIRECT:
	case VeRenderCall::DRAW_INDEXED_INSTANCED_INDIRECT:
	case VeRenderCall::DRAW_AUTO:
	default:
		return NULL;
	}
}
//--------------------------------------------------------------------------
bool VeRenderer::MapRes(const VeRenderObjectPtr& spObject,
	VeUInt32 u32SubResource, Map eType, VE_BOOL bSync,
	MappedSubresource* pkMapped)
{
	VE_ASSERT(pkMapped);
	VeZeroMemory(pkMapped, sizeof(MappedSubresource));
	return false;
}
//--------------------------------------------------------------------------
void VeRenderer::UnMapRes(const VeRenderObjectPtr& spObject,
	VeUInt32 u32SubResource)
{

}
//--------------------------------------------------------------------------
bool VeRenderer::CreateRenderObject(VeBinaryIStream& kStream,
	VeRenderObjectPtr* pspRenderObject)
{
	VeUInt32 u32Type;
	kStream >> u32Type;
	VeStringA kName;
	kStream.GetStringAligned4(kName);
	switch(u32Type)
	{
	case VeRenderObject::TYPE_BLEND_STATE:
		return CreateBlendState(
			kName, kStream, (VeROBlendStatePtr*)pspRenderObject);
	case VeRenderObject::TYPE_DEPTH_STENCIL_STATE:
		return CreateDepthStencilState(
			kName, kStream, (VeRODepthStencilStatePtr*)pspRenderObject);
	case VeRenderObject::TYPE_RASTERIZER_STATE:
		return CreateRasterizerState(
			kName, kStream, (VeRORasterizeStatePtr*)pspRenderObject);
	case VeRenderObject::TYPE_SAMPLER_STATE:
		return CreateSamplerState(
			kName, kStream, (VeROSamplerStatePtr*)pspRenderObject);
	case VeRenderObject::TYPE_INPUT_LAYOUT:
		return CreateInputLayout(
			kName, kStream, (VeROInputLayoutPtr*)pspRenderObject);
	default:
		if(pspRenderObject)
		{
			*pspRenderObject = NULL;
		}
		return false;
	}
}
//--------------------------------------------------------------------------
bool VeRenderer::CreateRenderObject(VeXMLElement* pkElement,
	VeRenderObjectPtr* pspRenderObject)
{
	VeRenderObject::Type eType = Parse(pkElement->Value(), VeRenderObject::TYPE_MAX);
	const VeChar8* pcName = pkElement->GetAttribute("name");
	VE_ASSERT(pcName);
	VeMemoryOStream kStream;
	switch(eType)
	{
	case VeRenderObject::TYPE_BLEND_STATE:
		{
			bool bEnable = pkElement->Attribute("alpha_to_coverage", false);
			kStream << VeUInt16(bEnable ? VE_TRUE : VE_FALSE);
			VeUInt16 u16Num(0);
			VeXMLElement* pkRenderTarget = pkElement->FirstChildElement("RenderTarget");
			while(pkRenderTarget)
			{
				++u16Num;
				pkRenderTarget = pkRenderTarget->NextSiblingElement("RenderTarget");
			}
			kStream << u16Num;
			pkRenderTarget = pkElement->FirstChildElement("RenderTarget");
			while(pkRenderTarget)
			{
				bool bEnable = pkRenderTarget->Attribute("enable", false);
				kStream << VeUInt8(bEnable ? VE_TRUE : VE_FALSE);
				kStream << VeUInt8(Blend(Parse(pkRenderTarget->GetAttribute("blend_src"), BLEND_ONE)));
				kStream << VeUInt8(Blend(Parse(pkRenderTarget->GetAttribute("blend_dst"), BLEND_ZERO)));
				kStream << VeUInt8(BlendOp(Parse(pkRenderTarget->GetAttribute("blend_op"), BLEND_OP_ADD)));
				kStream << VeUInt8(Blend(Parse(pkRenderTarget->GetAttribute("blend_src_alpha"), BLEND_ONE)));
				kStream << VeUInt8(Blend(Parse(pkRenderTarget->GetAttribute("blend_dst_alpha"), BLEND_ZERO)));
				kStream << VeUInt8(BlendOp(Parse(pkRenderTarget->GetAttribute("blend_op_alpha"), BLEND_OP_ADD)));
				const VeChar8* pcWrite = pkRenderTarget->GetAttribute("write");
				if(pcWrite)
				{
					VeChar8 acWirte[128];
					VeStrcpy(acWirte, sizeof(acWirte), pcWrite);
					VeUInt8 u8Write(0);
					VeChar8* pcContext;
					const VeChar8 acSeps [] = "|";
					VeChar8* pcTemp = VeStrtok(acWirte, acSeps, &pcContext);
					while(pcTemp)
					{
						u8Write |= Parse(pcTemp, COLOR_WRITE_ENABLE_NULL);
						pcTemp = VeStrtok(NULL, acSeps, &pcContext);
					}
					kStream << u8Write;
				}
				else
				{
					kStream << VeUInt8(COLOR_WRITE_ENABLE_ALL);
				}
				pkRenderTarget = pkRenderTarget->NextSiblingElement("RenderTarget");
			}
			return CreateBlendState(pcName, kStream, (VeROBlendStatePtr*)pspRenderObject);
		}
		break;
	case VeRenderObject::TYPE_DEPTH_STENCIL_STATE:
		{
			kStream << VeUInt8(bool(pkElement->Attribute("enable", true)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(bool(pkElement->Attribute("write", true)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(ComparisonFunc(Parse(pkElement->GetAttribute("func"), COMPARISON_LESS)));
			bool bEnableStencil = pkElement->Attribute("stencil", false);
			kStream << VeUInt8(bEnableStencil ? VE_TRUE : VE_FALSE);
			if(bEnableStencil)
			{
				kStream << VeUInt8(pkElement->Attribute("stencil_read_mask", 0xff));
				kStream << VeUInt8(pkElement->Attribute("stencil_write_mask", 0xff));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("front_fail_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("front_dfail_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("front_pass_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(ComparisonFunc(Parse(pkElement->GetAttribute("front_func"), COMPARISON_ALWAYS)));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("back_fail_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("back_dfail_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(BlendOp(Parse(pkElement->GetAttribute("back_pass_op"), STENCIL_OP_KEEP)));
				kStream << VeUInt8(ComparisonFunc(Parse(pkElement->GetAttribute("back_func"), COMPARISON_ALWAYS)));
				kStream << VeUInt16(0);
			}
			return CreateDepthStencilState(pcName, kStream, (VeRODepthStencilStatePtr*)pspRenderObject);
		}
		break;
	case VeRenderObject::TYPE_INPUT_LAYOUT:
		{
			const VeChar8* pcShader = pkElement->GetAttribute("shader");
			VE_ASSERT(pcShader);
			kStream.AppendStringAligned4(pcShader, (VeInt32)VeStrlen(pcShader));
			VeUInt32 u32Num(0);
			VeXMLElement* pkInputElement = pkElement->FirstChildElement("Element");
			while(pkInputElement)
			{
				++u32Num;
				pkInputElement = pkInputElement->NextSiblingElement("Element");
			}
			kStream << u32Num;
			pkInputElement = pkElement->FirstChildElement("Element");
			while(pkInputElement)
			{
				kStream << VeUInt8(Semantic(Parse(pkInputElement->GetAttribute("semantic"), SE_TEXCOORD)));
				kStream << VeUInt8(pkInputElement->Attribute("index", 0));
				kStream << VeUInt8(Format(Parse(pkInputElement->GetAttribute("format"), FMT_UNKNOWN)));
				kStream << VeUInt8(pkInputElement->Attribute("slot", 0));
				kStream << VeUInt8(pkInputElement->Attribute("offset", 0));
				kStream << VeUInt8(InputClassification(Parse(pkInputElement->GetAttribute("class"), PER_VERTEX_DATA)));
				kStream << VeUInt16(pkInputElement->Attribute("rate", 0));
				pkInputElement = pkInputElement->NextSiblingElement("Element");
			}
			return CreateInputLayout(pcName, kStream, (VeROInputLayoutPtr*)pspRenderObject);
		}
		break;
	case VeRenderObject::TYPE_RASTERIZER_STATE:
		{
			kStream << VeUInt8(FillMode(Parse(pkElement->GetAttribute("fill_mode"), FILL_SOLID)));
			kStream << VeUInt8(CullMode(Parse(pkElement->GetAttribute("cull_mode"), CULL_BACK)));
			kStream << VeUInt8(bool(pkElement->Attribute("ccw", false)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(DepthBiasMode(Parse(pkElement->GetAttribute("depth_bias"), DBIAS_NONE)));
			kStream << VeUInt8(bool(pkElement->Attribute("depth_clip", true)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(bool(pkElement->Attribute("scissor", false)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(bool(pkElement->Attribute("msaa", false)) ? VE_TRUE : VE_FALSE);
			kStream << VeUInt8(bool(pkElement->Attribute("aa_line", false)) ? VE_TRUE : VE_FALSE);
			return CreateRasterizerState(pcName, kStream, (VeRORasterizeStatePtr*)pspRenderObject);
		}
		break;
	case VeRenderObject::TYPE_SAMPLER_STATE:
		{
			kStream << VeUInt8(Filter(Parse(pkElement->GetAttribute("filter"), FILTER_MIN_MAG_MIP_LINEAR)));
			kStream << VeUInt8(TextureAddressMode(Parse(pkElement->GetAttribute("address_u"), TEXTURE_ADDRESS_CLAMP)));
			kStream << VeUInt8(TextureAddressMode(Parse(pkElement->GetAttribute("address_v"), TEXTURE_ADDRESS_CLAMP)));
			kStream << VeUInt8(TextureAddressMode(Parse(pkElement->GetAttribute("address_w"), TEXTURE_ADDRESS_CLAMP)));
			kStream << VeUInt16(pkElement->Attribute("anisotropy", 16));
			kStream << VeUInt16(ComparisonFunc(Parse(pkElement->GetAttribute("comp_func"), COMPARISON_NEVER)));
			kStream << VeFloat32(pkElement->Attribute("mip_bias", 0));
			kStream << VeFloat32(pkElement->Attribute("lod_min", -VE_MAXFLOAT_F));
			kStream << VeFloat32(pkElement->Attribute("lod_max", VE_MAXFLOAT_F));
			VeColorA kBorder = pkElement->Attribute("border", VeColorA::BLACK);
			kStream << kBorder;
			return CreateSamplerState(pcName, kStream, (VeROSamplerStatePtr*)pspRenderObject);
		}
		break;
	default:
		break;
	}

	return false;
}
//--------------------------------------------------------------------------
VeRenderObject::Type VeRenderer::ParseToType(const VeChar8* pcStr,
	const VeRenderObject::Type& eType)
{
	VeRenderObject::Type* peIter = m_kRenderObjectTypeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::Blend VeRenderer::ParseToType(const VeChar8* pcStr,
	const Blend& eType)
{
	Blend* peIter = m_kBlendParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::BlendOp VeRenderer::ParseToType(const VeChar8* pcStr,
	const BlendOp& eType)
{
	BlendOp* peIter = m_kBlendOpParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::ColorWriteEnable VeRenderer::ParseToType(const VeChar8* pcStr,
	const ColorWriteEnable& eType)
{
	ColorWriteEnable* peIter = m_kColorWriteParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::ComparisonFunc VeRenderer::ParseToType(const VeChar8* pcStr,
	const ComparisonFunc& eType)
{
	ComparisonFunc* peIter = m_kComparisonFuncParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::StencilOp VeRenderer::ParseToType(const VeChar8* pcStr,
	const StencilOp& eType)
{
	StencilOp* peIter = m_kStencilOpParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::Semantic VeRenderer::ParseToType(const VeChar8* pcStr,
	const Semantic& eType)
{
	Semantic* peIter = m_kSemanticParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::Format VeRenderer::ParseToType(const VeChar8* pcStr,
	const Format& eType)
{
	Format* peIter = m_kFormatParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::InputClassification VeRenderer::ParseToType(const VeChar8* pcStr,
	const InputClassification& eType)
{
	InputClassification* peIter = m_kInputCalssParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::FillMode VeRenderer::ParseToType(const VeChar8* pcStr,
	const FillMode& eType)
{
	FillMode* peIter = m_kFillModeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::CullMode VeRenderer::ParseToType(const VeChar8* pcStr,
	const CullMode& eType)
{
	CullMode* peIter = m_kCullModeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::DepthBiasMode VeRenderer::ParseToType(const VeChar8* pcStr,
	const DepthBiasMode& eType)
{
	DepthBiasMode* peIter = m_kDepthBiasModeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::Filter VeRenderer::ParseToType(const VeChar8* pcStr,
	const Filter& eType)
{
	Filter* peIter = m_kFilterParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderer::TextureAddressMode VeRenderer::ParseToType(const VeChar8* pcStr,
	const TextureAddressMode& eType)
{
	TextureAddressMode* peIter = m_kTexAddressModeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeRenderTick::Type VeRenderer::ParseToType(const VeChar8* pcStr,
	const VeRenderTick::Type& eType)
{
	VeRenderTick::Type* peIter = m_kRenderTickTypeParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
VeIAPrimitiveTopology::Topology VeRenderer::ParseToType(const VeChar8* pcStr,
	const VeIAPrimitiveTopology::Topology& eType)
{
	VeIAPrimitiveTopology::Topology* peIter = m_kPrimitiveTopologyParser.Find(pcStr);
	return peIter ? *peIter : eType;
}
//--------------------------------------------------------------------------
