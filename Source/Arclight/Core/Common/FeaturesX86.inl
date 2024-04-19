/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 FeaturesX86.inl
 */



static constexpr Feature SSE3			= {Register::ECX, 0};
static constexpr Feature PCLMULQDQ		= {Register::ECX, 1};
static constexpr Feature DTES64			= {Register::ECX, 2};
static constexpr Feature MONITOR		= {Register::ECX, 3};
static constexpr Feature DSCPL			= {Register::ECX, 4};
static constexpr Feature VMX			= {Register::ECX, 5};
static constexpr Feature SMX			= {Register::ECX, 6};
static constexpr Feature EST			= {Register::ECX, 7};
static constexpr Feature TM2			= {Register::ECX, 8};
static constexpr Feature SSSE3			= {Register::ECX, 9};
static constexpr Feature CNXTID			= {Register::ECX, 10};
static constexpr Feature SDBG			= {Register::ECX, 11};
static constexpr Feature FMA			= {Register::ECX, 12};
static constexpr Feature CMPXCHG16B		= {Register::ECX, 13};
static constexpr Feature XTPR			= {Register::ECX, 14};
static constexpr Feature PDCM			= {Register::ECX, 15};
static constexpr Feature PCID			= {Register::ECX, 17};
static constexpr Feature DCA			= {Register::ECX, 18};
static constexpr Feature SSE41			= {Register::ECX, 19};
static constexpr Feature SSE42			= {Register::ECX, 20};
static constexpr Feature X2APIC			= {Register::ECX, 21};
static constexpr Feature MOVBE			= {Register::ECX, 22};
static constexpr Feature POPCNT			= {Register::ECX, 23};
static constexpr Feature TSCDEADLINE	= {Register::ECX, 24};
static constexpr Feature AESNI			= {Register::ECX, 25};
static constexpr Feature XSAVE			= {Register::ECX, 26};
static constexpr Feature OSXSAVE		= {Register::ECX, 27};
static constexpr Feature AVX			= {Register::ECX, 28};
static constexpr Feature F16C			= {Register::ECX, 29};
static constexpr Feature RDRND			= {Register::ECX, 30};
static constexpr Feature HYPERVISOR		= {Register::ECX, 31};

static constexpr Feature FPU	= {Register::EDX, 0};
static constexpr Feature VME	= {Register::EDX, 1};
static constexpr Feature DE		= {Register::EDX, 2};
static constexpr Feature PSE	= {Register::EDX, 3};
static constexpr Feature TSC	= {Register::EDX, 4};
static constexpr Feature MSR	= {Register::EDX, 5};
static constexpr Feature PAE	= {Register::EDX, 6};
static constexpr Feature MCE	= {Register::EDX, 7};
static constexpr Feature CX8	= {Register::EDX, 8};
static constexpr Feature APIC	= {Register::EDX, 9};
static constexpr Feature SEP	= {Register::EDX, 11};
static constexpr Feature MTRR	= {Register::EDX, 12};
static constexpr Feature PGE	= {Register::EDX, 13};
static constexpr Feature MCA	= {Register::EDX, 14};
static constexpr Feature CMOV	= {Register::EDX, 15};
static constexpr Feature PAT	= {Register::EDX, 16};
static constexpr Feature PSE36	= {Register::EDX, 17};
static constexpr Feature PSN	= {Register::EDX, 18};
static constexpr Feature CLFSH	= {Register::EDX, 19};
static constexpr Feature NX		= {Register::EDX, 20};
static constexpr Feature DS		= {Register::EDX, 21};
static constexpr Feature ACPI	= {Register::EDX, 22};
static constexpr Feature MMX	= {Register::EDX, 23};
static constexpr Feature FXSR	= {Register::EDX, 24};
static constexpr Feature SSE	= {Register::EDX, 25};
static constexpr Feature SSE2	= {Register::EDX, 26};
static constexpr Feature SS		= {Register::EDX, 27};
static constexpr Feature HTT	= {Register::EDX, 28};
static constexpr Feature TM		= {Register::EDX, 29};
static constexpr Feature IA64	= {Register::EDX, 30};
static constexpr Feature PBE	= {Register::EDX, 31};


static constexpr Feature FSGSBASE				= {Register::EBX, 0, 0};
static constexpr Feature MSR_IA32_TSC_ADJUST	= {Register::EBX, 1, 0};
static constexpr Feature SGX					= {Register::EBX, 2, 0};
static constexpr Feature BMI1					= {Register::EBX, 3, 0};
static constexpr Feature HLE					= {Register::EBX, 4, 0};
static constexpr Feature AVX2					= {Register::EBX, 5, 0};
static constexpr Feature FDP_EXCPTN_ONLY		= {Register::EBX, 6, 0};
static constexpr Feature SMEP					= {Register::EBX, 7, 0};
static constexpr Feature BMI2					= {Register::EBX, 8, 0};
static constexpr Feature ERMS					= {Register::EBX, 9, 0};
static constexpr Feature INVPCID				= {Register::EBX, 10, 0};
static constexpr Feature RTM					= {Register::EBX, 11, 0};
static constexpr Feature RDT_M					= {Register::EBX, 12, 0};
static constexpr Feature DEPRECATE_FPU_CS_DS	= {Register::EBX, 13, 0};
static constexpr Feature MPX					= {Register::EBX, 14, 0};
static constexpr Feature RDT_A					= {Register::EBX, 15, 0};
static constexpr Feature AVX512F				= {Register::EBX, 16, 0};
static constexpr Feature AVX512DQ				= {Register::EBX, 17, 0};
static constexpr Feature RDSEED					= {Register::EBX, 18, 0};
static constexpr Feature ADX					= {Register::EBX, 19, 0};
static constexpr Feature SMAP					= {Register::EBX, 20, 0};
static constexpr Feature AVX512_IFMA			= {Register::EBX, 21, 0};
static constexpr Feature RDPID_AMD				= {Register::EBX, 22, 0};
static constexpr Feature CLFLUSHOPT				= {Register::EBX, 23, 0};
static constexpr Feature CLWB					= {Register::EBX, 24, 0};
static constexpr Feature PT						= {Register::EBX, 25, 0};
static constexpr Feature AVX512PF				= {Register::EBX, 26, 0};
static constexpr Feature AVX512ER				= {Register::EBX, 27, 0};
static constexpr Feature AVX512CD				= {Register::EBX, 28, 0};
static constexpr Feature SHA					= {Register::EBX, 29, 0};
static constexpr Feature AVX512BW				= {Register::EBX, 30, 0};
static constexpr Feature AVX512VL				= {Register::EBX, 31, 0};

static constexpr Feature PQM = RDT_M;
static constexpr Feature PQE = RDT_A;

static constexpr Feature PREFETCHWT1		= {Register::ECX, 0, 0};
static constexpr Feature AVX512_VBMI		= {Register::ECX, 1, 0};
static constexpr Feature UMIP				= {Register::ECX, 2, 0};
static constexpr Feature PKU				= {Register::ECX, 3, 0};
static constexpr Feature OSPKE				= {Register::ECX, 4, 0};
static constexpr Feature WAITPKG			= {Register::ECX, 5, 0};
static constexpr Feature AVX512_VBMI2		= {Register::ECX, 6, 0};
static constexpr Feature CET_SS				= {Register::ECX, 7, 0};
static constexpr Feature GFNI				= {Register::ECX, 8, 0};
static constexpr Feature VAES				= {Register::ECX, 9, 0};
static constexpr Feature VPCLMULQDQ			= {Register::ECX, 10, 0};
static constexpr Feature AVX512_VNNI		= {Register::ECX, 11, 0};
static constexpr Feature AVX512_BITALG		= {Register::ECX, 12, 0};
static constexpr Feature TME_EN				= {Register::ECX, 13, 0};
static constexpr Feature AVX512_VPOPCNTDQ	= {Register::ECX, 14, 0};
static constexpr Feature LA57				= {Register::ECX, 16, 0};
static constexpr Feature RDPID_INTEL		= {Register::ECX, 22, 0};
static constexpr Feature KL					= {Register::ECX, 23, 0};
static constexpr Feature BUS_LOCK_DETECT	= {Register::ECX, 24, 0};
static constexpr Feature CLDEMOTE			= {Register::ECX, 25, 0};
static constexpr Feature MOVDIRI			= {Register::ECX, 27, 0};
static constexpr Feature MOVDIR64B			= {Register::ECX, 28, 0};
static constexpr Feature ENQCMD				= {Register::ECX, 29, 0};
static constexpr Feature SGX_LC				= {Register::ECX, 30, 0};
static constexpr Feature PKS				= {Register::ECX, 31, 0};

static constexpr Feature SHSTK = CET_SS;
static constexpr Feature BUSLOCKTRAP = BUS_LOCK_DETECT;

static constexpr Feature SGX_KEYS					= {Register::EDX, 1, 0};
static constexpr Feature AVX512_4VNNIW				= {Register::EDX, 2, 0};
static constexpr Feature AVX512_4FMAPS				= {Register::EDX, 3, 0};
static constexpr Feature FSRM						= {Register::EDX, 4, 0};
static constexpr Feature UINTR						= {Register::EDX, 5, 0};
static constexpr Feature AVX512_VP2INTERSECT		= {Register::EDX, 8, 0};
static constexpr Feature SRBDS_CTRL					= {Register::EDX, 9, 0};
static constexpr Feature MD_CLEAR					= {Register::EDX, 10, 0};
static constexpr Feature RTM_ALWAYS_ABORT			= {Register::EDX, 11, 0};
static constexpr Feature RTM_FORCE_ABORT			= {Register::EDX, 13, 0};
static constexpr Feature SERIALIZE					= {Register::EDX, 14, 0};
static constexpr Feature HYBRID						= {Register::EDX, 15, 0};
static constexpr Feature TSXLDTRK					= {Register::EDX, 16, 0};
static constexpr Feature PCONFIG					= {Register::EDX, 18, 0};
static constexpr Feature LBR						= {Register::EDX, 19, 0};
static constexpr Feature CET_IBT					= {Register::EDX, 20, 0};
static constexpr Feature AMX_BF16					= {Register::EDX, 22, 0};
static constexpr Feature AVX512_FP16				= {Register::EDX, 23, 0};
static constexpr Feature AMX_TILE					= {Register::EDX, 24, 0};
static constexpr Feature AMX_INT8					= {Register::EDX, 25, 0};
static constexpr Feature IBRS						= {Register::EDX, 26, 0};
static constexpr Feature STIBP						= {Register::EDX, 27, 0};
static constexpr Feature L1D_FLUSH					= {Register::EDX, 28, 0};
static constexpr Feature MSR_IA32_ARCH_CAPABILITIES	= {Register::EDX, 29, 0};
static constexpr Feature MSR_IA32_CORE_CAPABILITIES	= {Register::EDX, 30, 0};
static constexpr Feature SSBD						= {Register::EDX, 31, 0};

static constexpr Feature SHA512							= {Register::EAX, 0, 1};
static constexpr Feature SM3							= {Register::EAX, 1, 1};
static constexpr Feature SM4							= {Register::EAX, 2, 1};
static constexpr Feature RAO_INT						= {Register::EAX, 3, 1};
static constexpr Feature AVX_VNNI						= {Register::EAX, 4, 1};
static constexpr Feature AVX512_BF16					= {Register::EAX, 5, 1};
static constexpr Feature LASS							= {Register::EAX, 6, 1};
static constexpr Feature CMPCCXADD						= {Register::EAX, 7, 1};
static constexpr Feature ARCHPERFMONEXT					= {Register::EAX, 8, 1};
static constexpr Feature FZRM							= {Register::EAX, 10, 1};
static constexpr Feature FSRS							= {Register::EAX, 11, 1};
static constexpr Feature RSRCS							= {Register::EAX, 12, 1};
static constexpr Feature FRED							= {Register::EAX, 17, 1};
static constexpr Feature LKGS							= {Register::EAX, 18, 1};
static constexpr Feature WRMSRNS						= {Register::EAX, 19, 1};
static constexpr Feature NMI_SRC						= {Register::EAX, 20, 1};
static constexpr Feature AMX_FP16						= {Register::EAX, 21, 1};
static constexpr Feature HRESET							= {Register::EAX, 22, 1};
static constexpr Feature AVX_IFMA						= {Register::EAX, 23, 1};
static constexpr Feature LAM							= {Register::EAX, 26, 1};
static constexpr Feature MSRLIST						= {Register::EAX, 27, 1};
static constexpr Feature INVD_DISABLE_POST_BIOS_DONE	= {Register::EAX, 30, 1};

static constexpr Feature MSR_IA32_PPIN	= {Register::EBX, 0, 1};
static constexpr Feature PBNDKB			= {Register::EBX, 1, 1};

static constexpr Feature AVX_VNNI_INT8			= {Register::EDX, 4, 1};
static constexpr Feature AVX_NE_CONVERT			= {Register::EDX, 5, 1};
static constexpr Feature AMX_COMPLEX			= {Register::EDX, 8, 1};
static constexpr Feature AVX_VNNI_INT16			= {Register::EDX, 10, 1};
static constexpr Feature PREFETCHI				= {Register::EDX, 14, 1};
static constexpr Feature USER_MSR				= {Register::EDX, 15, 1};
static constexpr Feature UIRET_UIF_FROM_RFLAGS	= {Register::EDX, 17, 1};
static constexpr Feature CET_SSS				= {Register::EDX, 18, 1};
static constexpr Feature AVX10					= {Register::EDX, 19, 1};
static constexpr Feature APX_F					= {Register::EDX, 21, 1};

static constexpr Feature PSFD			= {Register::EDX, 0, 2};
static constexpr Feature IPRED_CTRL		= {Register::EDX, 1, 2};
static constexpr Feature RRSBA_CTRL		= {Register::EDX, 2, 2};
static constexpr Feature DDPD_U			= {Register::EDX, 3, 2};
static constexpr Feature BHI_CTRL		= {Register::EDX, 4, 2};
static constexpr Feature MCDT_NO		= {Register::EDX, 5, 2};
static constexpr Feature UCLOCKDISABLE	= {Register::EDX, 6, 2};
