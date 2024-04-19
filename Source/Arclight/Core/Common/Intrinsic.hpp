/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Intrinsic.hpp
 */

#pragma once

#include "Build.hpp"
#include "Config.hpp"
#include "Types.hpp"
#include "Assert.hpp"
#include "Util/Preprocessor.hpp"

#include <utility>



#if defined(ARC_PLATFORM_X86) && defined(ARC_CFG_INTRINSIC_X86)
	#define ARC_INTRINSIC_H <immintrin.h>
	#define ARC_INTRINSIC_AVAILABLE
#elif defined(ARC_PLATFORM_ARM) && defined(ARC_CFG_INTRINSIC_ARM)
	#define ARC_INTRINSIC_H <arm_neon.h>
	#define ARC_INTRINSIC_AVAILABLE
#else
	#define ARC_INTRINSIC_H "Common/Intrinsic.hpp" // Nothing will get included due to #pragma once
#endif


namespace CPU::X86 {

	enum class Register {
		EAX,
		EBX,
		ECX,
		EDX
	};


	struct Feature {

		constexpr Feature(Register reg, u32 bit, u32 subLeaf, bool extended) noexcept : packed(0) {

			arc_assert(bit >= 0 && bit <= 31, "Bit out of range");
			arc_assert(subLeaf <= 0x7F, "Subleaf out of range");

			arc_assert(!extended || (subLeaf != 0) || (reg != Register::EAX), "At leaf 0x7 subleaf 0x0 EAX does not contain features");
			arc_assert(extended || (reg != Register::EAX) || (reg != Register::EBX), "At leaf 0x1 EAX and EBX do not contain features");

			packed |= std::to_underlying(reg) & 0x7;
			packed |= (bit & 0x1F) << 5;
			packed |= (subLeaf & 0x7F) << 8;
			packed |= extended << 15;

		}

		constexpr Feature(Register reg, u32 bit, u32 subLeaf) noexcept : Feature(reg, bit, subLeaf, true) {}

		constexpr Feature(Register reg, u32 bit) noexcept : Feature(reg, bit, 0, false) {}


		constexpr bool extended() const noexcept {
			return packed & (1 << 15);
		}

		constexpr Register reg() const noexcept {
			return Register{packed & 0x7};
		}

		constexpr u32 bit() const noexcept {
			return (packed >> 3) & 0x1F;
		}

		constexpr u32 subLeaf() const noexcept {
			return (packed >> 8) & 0x7F;
		}


		u16 packed;

	};


	#include "FeaturesX86.inl"

}


namespace CPU::ARM {

	enum class Feature {};


	#include "FeaturesARM.inl"

}


namespace CPU {

	enum class Manufacturer {
		Unknown,
		AMD,
		Centaur,
		Intel,
		Transmeta,
		Cyrix,
		NSC,
		NexGen,
		Rise,
		SiS,
		UMC,
		VIA,
		Vortex,
		Zhaoxin,
		AO486,
		Hygon,
		RDC,
		Elbrus,
		BHYVE,
		KVM,
		QEMU,
		MicrosoftHV,
		MicrosoftXTA,
		Parallels,
		VMware,
		VirtualBox,
		Xen,
		ACRN,
		QNX,
		Rosetta
	};

	static constexpr u32 UnknownValue = 0xFFFFFFFF;


	extern void init() noexcept;


	extern bool supports(X86::Feature feature) noexcept;
	extern bool supports(ARM::Feature feature) noexcept;


	extern std::string manufacturerID() noexcept;
	extern Manufacturer manufacturer() noexcept;

	extern u32 stepping() noexcept;
	extern u32 model() noexcept;
	extern u32 family() noexcept;


	inline bool hasSSE() noexcept {
		return supports(X86::SSE);
	}

	inline bool hasSSE2() noexcept {
		return supports(X86::SSE2);
	}

	inline bool hasSSE3() noexcept {
		return supports(X86::SSE3);
	}

	inline bool hasSSSE3() noexcept {
		return supports(X86::SSSE3);
	}

	inline bool hasSSE41() noexcept {
		return supports(X86::SSE41);
	}

	inline bool hasSSE42() noexcept {
		return supports(X86::SSE42);
	}

	inline bool hasAVX() noexcept {
		return supports(X86::AVX);
	}

	inline bool hasAVX2() noexcept {
		return supports(X86::AVX2);
	}


	inline bool hasNEON() noexcept {
		return supports(ARM::NEON);
	}

}


#ifdef ARC_INTRINSIC_AVAILABLE
	#define ARC_INTRINSIC_ENABLED(arch, id)	ARC_PP_IS_NULL(ARC_CFG_INTRINSIC_ENABLE_##arch##_##id)
	#define ARC_INTRINSIC_STATIC(arch, id)	ARC_PP_IS_NULL(ARC_CFG_INTRINSIC_STATIC_##arch##_##id)
#else
	#define ARC_INTRINSIC_ENABLED(arch, id)	0
	#define ARC_INTRINSIC_STATIC(arch, id)	0
#endif

#define __ARC_INTRINSIC_CONDITION(arch, id) \
ARC_PP_IF_ELSE(ARC_INTRINSIC_ENABLED(arch, id), \
	ARC_PP_IF_ELSE(ARC_INTRINSIC_STATIC(arch, id), if (!std::is_constant_evaluated()), if (!std::is_constant_evaluated() && CPU::supports(CPU::arch::id))), \
	if constexpr (false) \
)

#define __ARC_INTRINSIC_EXPAND(...) { __VA_ARGS__ }
#define __ARC_INTRINSIC_IGNORE(...) {}

#define __ARC_INTRINSIC_BODY(arch, id) ARC_PP_IF_ELSE(ARC_INTRINSIC_ENABLED(arch, id), __ARC_INTRINSIC_EXPAND, __ARC_INTRINSIC_IGNORE)


#define arc_intrinsic(arch, id) __ARC_INTRINSIC_CONDITION(arch, id) __ARC_INTRINSIC_BODY(arch, id)

#define arc_intrinsic_sse(...)		arc_intrinsic(X86, SSE)		(__VA_ARGS__)
#define arc_intrinsic_sse2(...)	    arc_intrinsic(X86, SSE2)	(__VA_ARGS__)
#define arc_intrinsic_sse3(...)	    arc_intrinsic(X86, SSE3)	(__VA_ARGS__)
#define arc_intrinsic_ssse3(...)	arc_intrinsic(X86, SSSE3)	(__VA_ARGS__)
#define arc_intrinsic_sse41(...)	arc_intrinsic(X86, SSE41)	(__VA_ARGS__)
#define arc_intrinsic_sse42(...)	arc_intrinsic(X86, SSE42)	(__VA_ARGS__)
#define arc_intrinsic_avx(...)		arc_intrinsic(X86, AVX)		(__VA_ARGS__)
#define arc_intrinsic_avx2(...)		arc_intrinsic(X86, AVX2)	(__VA_ARGS__)
