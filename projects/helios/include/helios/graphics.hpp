#pragma once

#include <helios/optional.hpp>
#include <helios/vector.hpp>
#include <helios/window.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace helios
{
	enum EAccessFlagBits : uint32_t
	{
		ACCESS_INDIRECT_COMMAND_READ_BIT = 0x00000001,
		ACCESS_INDEX_READ_BIT = 0x00000002,
		ACCESS_VERTEX_ATTRIBUTE_READ_BIT = 0x00000004,
		ACCESS_UNIFORM_READ_BIT = 0x00000008,
		ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00000010,
		ACCESS_SHADER_READ_BIT = 0x00000020,
		ACCESS_SHADER_WRITE_BIT = 0x00000040,
		ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,
		ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
		ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200,
		ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400,
		ACCESS_TRANSFER_READ_BIT = 0x00000800,
		ACCESS_TRANSFER_WRITE_BIT = 0x00001000,
		ACCESS_HOST_READ_BIT = 0x00002000,
		ACCESS_HOST_WRITE_BIT = 0x00004000,
		ACCESS_MEMORY_READ_BIT = 0x00008000,
		ACCESS_MEMORY_WRITE_BIT = 0x00010000,
		ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT = 0x02000000,
		ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT = 0x04000000,
		ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT = 0x08000000,
		ACCESS_CONDITIONAL_RENDERING_READ_BIT = 0x00100000,
		ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT = 0x00080000,
		ACCESS_ACCELERATION_STRUCTURE_READ_BIT = 0x00200000,
		ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT = 0x00400000,
		ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT = 0x01000000,
	};

	using EAccessFlags = uint32_t;

	enum class EAttachmentLoadOp : uint32_t
	{
		LOAD = 0,
		CLEAR = 1,
		DONT_CARE = 2
	};

	enum class EAttachmentStoreOp : uint32_t
	{
		STORE = 0,
		DONT_CARE = 1
	};

	enum class EBindPoint : uint32_t
	{
		GRAPHICS = 0,
		COMPUTE = 1,
		RAY_TRACING = 1000165000
	};

	enum class EBlendFactor : uint32_t
	{
		ZERO = 0,
		ONE = 1,
		SRC_COLOR = 2,
		ONE_MINUS_SRC_COLOR = 3,
		DST_COLOR = 4,
		ONE_MINUS_DST_COLOR = 5,
		SRC_ALPHA = 6,
		ONE_MINUS_SRC_ALPHA = 7,
		DST_ALPHA = 8,
		ONE_MINUS_DST_ALPHA = 9,
		CONSTANT_COLOR = 10,
		ONE_MINUS_CONSTANT_COLOR = 11,
		CONSTANT_ALPHA = 12,
		ONE_MINUS_CONSTANT_ALPHA = 13,
		SRC_ALPHA_SATURATE = 14,
		SRC1_COLOR = 15,
		ONE_MINUS_SRC1_COLOR = 16,
		SRC1_ALPHA = 17,
		ONE_MINUS_SRC1_ALPHA = 18
	};

	enum class EBlendOp : uint32_t
	{
		ADD = 0,
		SUBTRACT = 1,
		REVERSE_SUBTRACT = 2,
		MIN = 3,
		MAX = 4
	};

	enum EBufferTypeFlagBits : uint32_t
	{
		BUFFER_TYPE_TRANSFER_SRC = 0x001,
		BUFFER_TYPE_TRANSFER_DST = 0x002,
		BUFFER_TYPE_UNIFORM_TEXEL = 0x004,
		BUFFER_TYPE_STORAGE_TEXEL = 0x008,
		BUFFER_TYPE_UNIFORM = 0x010,
		BUFFER_TYPE_STORAGE = 0x020,
		BUFFER_TYPE_INDEX = 0x040,
		BUFFER_TYPE_VERTEX = 0x080,
		BUFFER_TYPE_INDIRECT = 0x100
	};

	using EBufferTypeFlags = uint32_t;

	enum EColorComponentFlagBits : uint32_t
	{
		COLOR_COMPONENT_R = 0x1,
		COLOR_COMPONENT_G = 0x2,
		COLOR_COMPONENT_B = 0x4,
		COLOR_COMPONENT_A = 0x8
	};

	using EColorComponentFlags = uint32_t;

	enum class EColorSpace : uint32_t
	{
		SRGB_NONLINEAR = 0,
		DISPLAY_P3_NONLINEAR = 1000104001,
		EXTENDED_SRGB_LINEAR = 1000104002,
		DISPLAY_P3_LINEAR = 1000104003,
		DCI_P3_NONLINEAR = 1000104004,
		BT709_LINEAR = 1000104005,
		BT709_NONLINEAR = 1000104006,
		BT2020_LINEAR = 1000104007,
		HDR10_ST2084 = 1000104008,
		DOLBYVISION = 1000104009,
		HDR10_HLG = 1000104010,
		ADOBERGB_LINEAR = 1000104011,
		ADOBERGB_NONLINEAR = 1000104012,
		PASS_THROUGH = 1000104013,
		EXTENDED_SRGB_NONLINEAR = 1000104014,
	};

	enum class ECommandBufferLevel : uint32_t
	{
		PRIMARY = 0,
		SECONDARY = 1
	};

	enum class ECompareOp : uint32_t
	{
		NEVER = 0,
		LESS = 1,
		EQUAL = 2,
		LESS_OR_EQUAL = 3,
		GREATER = 4,
		NOT_EQUAL = 5,
		GREATER_OR_EQUAL = 6,
		ALWAYS = 7
	};

	enum class EComponentSwizzle : uint32_t
	{
		IDENTITY = 0,
		ZERO = 1,
		ONE = 2,
		RED = 3,
		GREEN = 4,
		BLUE = 5,
		ALPHA = 6
	};

	enum ECullModeFlagBits : uint32_t
	{
		CULL_MODE_NONE = 0x0,
		CULL_MODE_FRONT = 0x1,
		CULL_MODE_BACK = 0x2,
		CULL_MODE_ALL = 0x3
	};

	using ECullModeFlags = uint32_t;

	enum EDependencyFlagBits : uint32_t
	{
		DEPENDENCY_BY_REGION = 0x01,
		DEPENDENCY_VIEW_LOCAL_BIT = 0x02,
		DEPENDENCY_DEVICE_GROUP_BIT = 0x04,
	};

	using EDependencyFlags = uint32_t;

	enum class EDescriptorType : uint32_t
	{
		SAMPLER = 0,
		COMBINED_IMAGE_SAMPLER = 1,
		SAMPLED_IMAGE = 2,
		STORAGE_IMAGE = 3,
		UNIFORM_TEXEL_BUFFER = 4,
		STORAGE_TEXEL_BUFFER = 5,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		UNIFORM_BUFFER_DYNAMIC = 8,
		STORAGE_BUFFER_DYNAMIC = 9,
		INPUT_ATTACHMENT = 10
	};

	enum class EDynamicState : uint32_t
	{
		VIEWPORT = 0,
		SCISSOR = 1,
		LINE_WIDTH = 2,
		DEPTH_BIAS = 3,
		BLEND_CONSTANTS = 4,
		DEPTH_BOUNDS = 5,
		STENCIL_COMPARE_MISK = 6,
		STENCIL_WRITE_MASK = 7,
		STENCIL_REFERENCE = 8
	};

	enum class EFormat
	{
		UNDEFINED = 0,
		R4G4_UNORM_PACK8 = 1,
		R4G4B4A4_UNORM_PACK16 = 2,
		B4G4R4A4_UNORM_PACK16 = 3,
		R5G6B5_UNORM_PACK16 = 4,
		B5G6R5_UNORM_PACK16 = 5,
		R5G5B5A1_UNORM_PACK16 = 6,
		B5G5R5A1_UNORM_PACK16 = 7,
		A1R5G5B5_UNORM_PACK16 = 8,
		R8_UNORM = 9,
		R8_SNORM = 10,
		R8_USCALED = 11,
		R8_SSCALED = 12,
		R8_UINT = 13,
		R8_SINT = 14,
		R8_SRGB = 15,
		R8G8_UNORM = 16,
		R8G8_SNORM = 17,
		R8G8_USCALED = 18,
		R8G8_SSCALED = 19,
		R8G8_UINT = 20,
		R8G8_SINT = 21,
		R8G8_SRGB = 22,
		R8G8B8_UNORM = 23,
		R8G8B8_SNORM = 24,
		R8G8B8_USCALED = 25,
		R8G8B8_SSCALED = 26,
		R8G8B8_UINT = 27,
		R8G8B8_SINT = 28,
		R8G8B8_SRGB = 29,
		B8G8R8_UNORM = 30,
		B8G8R8_SNORM = 31,
		B8G8R8_USCALED = 32,
		B8G8R8_SSCALED = 33,
		B8G8R8_UINT = 34,
		B8G8R8_SINT = 35,
		B8G8R8_SRGB = 36,
		R8G8B8A8_UNORM = 37,
		R8G8B8A8_SNORM = 38,
		R8G8B8A8_USCALED = 39,
		R8G8B8A8_SSCALED = 40,
		R8G8B8A8_UINT = 41,
		R8G8B8A8_SINT = 42,
		R8G8B8A8_SRGB = 43,
		B8G8R8A8_UNORM = 44,
		B8G8R8A8_SNORM = 45,
		B8G8R8A8_USCALED = 46,
		B8G8R8A8_SSCALED = 47,
		B8G8R8A8_UINT = 48,
		B8G8R8A8_SINT = 49,
		B8G8R8A8_SRGB = 50,
		A8B8G8R8_UNORM_PACK32 = 51,
		A8B8G8R8_SNORM_PACK32 = 52,
		A8B8G8R8_USCALED_PACK32 = 53,
		A8B8G8R8_SSCALED_PACK32 = 54,
		A8B8G8R8_UINT_PACK32 = 55,
		A8B8G8R8_SINT_PACK32 = 56,
		A8B8G8R8_SRGB_PACK32 = 57,
		A2R10G10B10_UNORM_PACK32 = 58,
		A2R10G10B10_SNORM_PACK32 = 59,
		A2R10G10B10_USCALED_PACK32 = 60,
		A2R10G10B10_SSCALED_PACK32 = 61,
		A2R10G10B10_UINT_PACK32 = 62,
		A2R10G10B10_SINT_PACK32 = 63,
		A2B10G10R10_UNORM_PACK32 = 64,
		A2B10G10R10_SNORM_PACK32 = 65,
		A2B10G10R10_USCALED_PACK32 = 66,
		A2B10G10R10_SSCALED_PACK32 = 67,
		A2B10G10R10_UINT_PACK32 = 68,
		A2B10G10R10_SINT_PACK32 = 69,
		R16_UNORM = 70,
		R16_SNORM = 71,
		R16_USCALED = 72,
		R16_SSCALED = 73,
		R16_UINT = 74,
		R16_SINT = 75,
		R16_SFLOAT = 76,
		R16G16_UNORM = 77,
		R16G16_SNORM = 78,
		R16G16_USCALED = 79,
		R16G16_SSCALED = 80,
		R16G16_UINT = 81,
		R16G16_SINT = 82,
		R16G16_SFLOAT = 83,
		R16G16B16_UNORM = 84,
		R16G16B16_SNORM = 85,
		R16G16B16_USCALED = 86,
		R16G16B16_SSCALED = 87,
		R16G16B16_UINT = 88,
		R16G16B16_SINT = 89,
		R16G16B16_SFLOAT = 90,
		R16G16B16A16_UNORM = 91,
		R16G16B16A16_SNORM = 92,
		R16G16B16A16_USCALED = 93,
		R16G16B16A16_SSCALED = 94,
		R16G16B16A16_UINT = 95,
		R16G16B16A16_SINT = 96,
		R16G16B16A16_SFLOAT = 97,
		R32_UINT = 98,
		R32_SINT = 99,
		R32_SFLOAT = 100,
		R32G32_UINT = 101,
		R32G32_SINT = 102,
		R32G32_SFLOAT = 103,
		R32G32B32_UINT = 104,
		R32G32B32_SINT = 105,
		R32G32B32_SFLOAT = 106,
		R32G32B32A32_UINT = 107,
		R32G32B32A32_SINT = 108,
		R32G32B32A32_SFLOAT = 109,
		R64_UINT = 110,
		R64_SINT = 111,
		R64_SFLOAT = 112,
		R64G64_UINT = 113,
		R64G64_SINT = 114,
		R64G64_SFLOAT = 115,
		R64G64B64_UINT = 116,
		R64G64B64_SINT = 117,
		R64G64B64_SFLOAT = 118,
		R64G64B64A64_UINT = 119,
		R64G64B64A64_SINT = 120,
		R64G64B64A64_SFLOAT = 121,
		B10G11R11_UFLOAT_PACK32 = 122,
		E5B9G9R9_UFLOAT_PACK32 = 123,
		D16_UNORM = 124,
		X8_D24_UNORM_PACK32 = 125,
		D32_SFLOAT = 126,
		S8_UINT = 127,
		D16_UNORM_S8_UINT = 128,
		D24_UNORM_S8_UINT = 129,
		D32_SFLOAT_S8_UINT = 130,
		BC1_RGB_UNORM_BLOCK = 131,
		BC1_RGB_SRGB_BLOCK = 132,
		BC1_RGBA_UNORM_BLOCK = 133,
		BC1_RGBA_SRGB_BLOCK = 134,
		BC2_UNORM_BLOCK = 135,
		BC2_SRGB_BLOCK = 136,
		BC3_UNORM_BLOCK = 137,
		BC3_SRGB_BLOCK = 138,
		BC4_UNORM_BLOCK = 139,
		BC4_SNORM_BLOCK = 140,
		BC5_UNORM_BLOCK = 141,
		BC5_SNORM_BLOCK = 142,
		BC6H_UFLOAT_BLOCK = 143,
		BC6H_SFLOAT_BLOCK = 144,
		BC7_UNORM_BLOCK = 145,
		BC7_SRGB_BLOCK = 146,
		ETC2_R8G8B8_UNORM_BLOCK = 147,
		ETC2_R8G8B8_SRGB_BLOCK = 148,
		ETC2_R8G8B8A1_UNORM_BLOCK = 149,
		ETC2_R8G8B8A1_SRGB_BLOCK = 150,
		ETC2_R8G8B8A8_UNORM_BLOCK = 151,
		ETC2_R8G8B8A8_SRGB_BLOCK = 152,
		EAC_R11_UNORM_BLOCK = 153,
		EAC_R11_SNORM_BLOCK = 154,
		EAC_R11G11_UNORM_BLOCK = 155,
		EAC_R11G11_SNORM_BLOCK = 156,
		ASTC_4x4_UNORM_BLOCK = 157,
		ASTC_4x4_SRGB_BLOCK = 158,
		ASTC_5x4_UNORM_BLOCK = 159,
		ASTC_5x4_SRGB_BLOCK = 160,
		ASTC_5x5_UNORM_BLOCK = 161,
		ASTC_5x5_SRGB_BLOCK = 162,
		ASTC_6x5_UNORM_BLOCK = 163,
		ASTC_6x5_SRGB_BLOCK = 164,
		ASTC_6x6_UNORM_BLOCK = 165,
		ASTC_6x6_SRGB_BLOCK = 166,
		ASTC_8x5_UNORM_BLOCK = 167,
		ASTC_8x5_SRGB_BLOCK = 168,
		ASTC_8x6_UNORM_BLOCK = 169,
		ASTC_8x6_SRGB_BLOCK = 170,
		ASTC_8x8_UNORM_BLOCK = 171,
		ASTC_8x8_SRGB_BLOCK = 172,
		ASTC_10x5_UNORM_BLOCK = 173,
		ASTC_10x5_SRGB_BLOCK = 174,
		ASTC_10x6_UNORM_BLOCK = 175,
		ASTC_10x6_SRGB_BLOCK = 176,
		ASTC_10x8_UNORM_BLOCK = 177,
		ASTC_10x8_SRGB_BLOCK = 178,
		ASTC_10x10_UNORM_BLOCK = 179,
		ASTC_10x10_SRGB_BLOCK = 180,
		ASTC_12x10_UNORM_BLOCK = 181,
		ASTC_12x10_SRGB_BLOCK = 182,
		ASTC_12x12_UNORM_BLOCK = 183,
		ASTC_12x12_SRGB_BLOCK = 184,
		G8B8G8R8_422_UNORM = 1000156000,
		B8G8R8G8_422_UNORM = 1000156001,
		G8_B8_R8_3PLANE_420_UNORM = 1000156002,
		G8_B8R8_2PLANE_420_UNORM = 1000156003,
		G8_B8_R8_3PLANE_422_UNORM = 1000156004,
		G8_B8R8_2PLANE_422_UNORM = 1000156005,
		G8_B8_R8_3PLANE_444_UNORM = 1000156006,
		R10X6_UNORM_PACK16 = 1000156007,
		R10X6G10X6_UNORM_2PACK16 = 1000156008,
		R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
		G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
		B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
		G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
		G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
		G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
		G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
		G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
		R12X4_UNORM_PACK16 = 1000156017,
		R12X4G12X4_UNORM_2PACK16 = 1000156018,
		R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
		G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
		B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
		G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
		G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
		G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
		G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
		G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
		G16B16G16R16_422_UNORM = 1000156027,
		B16G16R16G16_422_UNORM = 1000156028,
		G16_B16_R16_3PLANE_420_UNORM = 1000156029,
		G16_B16R16_2PLANE_420_UNORM = 1000156030,
		G16_B16_R16_3PLANE_422_UNORM = 1000156031,
		G16_B16R16_2PLANE_422_UNORM = 1000156032,
		G16_B16_R16_3PLANE_444_UNORM = 1000156033,
		PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
		PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
		PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
		PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
		PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
		PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
		PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
		PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
	};

	enum EImageAspectFlagBits : uint32_t
	{
		ASPECT_COLOR = 0x01,
		ASPECT_DEPTH = 0x02,
		ASPECT_STENCIL = 0x04,
		ASPECT_METADATA = 0x08,
		ASPECT_PLANE_0 = 0x10,
		ASPECT_PLANE_1 = 0x20,
		ASPECT_PLANE_2 = 0x40
	};

	using EImageAspectFlags = uint32_t;

	enum class EImageLayout : uint32_t
	{
		UNDEFINED = 0,
		GENERAL = 1,
		COLOR_ATTACHMENT_OPTIMAL = 2,
		DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
		DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
		SHADER_READ_ONLY_OPTIMAL = 5,
		TRANSFER_SRC_OPTIMAL = 6,
		TRANSFER_DST_OPTIMAL = 7,
		PREINITIALIZED = 8,
		DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
		DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
		DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
		DEPTH_READ_ONLY_OPTIMAL = 1000241001,
		STENCIL_ATTACHMENT_OPTIMAL = 1000241002,
		STENCIL_READ_ONLY_OPTIMAL = 1000241003,
		PRESENT_SRC = 1000001002,
	};

	enum EImageUsageFlagBits : uint32_t
	{
		IMAGE_TRANSFER_SRC = 0x001,
		IMAGE_TRANSFER_DST = 0x002,
		IMAGE_SAMPLED = 0x004,
		IMAGE_STORAGE = 0x008,
		IMAGE_COLOR_ATTACHMENT = 0x010,
		IMAGE_DEPTH_STENCIL_ATTACHMENT = 0x020,
		IMAGE_TRANSIENT_ATTACHMENT = 0x040,
		IMAGE_INPUT_ATTACHMENT = 0x080
	};

	using EImageUsageFlags = uint32_t;

	enum class EImageViewType : uint32_t
	{
		TYPE_1D = 0,
		TYPE_2D = 1,
		TYPE_3D = 2,
		TYPE_CUBE = 3,
		TYPE_1D_ARRAY = 4,
		TYPE_2D_ARRAY = 5,
		TYPE_CUBE_ARRAY = 6
	};

	enum class ELogicOp
	{
		CLEAR = 0,
		AND = 1,
		AND_REVERSE = 2,
		COPY = 3,
		AND_INVERTED = 4,
		NO_OP = 5,
		XOR = 6,
		OR = 7,
		NOR = 8,
		EQUIVALENT = 9,
		INVERT = 10,
		OR_REVERSE = 11,
		COPY_INVERTED = 12,
		OR_INVERTED = 13,
		NAND = 14,
		SET = 15
	};

	enum class EMessageSeverity
	{
		VERBOSE,
		INFO,
		WARNING,
		ERROR
	};

	enum class EPhysicalDeviceType
	{
		OTHER,
		INTEGRATED,
		DISCRETE,
		VIRTUAL,
		CPU
	};

	enum EPipelineStageFlagBits : uint32_t
	{
		PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
		PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00000002,
		PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00000004,
		PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00000008,
		PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
		PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
		PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00000040,
		PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080,
		PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
		PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00000200,
		PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
		PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00000800,
		PIPELINE_STAGE_TRANSFER_BIT = 0x00001000,
		PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00002000,
		PIPELINE_STAGE_HOST_BIT = 0x00004000,
		PIPELINE_STAGE_ALL_GRAPHICS_BIT = 0x00008000,
		PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000,
		PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT = 0x01000000,
		PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT = 0x00040000,
		PIPELINE_STAGE_RAY_TRACING_SHADER_BIT = 0x00200000,
		PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT = 0x02000000,
	};

	using EPipelineStageFlags = uint32_t;

	enum class EPolygonMode : uint32_t
	{
		FILL = 0,
		LINE = 1,
		POINT = 2
	};

	enum class EPresentMode : uint32_t
	{
		IMMEDIATE = 0,
		MAILBOX = 1,
		FIFO = 2,
		FIFO_RELAXED = 3
	};

	enum class EPrimitiveTopology
	{
		POINT_LIST = 0,
		LINE_LIST = 1,
		LINE_STRIP = 2,
		TRIANGLE_LIST = 3,
		TRIANGLE_STRIP = 4,
		TRIANGLE_FAN = 5,
		LINE_LIST_WITH_ADJ = 6,
		LINE_STRIP_WITH_ADJ = 7,
		TRIANGLE_LIST_WITH_ADJ = 8,
		TRIANGLE_STRIP_WITH_ADJ = 9,
		PATCH_LOST = 10
	};

	enum ESampleCountFlagBits : uint32_t
	{
		SAMPLE_COUNT_1 = 0x01,
		SAMPLE_COUNT_2 = 0x02,
		SAMPLE_COUNT_4 = 0x04,
		SAMPLE_COUNT_8 = 0x08,
		SAMPLE_COUNT_16 = 0x10,
		SAMPLE_COUNT_32 = 0x20,
		SAMPLE_COUNT_64 = 0x40
	};

	using ESampleCountFlags = uint32_t;

	enum EShaderStageFlagBits : uint32_t
	{
		SHADER_STAGE_VERTEX_BIT = 0x00000001,
		SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
		SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
		SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
		SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
		SHADER_STAGE_COMPUTE_BIT = 0x00000020,
		SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
		SHADER_STAGE_ALL = 0x7FFFFFFF
	};

	using EShaderStageFlags = uint32_t;

	enum class EStencilOp : uint32_t
	{
		KEEP = 0,
		ZERO = 1,
		REPLACE = 2,
		INCREMENT_AND_CLAMP = 3,
		DECREMENT_AND_CLAMP = 4,
		INVERT = 5,
		INCREMENT_AND_WRAP = 6,
		DECREMENT_AND_WRAP = 7
	};

	enum ESurfaceTransformFlagBits : uint32_t
	{
		TRANSFORM_IDENTITY = 0x001,
		TRANSFORM_ROTATE_90 = 0x002,
		TRANSFORM_ROTATE_180 = 0x004,
		TRANSFORM_ROTATE_270 = 0x008,
		TRANSFORM_HORIZONTAL_MIRROR = 0x010,
		TRANSFORM_HORIZONTAL_MIRROR_90 = 0x020,
		TRANSFORM_HORIZONTAL_MIRROR_180 = 0x040,
		TRANSFORM_HORIZONTAL_MIRROR_270 = 0x080,
		TRANSFORM_INHERIT = 0x100
	};

	using ESurfaceTransformFlags = uint32_t;

	enum class EVertexInputRate : uint32_t
	{
		VERTEX = 0,
		INSTANCE = 1
	};

	enum class EVertexWindingOrder : uint32_t
	{
		COUNTER_CLOCKWISE = 0,
		CLOCKWISE = 1
	};

	union ClearColorValue {
		float float32[4];
		int32_t int32[4];
		uint32_t uint32[4];
	};

	struct ClearDepthStencilValue
	{
		float depth;
		uint32_t stencil;
	};

	union ClearValue {
		ClearColorValue color;
		ClearDepthStencilValue depthStencil;
	};

	std::string stringify(const EPhysicalDeviceType &pType);

	class ICommandBuffer;
	class ICommandPool;
	class IContext;
	class IDescriptorSetLayout;
	class IDevice;
	class IFence;
	class IFramebuffer;
	class IGraphicsPipeline;
	class IImage;
	class IImageView;
	class IPhysicalDevice;
	class IPipelineLayout;
	class IQueue;
	class IRenderPass;
	class ISemaphore;
	class IShaderModule;
	class ISurface;
	class ISwapchain;

	struct QueueFamilyInfo
	{
		uint32_t family;
		bool protect = false;
		vector<float> priorities;
	};

	struct DescriptorSetLayoutBinding
	{
		uint32_t binding;
		EDescriptorType type;
		uint32_t count;
		EShaderStageFlags stages;
	};

	struct PushConstantRange
	{
		EShaderStageFlags stages;
		uint32_t offset;
		uint32_t size;
	};

	class ContextBuilder final
	{
	public:
		ContextBuilder();
		ContextBuilder(const ContextBuilder &) = delete;
		ContextBuilder(ContextBuilder &&) noexcept = delete;
		~ContextBuilder();
		ContextBuilder &operator=(const ContextBuilder &) = delete;
		ContextBuilder &operator=(ContextBuilder &&) noexcept = delete;

		ContextBuilder &applicationName(const std::string &name);
		ContextBuilder &applicationVersion(const uint32_t major, const uint32_t minor, const uint32_t revision);
		ContextBuilder &engineName(const std::string &name);
		ContextBuilder &engineVersion(const uint32_t major, const uint32_t minor, const uint32_t revision);
		ContextBuilder &validation(std::function<void(EMessageSeverity, std::string)> callback = DefaultCallback);
		IContext *build() const;

	private:
		struct ContextBuilderImpl;

		ContextBuilderImpl *_impl;
		static std::function<void(EMessageSeverity, const std::string)> DefaultCallback;
	};

	class IContext
	{
	protected:
		IContext() = default;

	public:
		virtual ~IContext() = default;

		[[nodiscard]] virtual vector<IPhysicalDevice *> physicalDevices() const = 0;

		IContext(const IContext &) = delete;
		IContext(IContext &&) noexcept = delete;
		IContext &operator=(const IContext &) = delete;
		IContext &operator=(IContext &&) noexcept = delete;
	};

	class IPhysicalDevice
	{
	protected:
		IPhysicalDevice() = default;

	public:
		struct Features
		{
			bool robustBufferAccess;
			bool fullDrawUint32;
			bool imageCubeArray;
			bool independentBlend;
			bool geometryShader;
			bool tessellationShader;
			bool sampleRateShading;
			bool dualSourceBlend;
			bool logicOperations;
			bool multidrawIndirect;
			bool drawIndirectFirstInstance;
			bool depthClamp;
			bool depthBiasClamp;
			bool fillModeNonSolid;
			bool depthBounds;
			bool wideLines;
			bool largePoints;
			bool alphaToOne;
			bool multiViewport;
			bool samplerAnisotropy;
			bool textureCompressionEtc2;
			bool textureCompressionAstcLdr;
			bool textureCompressionBc;
			bool occlusionQueryPrecise;
			bool pipelineStatisticsQuery;
			bool vertexPipelineStoresAndAtomics;
			bool fragmentStoresAndAtomics;
			bool shaderTessellationAndGeometryPointSize;
			bool shaderImageGatherExtended;
			bool shaderStorageImageExtendedFormats;
			bool shaderStorageImageMultisample;
			bool shaderStorageImageReadWithoutFormat;
			bool shaderStorageImageWriteWithoutFormat;
			bool shaderUniformBufferArrayDynamicIndexing;
			bool shaderSampledImageArrayDynamicIndexing;
			bool shaderStorageBufferArrayDynamicIndexing;
			bool shaderStorageImageArrayDynamicIndexing;
			bool shaderClipDistance;
			bool shaderCullDistance;
			bool shaderFloat64;
			bool shaderInt64;
			bool shaderInt16;
			bool shaderResourceResidency;
			bool shaderResourceMinLod;
			bool sparseBinding;
			bool sparseResidencyBuffer;
			bool sparseResidencyImage2D;
			bool sparseResidencyImage3D;
			bool sparseResidency2Samples;
			bool sparseResidency4Samples;
			bool sparseResidency8Samples;
			bool sparseResidency16Samples;
			bool sparseResidencyAliased;
			bool variableMultisampleRate;
			bool inheritedQueries;
		};

		struct QueueProperties
		{
			uint32_t index;
			bool graphics;
			bool compute;
			bool transfer;
			bool sparse;
			bool protect;
			uint32_t count;
		};

		virtual ~IPhysicalDevice() = default;
		[[nodiscard]] virtual std::string name() const = 0;
		[[nodiscard]] virtual EPhysicalDeviceType type() const = 0;
		[[nodiscard]] virtual Features features() const = 0;
		[[nodiscard]] virtual vector<QueueProperties> queueProperties() = 0;

		IPhysicalDevice(const IPhysicalDevice &) = delete;
		IPhysicalDevice(IPhysicalDevice &&) noexcept = delete;
		IPhysicalDevice &operator=(const IPhysicalDevice &) = delete;
		IPhysicalDevice &operator=(IPhysicalDevice &&) noexcept = delete;
	};

	class DeviceBuilder
	{
	public:
		DeviceBuilder();
		DeviceBuilder(const DeviceBuilder &) = delete;
		DeviceBuilder(DeviceBuilder &&) noexcept = delete;
		~DeviceBuilder();
		DeviceBuilder &operator=(const DeviceBuilder &) = delete;
		DeviceBuilder &operator=(DeviceBuilder &&) noexcept = delete;

		DeviceBuilder &queues(const vector<QueueFamilyInfo> &infos);
		DeviceBuilder &features(const IPhysicalDevice::Features &features);
		DeviceBuilder &validation();
		DeviceBuilder &swapchain();
		DeviceBuilder &physical(const IPhysicalDevice *pPhysical);
		DeviceBuilder &compute(const uint32_t count);
		DeviceBuilder &transfer(const uint32_t count);
		DeviceBuilder &graphics(const uint32_t count);
		[[nodiscard]] IDevice *build() const;

	private:
		struct DeviceBuilderImpl;

		DeviceBuilderImpl *_impl;
	};

	class IQueue
	{
	protected:
		IQueue() = default;

	public:
		struct SubmitInfo
		{
			vector<ISemaphore *> wait;
			vector<EPipelineStageFlags> waitMask;
			vector<ISemaphore *> signal;
			vector<ICommandBuffer *> buffers;
		};

		struct PresentInfo
		{
			vector<ISemaphore *> waits;
			ISwapchain *swapchain;
			uint32_t image;
		};

		IQueue(const IQueue &) = delete;
		IQueue(IQueue &&) noexcept = delete;
		virtual ~IQueue() = default;
		IQueue &operator=(const IQueue &) = delete;
		IQueue &operator=(IQueue &&) noexcept = delete;

		[[nodiscard]] virtual float priority() const = 0;
		[[nodiscard]] virtual uint32_t index() = 0;
		[[nodiscard]] virtual IPhysicalDevice::QueueProperties props() const = 0;
		[[nodiscard]] virtual bool canPresent(const IPhysicalDevice *device, const ISurface *surface) const = 0;
		virtual void submit(const vector<SubmitInfo> &submitInfo, const IFence *signal) const = 0;
		virtual void present(const PresentInfo &presentInfo) const = 0;
	};

	class IDevice
	{
	protected:
		IDevice() = default;

	public:
		virtual ~IDevice() = default;

		[[nodiscard]] virtual vector<IQueue *> queues() const = 0;
		virtual void idle() const = 0;

		IDevice(const IDevice &) = delete;
		IDevice(IDevice &&) noexcept = delete;
		IDevice &operator=(const IDevice &) = delete;
		IDevice &operator=(IDevice &&) noexcept = delete;
	};

	class SurfaceBuilder
	{
	public:
		SurfaceBuilder();
		~SurfaceBuilder();

		SurfaceBuilder &device(const IDevice *device);
		SurfaceBuilder &window(const IWindow *window);
		[[nodiscard]] ISurface *build() const;

		SurfaceBuilder(const SurfaceBuilder &) = delete;
		SurfaceBuilder(SurfaceBuilder &&) noexcept = delete;
		SurfaceBuilder &operator=(const SurfaceBuilder &) = delete;
		SurfaceBuilder &operator=(SurfaceBuilder &&) noexcept = delete;

	private:
		struct SurfaceBuilderImpl;
		SurfaceBuilderImpl *_impl;
	};

	class ISurface
	{
	protected:
		ISurface() = default;

	public:
		struct SurfaceFormat
		{
			EFormat format;
			EColorSpace colorSpace;
		};

		struct SwapchainSupport
		{
			uint32_t minImages;
			uint32_t maxImages;
			uint32_t currentWidth;
			uint32_t currentHeight;
			uint32_t minWidth;
			uint32_t minHeight;
			uint32_t maxWidth;
			uint32_t maxHeight;
			uint32_t maxImageArrayLayers;
			bool alphaOpaque;
			bool alphaPremultiplied;
			bool alphaPostmultiplied;
			bool alphaInherit;
			EImageUsageFlags supportedUsage;
			ESurfaceTransformFlagBits currentTransform;
			ESurfaceTransformFlags supportedTransforms;

			vector<SurfaceFormat> surfaceFormats;
			vector<EPresentMode> presentModes;
		};

		virtual ~ISurface() = default;
		[[nodiscard]] virtual IWindow *window() const = 0;
		[[nodiscard]] virtual SwapchainSupport swapchainSupport(const IPhysicalDevice *device) const = 0;

		ISurface(const ISurface &) = delete;
		ISurface(ISurface &&) noexcept = delete;
		ISurface &operator=(const ISurface &) = delete;
		ISurface &operator=(ISurface &&) noexcept = delete;
	};

	class SwapchainBuilder
	{
	public:
		SwapchainBuilder();
		~SwapchainBuilder();

		SwapchainBuilder &surface(const ISurface *surface);
		SwapchainBuilder &images(const uint32_t count);
		SwapchainBuilder &width(const uint32_t width);
		SwapchainBuilder &height(const uint32_t height);
		SwapchainBuilder &layers(const uint32_t layers);
		SwapchainBuilder &previous(const ISwapchain *previous);
		SwapchainBuilder &present(const EPresentMode present);
		SwapchainBuilder &format(const EFormat format);
		SwapchainBuilder &colorSpace(const EColorSpace colorSpace);
		SwapchainBuilder &queues(const vector<IQueue *> &queues);
		SwapchainBuilder &usage(const EImageUsageFlags usage);
		SwapchainBuilder &transform(const ESurfaceTransformFlagBits transform);
		SwapchainBuilder &alphaOpaque();
		SwapchainBuilder &alphaPremultiply();
		SwapchainBuilder &alphaPostmultiply();
		SwapchainBuilder &alphaInherit();
		SwapchainBuilder &clipped();
		[[nodiscard]] ISwapchain *build() const;

		SwapchainBuilder(const SwapchainBuilder &) = delete;
		SwapchainBuilder(SwapchainBuilder &&) noexcept = delete;
		SwapchainBuilder &operator=(const SwapchainBuilder &) = delete;
		SwapchainBuilder &operator=(SwapchainBuilder &&) noexcept = delete;

	private:
		struct SwapchainBuilderImpl;

		SwapchainBuilderImpl *_impl;
	};

	class ISwapchain
	{
	protected:
		ISwapchain() = default;

	public:
		virtual ~ISwapchain() = default;

		[[nodiscard]] virtual uint32_t imagesCount() const = 0;
		[[nodiscard]] virtual vector<IImageView *> views() const = 0;
		[[nodiscard]] virtual EFormat format() const = 0;
		[[nodiscard]] virtual uint32_t acquireNextImage(const uint64_t wait, const ISemaphore *signal, const IFence *fence) = 0;

		ISwapchain(const ISwapchain &) = delete;
		ISwapchain(ISwapchain &&) noexcept = delete;
		ISwapchain &operator=(const ISwapchain &) = delete;
		ISwapchain &operator=(ISwapchain &&) noexcept = delete;
	};

	class ImageBuilder
	{
	public:
		ImageBuilder();
		~ImageBuilder();

		ImageBuilder(const ImageBuilder &) = delete;
		ImageBuilder(ImageBuilder &&) noexcept = delete;
		ImageBuilder &operator=(const ImageBuilder &) = delete;
		ImageBuilder &operator=(ImageBuilder &&) noexcept = delete;

	private:
		struct ImageBuilderImpl;

		ImageBuilderImpl *_impl;
	};

	class IImage
	{
	protected:
		IImage() = default;

	public:
		virtual ~IImage() = default;

		IImage(const IImage &) = delete;
		IImage(IImage &&) noexcept = delete;
		IImage &operator=(const IImage &) = delete;
		IImage &operator=(IImage &&) noexcept = delete;
	};

	class ImageViewBuilder
	{
	public:
		ImageViewBuilder();
		~ImageViewBuilder();

		ImageViewBuilder &type(const EImageViewType type);
		ImageViewBuilder &image(const IImage *image);
		ImageViewBuilder &format(const EFormat format);
		ImageViewBuilder &redMapping(const EComponentSwizzle red);
		ImageViewBuilder &greenMapping(const EComponentSwizzle green);
		ImageViewBuilder &blueMapping(const EComponentSwizzle blue);
		ImageViewBuilder &alphaMapping(const EComponentSwizzle alpha);
		ImageViewBuilder &aspect(const EImageAspectFlags aspect);
		ImageViewBuilder &baseMipLevel(const uint32_t base);
		ImageViewBuilder &mipLevels(const uint32_t count);
		ImageViewBuilder &baseArrayLayer(const uint32_t base);
		ImageViewBuilder &arrayLayers(const uint32_t count);
		[[nodiscard]] IImageView *build() const;

		ImageViewBuilder(const ImageViewBuilder &) = delete;
		ImageViewBuilder(ImageViewBuilder &&) noexcept = delete;
		ImageViewBuilder &operator=(const ImageViewBuilder &) = delete;
		ImageViewBuilder &operator=(ImageViewBuilder &&) noexcept = delete;

	private:
		struct ImageViewBuilderImpl;

		ImageViewBuilderImpl *_impl;
	};

	class IImageView
	{
	protected:
		IImageView() = default;

	public:
		virtual ~IImageView() = default;

		IImageView(const IImageView &) = delete;
		IImageView(IImageView &&) noexcept = delete;
		IImageView &operator=(const IImageView &) = delete;
		IImageView &operator=(IImageView &&) noexcept = delete;
	};

	class ShaderModuleBuilder
	{
	public:
		ShaderModuleBuilder();
		~ShaderModuleBuilder();

		ShaderModuleBuilder &device(const IDevice *device);
		ShaderModuleBuilder &source(const vector<uint8_t> &source);
		[[nodiscard]] IShaderModule *build() const;

		ShaderModuleBuilder(const ShaderModuleBuilder &) = delete;
		ShaderModuleBuilder(ShaderModuleBuilder &&) noexcept = delete;
		ShaderModuleBuilder &operator=(const ShaderModuleBuilder &) = delete;
		ShaderModuleBuilder &operator=(ShaderModuleBuilder &&) noexcept = delete;

	private:
		struct ShaderModuleBuilderImpl;

		ShaderModuleBuilderImpl *_impl;
	};

	class IShaderModule
	{
	protected:
		IShaderModule() = default;

	public:
		virtual ~IShaderModule() = default;

		IShaderModule(const IShaderModule &) = delete;
		IShaderModule(IShaderModule &&) noexcept = delete;
		IShaderModule &operator=(const IShaderModule &) = delete;
		IShaderModule &operator=(IShaderModule &&) noexcept = delete;
	};

	class DescriptorSetLayoutBuilder
	{
	public:
		DescriptorSetLayoutBuilder();
		~DescriptorSetLayoutBuilder();

		DescriptorSetLayoutBuilder &device(const IDevice *device);
		DescriptorSetLayoutBuilder &bindings(const vector<DescriptorSetLayoutBinding> &bindings);
		[[nodiscard]] IDescriptorSetLayout *build() const;

		DescriptorSetLayoutBuilder(const DescriptorSetLayoutBuilder &) = delete;
		DescriptorSetLayoutBuilder(DescriptorSetLayoutBuilder &&) noexcept = delete;
		DescriptorSetLayoutBuilder &operator=(const DescriptorSetLayoutBuilder &) = delete;
		DescriptorSetLayoutBuilder &operator=(DescriptorSetLayoutBuilder &&) noexcept = delete;

	private:
		struct DescriptorSetLayoutBuilderImpl;

		DescriptorSetLayoutBuilderImpl *_impl;
	};

	class IDescriptorSetLayout
	{
	protected:
		IDescriptorSetLayout() = default;

	public:
		virtual ~IDescriptorSetLayout() = default;

		[[nodiscard]] virtual vector<DescriptorSetLayoutBinding> bindings() const = 0;

		IDescriptorSetLayout(const IDescriptorSetLayout &) = delete;
		IDescriptorSetLayout(IDescriptorSetLayout &&) noexcept = delete;
		IDescriptorSetLayout &operator=(const IDescriptorSetLayout &) = delete;
		IDescriptorSetLayout &operator=(IDescriptorSetLayout &&) noexcept = delete;
	};

	class PipelineLayoutBuilder
	{
	public:
		PipelineLayoutBuilder();
		~PipelineLayoutBuilder();

		PipelineLayoutBuilder &device(const IDevice *device);
		PipelineLayoutBuilder &layouts(const vector<IDescriptorSetLayout *> &layouts);
		PipelineLayoutBuilder &pushConstants(const vector<PushConstantRange> &ranges);
		[[nodiscard]] IPipelineLayout *build() const;

		PipelineLayoutBuilder(const PipelineLayoutBuilder &) = delete;
		PipelineLayoutBuilder(PipelineLayoutBuilder &&) noexcept = delete;
		PipelineLayoutBuilder &operator=(const PipelineLayoutBuilder &) = delete;
		PipelineLayoutBuilder &operator=(PipelineLayoutBuilder &&) noexcept = delete;

	private:
		struct PipelineLayoutBuilderImpl;

		PipelineLayoutBuilderImpl *_impl;
	};

	class IPipelineLayout
	{
	protected:
		IPipelineLayout() = default;

	public:
		virtual ~IPipelineLayout() = default;

		IPipelineLayout(const IPipelineLayout &) = delete;
		IPipelineLayout(IPipelineLayout &&) noexcept = delete;
		IPipelineLayout &operator=(const IPipelineLayout &) = delete;
		IPipelineLayout &operator=(IPipelineLayout &&) noexcept = delete;
	};

	class GraphicsPipelineBuilder
	{
	public:
		struct VertexInputAttributeBinding
		{
			uint32_t binding;
			uint32_t stride;
			EVertexInputRate inputRate;
			uint32_t location;
			EFormat format;
			uint32_t offset;
		};

		struct VertexInputState
		{
			vector<VertexInputAttributeBinding> bindings;
		};

		struct InputAssemblyState
		{
			EPrimitiveTopology topology;
			bool primitiveRestartEnable;
		};

		struct TessellationState
		{
			uint32_t patchControlPoints;
		};

		struct Viewport
		{
			float x;
			float y;
			float width;
			float height;
			float minDepth;
			float maxDepth;
		};

		struct Rect2D
		{
			int32_t x;
			int32_t y;
			uint32_t width;
			uint32_t height;
		};

		struct ViewportState
		{
			vector<Viewport> viewports;
			vector<Rect2D> scissorRegions;
		};

		struct RasterizationState
		{
			bool depthClampEnable = false;
			bool rasterizerDiscardEnable = true;
			EPolygonMode mode = EPolygonMode::FILL;
			ECullModeFlags cullMode = CULL_MODE_BACK;
			EVertexWindingOrder frontFace = EVertexWindingOrder::COUNTER_CLOCKWISE;
			bool depthBiasEnable = false;
			float depthBiasConstantFactor = 0.0f;
			float depthBiasClamp = 0.0f;
			float depthBiasSlopeFactor = 0.0f;
			float lineWidth = 1.0f;
		};

		struct MultisampleState
		{
			ESampleCountFlagBits samples = SAMPLE_COUNT_1;
			bool sampleShadingEnable = false;
			float minSampleShading = 0.0f;
			optional<uint32_t> sampleMask;
			bool alphaToCoverage = false;
			bool alphaToOne = false;
		};

		struct StencilOpState
		{
			EStencilOp failOp;
			EStencilOp passOp;
			EStencilOp depthFailOp;
			ECompareOp compareOp;
			uint32_t compareMask;
			uint32_t writeMask;
			uint32_t reference;
		};

		struct DepthStencilState
		{
			bool depthTestEnable = false;
			bool depthWriteEnable = false;
			ECompareOp depthCompareOp = ECompareOp::LESS;
			bool depthBoundsTestEnable = false;
			bool stencilTestEnable = false;
			StencilOpState front = {};
			StencilOpState back = {};
			float minDepthBounds = 0.0f;
			float maxDepthBounds = 1.0f;
		};

		struct ColorBlendAttachmentState
		{
			bool blendEnable = false;
			EBlendFactor srcColorBlendFactor = EBlendFactor::SRC_COLOR;
			EBlendFactor dstColorBlendFactor = EBlendFactor::ONE_MINUS_CONSTANT_COLOR;
			EBlendOp colorBlendOp = EBlendOp::ADD;
			EBlendFactor srcAlphaBlendFactor = EBlendFactor::SRC_ALPHA;
			EBlendFactor dstAlphaBlendFactor = EBlendFactor::ONE_MINUS_SRC_ALPHA;
			EBlendOp alphaBlendOp = EBlendOp::ADD;
			EColorComponentFlags colorMask = COLOR_COMPONENT_R | COLOR_COMPONENT_G | COLOR_COMPONENT_B | COLOR_COMPONENT_A;
		};

		struct ColorBlendState
		{
			bool logicEnable = false;
			ELogicOp logicOp = ELogicOp::CLEAR;
			vector<ColorBlendAttachmentState> attachments;
			float blendConstants[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		};

		struct DynamicState
		{
			vector<EDynamicState> states;
		};

		GraphicsPipelineBuilder();
		~GraphicsPipelineBuilder();

		GraphicsPipelineBuilder &allowDerivatives();
		GraphicsPipelineBuilder &deriveFrom(const IGraphicsPipeline *parent);
		GraphicsPipelineBuilder &vertex(const IShaderModule *vertex);
		GraphicsPipelineBuilder &fragment(const IShaderModule *fragment);
		GraphicsPipelineBuilder &input(const VertexInputState &bindings);
		GraphicsPipelineBuilder &assembly(const InputAssemblyState &assembly);
		GraphicsPipelineBuilder &tessellation(const TessellationState &tessellation);
		GraphicsPipelineBuilder &viewports(const ViewportState &viewport);
		GraphicsPipelineBuilder &rasterization(const RasterizationState &raster);
		GraphicsPipelineBuilder &multisample(const MultisampleState &multi);
		GraphicsPipelineBuilder &depthStencil(const DepthStencilState &depth);
		GraphicsPipelineBuilder &colorBlend(const ColorBlendState &color);
		GraphicsPipelineBuilder &dynamic(const DynamicState &dynamic);
		GraphicsPipelineBuilder &layout(const IPipelineLayout *layout);
		GraphicsPipelineBuilder &subpass(const uint32_t subpass);
		GraphicsPipelineBuilder &renderpass(const IRenderPass *renderpass);
		[[nodiscard]] IGraphicsPipeline *build() const;

		GraphicsPipelineBuilder(const GraphicsPipelineBuilder &) = delete;
		GraphicsPipelineBuilder(GraphicsPipelineBuilder &&) noexcept = delete;
		GraphicsPipelineBuilder &operator=(const GraphicsPipelineBuilder &) = delete;
		GraphicsPipelineBuilder &operator=(GraphicsPipelineBuilder &&) noexcept = delete;

	private:
		struct GraphicsPipelineBuilderImpl;

		GraphicsPipelineBuilderImpl *_impl;
	};

	class IGraphicsPipeline
	{
	protected:
		IGraphicsPipeline() = default;

	public:
		virtual ~IGraphicsPipeline() = default;

		IGraphicsPipeline(const IGraphicsPipeline &) = delete;
		IGraphicsPipeline(IGraphicsPipeline &&) noexcept = delete;
		IGraphicsPipeline &operator=(const IGraphicsPipeline &) = delete;
		IGraphicsPipeline &operator=(IGraphicsPipeline &&) noexcept = delete;
	};

	class RenderPassBuilder
	{
	public:
		struct AttachmentDescription
		{
			EFormat format;
			ESampleCountFlagBits samples;
			EAttachmentLoadOp loadOp;
			EAttachmentStoreOp storeOp;
			EAttachmentLoadOp stencilLoadOp;
			EAttachmentStoreOp stencilStoreOp;
			EImageLayout initialLayout;
			EImageLayout finalLayout;
		};

		struct AttachmentReference
		{
			uint32_t attachment;
			EImageLayout layout;
		};

		struct SubpassDescription
		{
			EBindPoint bind;
			vector<AttachmentReference> inputAttachments;
			vector<AttachmentReference> colorAttachments;
			vector<AttachmentReference> resolveAttachments;
			optional<AttachmentReference> depthAttachment;
			vector<uint32_t> preserveAttachments;
		};

		struct SubpassDependency
		{
			uint32_t srcSubpass;
			uint32_t dstSubpass;
			EPipelineStageFlags srcStageMask;
			EPipelineStageFlags dstStageMask;
			EAccessFlags srcAccessMask;
			EAccessFlags dstAccessMask;
			EDependencyFlags dependencyFlags;
		};

		RenderPassBuilder();
		~RenderPassBuilder();

		RenderPassBuilder &device(const IDevice *device);
		RenderPassBuilder &attachments(const vector<AttachmentDescription> &attachments);
		RenderPassBuilder &subpasses(const vector<SubpassDescription> &subpasses);
		RenderPassBuilder &dependencies(const vector<SubpassDependency> &dependencies);
		[[nodiscard]] IRenderPass *build() const;

		RenderPassBuilder(const RenderPassBuilder &) = delete;
		RenderPassBuilder(RenderPassBuilder &&) noexcept = delete;
		RenderPassBuilder &operator=(const RenderPassBuilder &) = delete;
		RenderPassBuilder &operator=(RenderPassBuilder &&) noexcept = delete;

	private:
		struct RenderPassBuilderImpl;

		RenderPassBuilderImpl *_impl;
	};

	class IRenderPass
	{
	protected:
		IRenderPass() = default;

	public:
		virtual ~IRenderPass() = default;

		IRenderPass(const IRenderPass &) = delete;
		IRenderPass(IRenderPass &&) noexcept = delete;
		IRenderPass &operator=(const IRenderPass &) = delete;
		IRenderPass &operator=(IRenderPass &&) noexcept = delete;
	};

	class FramebufferBuilder
	{
	public:
		FramebufferBuilder();
		~FramebufferBuilder();

		FramebufferBuilder &renderpass(const IRenderPass *renderpass);
		FramebufferBuilder &attachments(const vector<IImageView *> &attachments);
		FramebufferBuilder &width(const uint32_t width);
		FramebufferBuilder &height(const uint32_t height);
		FramebufferBuilder &layers(const uint32_t layers);
		[[nodiscard]] IFramebuffer *build() const;

		FramebufferBuilder(const FramebufferBuilder &) = delete;
		FramebufferBuilder(FramebufferBuilder &&) noexcept = delete;
		FramebufferBuilder &operator=(const FramebufferBuilder &) = delete;
		FramebufferBuilder &operator=(FramebufferBuilder &&) noexcept = delete;

	private:
		struct FramebufferBuilderImpl;

		FramebufferBuilderImpl *_impl;
	};

	class IFramebuffer
	{
	protected:
		IFramebuffer() = default;

	public:
		virtual ~IFramebuffer() = default;

		[[nodiscard]] virtual uint32_t width() const = 0;
		[[nodiscard]] virtual uint32_t height() const = 0;
		[[nodiscard]] virtual uint32_t layers() const = 0;
		[[nodiscard]] virtual vector<IImageView *> attachments() const = 0;

		IFramebuffer(const IFramebuffer &) = delete;
		IFramebuffer(IFramebuffer &&) noexcept = delete;
		IFramebuffer &operator=(const IFramebuffer &) = delete;
		IFramebuffer &operator=(IFramebuffer &&) noexcept = delete;
	};

	class CommandPoolBuilder
	{
	public:
		CommandPoolBuilder();
		~CommandPoolBuilder();

		CommandPoolBuilder &device(const IDevice *device);
		CommandPoolBuilder &queue(const IQueue *queue);
		CommandPoolBuilder &transient();
		CommandPoolBuilder &reset();
		[[nodiscard]] ICommandPool *build() const;

		CommandPoolBuilder(const CommandPoolBuilder &) = delete;
		CommandPoolBuilder(CommandPoolBuilder &&) noexcept = delete;
		CommandPoolBuilder &operator=(const CommandPoolBuilder &) = delete;
		CommandPoolBuilder &operator=(CommandPoolBuilder &&) noexcept = delete;

	private:
		struct CommandPoolBuilderImpl;

		CommandPoolBuilderImpl *_impl;
	};

	class ICommandPool
	{
	protected:
		ICommandPool() = default;

	public:
		virtual ~ICommandPool() = default;

		[[nodiscard]] virtual ICommandBuffer *allocate(const ECommandBufferLevel level = ECommandBufferLevel::PRIMARY) = 0;
		[[nodiscard]] virtual vector<ICommandBuffer *> allocate(const uint32_t count, const ECommandBufferLevel level = ECommandBufferLevel::PRIMARY) = 0;

		ICommandPool(const ICommandPool &) = delete;
		ICommandPool(ICommandPool &&) noexcept = delete;
		ICommandPool &operator=(const ICommandPool &) = delete;
		ICommandPool &operator=(ICommandPool &&) noexcept = delete;
	};

	class ICommandBuffer
	{
	protected:
		ICommandBuffer() = default;

	public:
		struct RenderPassRecordInfo
		{
			IRenderPass *renderpass;
			IFramebuffer *renderTarget;
			int32_t x;
			int32_t y;
			uint32_t width;
			uint32_t height;
			vector<ClearValue> clearValues;
		};

		virtual ~ICommandBuffer() = default;

		virtual void record() const = 0;
		virtual void end() const = 0;
		virtual void beginRenderPass(const RenderPassRecordInfo &info, const bool isInline) = 0;
		virtual void endRenderPass() = 0;
		virtual void bind(const IGraphicsPipeline *pipeline) = 0;
		virtual void draw(const uint32_t vertices, const uint32_t instances, const uint32_t baseVertex, const uint32_t baseInstance) = 0;

		ICommandBuffer(const ICommandBuffer &) = delete;
		ICommandBuffer(ICommandBuffer &&) noexcept = delete;
		ICommandBuffer &operator=(const ICommandBuffer &) = delete;
		ICommandBuffer &operator=(ICommandBuffer &&) noexcept = delete;
	};

	class SemaphoreBuilder
	{
	public:
		SemaphoreBuilder();
		~SemaphoreBuilder();

		SemaphoreBuilder &device(const IDevice *device);
		[[nodiscard]] ISemaphore *build() const;

		SemaphoreBuilder(const SemaphoreBuilder &) = delete;
		SemaphoreBuilder(SemaphoreBuilder &&) noexcept = delete;
		SemaphoreBuilder &operator=(const SemaphoreBuilder &) = delete;
		SemaphoreBuilder &operator=(SemaphoreBuilder &&) noexcept = delete;

	private:
		struct SemaphoreBuilderImpl;
		SemaphoreBuilderImpl *_impl;
	};

	class ISemaphore
	{
	protected:
		ISemaphore() = default;

	public:
		virtual ~ISemaphore() = default;

		ISemaphore(const ISemaphore &) = delete;
		ISemaphore(ISemaphore &&) noexcept = delete;
		ISemaphore &operator=(const ISemaphore &) = delete;
		ISemaphore &operator=(ISemaphore &&) noexcept = delete;
	};

	class FenceBuilder
	{
	public:
		FenceBuilder();
		~FenceBuilder();

		FenceBuilder &device(const IDevice *device);
		FenceBuilder &signaled();
		[[nodiscard]] IFence *build() const;

		FenceBuilder(const FenceBuilder &) = delete;
		FenceBuilder(FenceBuilder &&) noexcept = delete;
		FenceBuilder &operator=(const FenceBuilder &) = delete;
		FenceBuilder &operator=(FenceBuilder &&) noexcept = delete;

	private:
		struct FenceBuilderImpl;
		FenceBuilderImpl *_impl;
	};

	class IFence
	{
	protected:
		IFence() = default;

	public:
		virtual ~IFence() = default;

		virtual void reset() = 0;
		virtual void wait() = 0;
		virtual bool status() = 0;

		IFence(const IFence &) = delete;
		IFence(IFence &&) noexcept = delete;
		IFence &operator=(const IFence &) = delete;
		IFence &operator=(IFence &&) noexcept = delete;
	};
} // namespace helios