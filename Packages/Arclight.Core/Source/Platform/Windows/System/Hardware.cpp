/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Hardware.hpp
 */

#include "System/Hardware.hpp"
#include "System/Common.hpp"
#include "System/SharedLibrary.hpp"
#include "Util/Log.hpp"
#include "Common/Win32.hpp"



namespace System::Hardware {

	std::string getComputerName() {

		DWORD size = 0;
		GetComputerNameExW(ComputerNamePhysicalDnsHostname, nullptr, &size);

		if (System::getSystemError() == ERROR_MORE_DATA) {

			std::wstring s(size, L'\0');

			if (GetComputerNameExW(ComputerNamePhysicalDnsHostname, s.data(), &size)) {
				return System::String::toUTF8(s);
			}

		}

		LogE("System") << "Failed to query computer name";
		return "?";

	}


	bool setComputerName(const std::string& name) {
		return SetComputerNameExW(ComputerNamePhysicalDnsHostname, System::String::toUTF16(name).c_str());
	}


	Version getOSVersion() {

		RTL_OSVERSIONINFOW info;
		System::loadLibrary("ntdll").getSymbol<NTSTATUS(*)(RTL_OSVERSIONINFOW*)>("RtlGetVersion")(&info);

		return Version(info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);

	}

	SizeT getHardwareConcurrency() {

		SizeT threads = 0;
		DWORD length = 0;

		if (GetLogicalProcessorInformationEx(RelationAll, nullptr, &length); System::getSystemError() == ERROR_INSUFFICIENT_BUFFER) {

		    std::vector<u8> buffer(length);

		    if (GetLogicalProcessorInformationEx(RelationAll, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &length)){

				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info;

				for (SizeT i = 0; i < length; i += info->Size) {

					info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data() + i);

					if (info->Relationship == RelationGroup) {

						const GROUP_RELATIONSHIP& group = info->Group;

						for (SizeT j = 0; j < group.ActiveGroupCount; j++){
							threads = Bits::popcount(group.GroupInfo[j].ActiveProcessorMask);
						}

					}

				}

				return threads ? threads : 1;

		    }

		}

		// If we encountered an error, fall back to SYSTEM_INFO
		SYSTEM_INFO info;
		ZeroMemory(&info, sizeof(info));

		GetNativeSystemInfo(&info);

		return info.dwNumberOfProcessors ? info.dwNumberOfProcessors : 1;

	}

}