/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Intrinsic.cpp
 */

#include "Intrinsic.hpp"

#include "Util/Bits.hpp"

#include <string>
#include <vector>
#include <array>
#include <unordered_map>



#ifdef ARC_PLATFORM_X86

	#ifdef ARC_COMPILER_MSVC
		#include <intrin.h>
	#else
		#include <cpuid.h>
	#endif

#elif defined(ARC_PLATFORM_ARM)

	#ifdef ARC_OS_WINDOWS
		#include "Common/Win32.hpp"
	#endif

#endif


namespace CPU {

	struct PlatformInfo {

	#ifdef ARC_PLATFORM_X86

		constexpr PlatformInfo() : basic{} {}

		std::vector<u32> extended;
		std::array<u32, 2> basic;

	#else

		constexpr PlatformInfo() : features(0) {}

		u32 features;

	#endif

	};

	struct Info {

		constexpr Info() noexcept :
			manufacturerID("Unknown"), manufacturer(Manufacturer::Unknown),
			stepping(UnknownValue), model(UnknownValue), family(UnknownValue), initialized(false) {}

		PlatformInfo platform;

		std::string manufacturerID;
		Manufacturer manufacturer;

		u32 stepping;
		u32 model;
		u32 family;

		bool initialized;

	};

	static Info info;


#ifdef ARC_PLATFORM_X86

	static const std::unordered_map<std::string, Manufacturer> manufacturers = {

		{"AMDisbetter!",	Manufacturer::AMD},
		{"AuthenticAMD",	Manufacturer::AMD},
		{"CentaurHauls",	Manufacturer::Centaur},
		{"GenuineIntel",	Manufacturer::Intel},
		{"GenuineIotel",	Manufacturer::Intel},
		{"TransmetaCPU",	Manufacturer::Transmeta},
		{"GenuineTMx86",	Manufacturer::Transmeta},
		{"CyrixInstead",	Manufacturer::Cyrix},
		{"Geode by NSC",	Manufacturer::NSC},
		{"NexGenDriven",	Manufacturer::NexGen},
		{"RiseRiseRise",	Manufacturer::Rise},
		{"SiS SiS SiS ",	Manufacturer::SiS},
		{"UMC UMC UMC ",	Manufacturer::UMC},
		{"VIA VIA VIA ",	Manufacturer::VIA},
		{"Vortex86 SoC",	Manufacturer::Vortex},
		{"  Shanghai  ",	Manufacturer::Zhaoxin},
		{"MiSTer AO486",	Manufacturer::AO486},
		{"GenuineAO486",	Manufacturer::AO486},
		{"HygonGenuine",	Manufacturer::Hygon},
		{"Genuine  RDC",	Manufacturer::RDC},
		{"E2K MACHINE",		Manufacturer::Elbrus},

		{"bhyve bhyve ",	Manufacturer::BHYVE},
		{"KVMKVMKVM",		Manufacturer::KVM},
		{"TCGTCGTCGTCG",	Manufacturer::QEMU},
		{"Microsoft Hv",	Manufacturer::MicrosoftHV},
		{"MicrosoftXTA",	Manufacturer::MicrosoftXTA},
		{" lrpepyh  vr",	Manufacturer::Parallels},
		{" prl hyperv ",	Manufacturer::Parallels},
		{"VMwareVMware",	Manufacturer::VMware},
		{"VBoxVBoxVBox",	Manufacturer::VirtualBox},
		{"XenVMMXenVMM",	Manufacturer::Xen},
		{"ACRNACRNACRN",	Manufacturer::ACRN},
		{" QNXQVMBSQG ",	Manufacturer::QNX},
		{"VirtualApple",	Manufacturer::Rosetta}

	};

#endif


	void init() noexcept {

		arc_assert(!info.initialized, "CPU Information already initialized");

		info.initialized = true;


	#ifdef ARC_PLATFORM_X86

		u32 data[4];


		__cpuidex(reinterpret_cast<i32*>(data), 0, 0); // Leaves count and manufacturer

		const u32 leaves = data[0];

		info.manufacturerID.resize(12);

		Bits::disassemble(data[1], &info.manufacturerID[0]);
		Bits::disassemble(data[3], &info.manufacturerID[4]);
		Bits::disassemble(data[2], &info.manufacturerID[8]);

		if (manufacturers.contains(info.manufacturerID)) {
			info.manufacturer = manufacturers.at(info.manufacturerID);
		} else {
			info.manufacturer = Manufacturer::Unknown;
		}


		if (leaves < 1) {
			return;
		}

		__cpuidex(reinterpret_cast<i32*>(data), 1, 0); // Features and model info

		info.stepping	= Bits::mask(data[0], 0, 4);
		info.family		= Bits::mask(data[0], 8, 4);

		if (info.family == 0xF) { // Extended family ID
			info.family += Bits::mask(data[0], 20, 7);
		}

		info.model = Bits::mask(data[0], 4, 4);

		if (info.family == 0x6 || info.family == 0xF) { // Extended model ID
			info.model += Bits::mask(data[0], 16, 4) << 4;
		}

		info.platform.basic = {data[2], data[3]};


		if (leaves < 7) {
			return;
		}

		__cpuidex(reinterpret_cast<i32*>(data), 7, 0); // Extended features

		for (SizeT i = 1; i < 4; i++) {
			info.platform.extended.emplace_back(data[i]);
		}

		for (u32 sub = 1; sub < data[0]; sub++) {

			__cpuidex(reinterpret_cast<i32*>(data), 7, sub);

			for (u32 bits : data) {
				info.platform.extended.emplace_back(bits);
			}

		}

	#elif defined(ARC_PLATFORM_ARM) && defined(ARC_OS_WINDOWS)

		constexpr u32 Features[] = {
			PF_ARM_64BIT_LOADSTORE_ATOMIC,
			PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE,
			PF_ARM_EXTERNAL_CACHE_AVAILABLE,
			PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE,
			PF_ARM_VFP_32_REGISTERS_AVAILABLE,
			PF_ARM_V8_INSTRUCTIONS_AVAILABLE,
			PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE,
			PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE,
			PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE,
			PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE,
			PF_ARM_V83_JSCVT_INSTRUCTIONS_AVAILABLE
		};

		for (SizeT i = 0; i < std::size(Features); i++) {
			info.arch.features |= !!IsProcessorFeaturePresent(Features[i]) << i;
		}

	#endif

	}


	bool supports(X86::Feature feature) noexcept {

		arc_assert(info.initialized, "CPU information is not initialized");

	#ifdef ARC_PLATFORM_X86

		const u32 reg = std::to_underlying(feature.reg());

		u32 value;

		if (feature.extended()) {
			value = info.platform.extended[feature.subLeaf() + reg - 1];
		} else {
			value = info.platform.basic[reg - 2];
		}

		return value & (1 << feature.bit());

	#else

		return false;

	#endif

	}

	bool supports(ARM::Feature feature) noexcept {

		arc_assert(info.initialized, "CPU information is not initialized");

	#ifdef ARC_PLATFORM_ARM

		return info.arch.features & (1 << std::to_underlying(feature));

	#else

		return false;

	#endif

	}


	std::string manufacturerID() noexcept {
		arc_assert(info.initialized, "CPU information is not initialized");
		return info.manufacturerID;
	}

	Manufacturer manufacturer() noexcept {
		arc_assert(info.initialized, "CPU information is not initialized");
		return info.manufacturer;
	}

	u32 stepping() noexcept {
		arc_assert(info.initialized, "CPU information is not initialized");
		return info.stepping;
	}

	u32 model() noexcept {
		arc_assert(info.initialized, "CPU information is not initialized");
		return info.model;
	}

	u32 family() noexcept {
		arc_assert(info.initialized, "CPU information is not initialized");
		return info.family;
	}

}
