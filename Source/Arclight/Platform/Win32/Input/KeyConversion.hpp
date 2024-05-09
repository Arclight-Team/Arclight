/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 KeyConversion.hpp
 */

#pragma once

#include "KeyDefs.hpp"

#include "Util/Range.hpp"

#include <array>



/*
 *	Key code transformation tables
 */
namespace KeyConversion {

	namespace Detail {

		template<bool ToPhysical>
		constexpr static auto GeneratePhysicalKeyTable = []() {

			std::array<u32, 512> keys {};
			std::array<u16, 256> scancodes {};

			auto bind = [&](u8 keycode, u16 scancode) {
				keys[scancode] = keycode;
				scancodes[keycode] = scancode;
			};

			for (u32 i : Range(keys.size())) {
				keys[i] = i << 16;
			}

			bind(PhysicalKey::A, 0x1E);
			bind(PhysicalKey::B, 0x30);
			bind(PhysicalKey::C, 0x2E);
			bind(PhysicalKey::D, 0x20);
			bind(PhysicalKey::E, 0x12);
			bind(PhysicalKey::F, 0x21);
			bind(PhysicalKey::G, 0x22);
			bind(PhysicalKey::H, 0x23);
			bind(PhysicalKey::I, 0x17);
			bind(PhysicalKey::J, 0x24);
			bind(PhysicalKey::K, 0x25);
			bind(PhysicalKey::L, 0x26);
			bind(PhysicalKey::M, 0x32);
			bind(PhysicalKey::N, 0x31);
			bind(PhysicalKey::O, 0x18);
			bind(PhysicalKey::P, 0x19);
			bind(PhysicalKey::Q, 0x10);
			bind(PhysicalKey::R, 0x13);
			bind(PhysicalKey::S, 0x1F);
			bind(PhysicalKey::T, 0x14);
			bind(PhysicalKey::U, 0x16);
			bind(PhysicalKey::V, 0x2F);
			bind(PhysicalKey::W, 0x11);
			bind(PhysicalKey::X, 0x2D);
			bind(PhysicalKey::Y, 0x15);
			bind(PhysicalKey::Z, 0x2C);
			bind(PhysicalKey::One, 0x02);
			bind(PhysicalKey::Two, 0x03);
			bind(PhysicalKey::Three, 0x04);
			bind(PhysicalKey::Four, 0x05);
			bind(PhysicalKey::Five, 0x06);
			bind(PhysicalKey::Six, 0x07);
			bind(PhysicalKey::Seven, 0x08);
			bind(PhysicalKey::Eight, 0x09);
			bind(PhysicalKey::Nine, 0x0A);
			bind(PhysicalKey::Zero, 0x0B);
			bind(PhysicalKey::Return, 0x1C);
			bind(PhysicalKey::Escape, 0x01);
			bind(PhysicalKey::Backspace, 0x0E);
			bind(PhysicalKey::Tab, 0x0F);
			bind(PhysicalKey::Space, 0x39);
			bind(PhysicalKey::Minus, 0x0C);
			bind(PhysicalKey::Equal, 0x0D);
			bind(PhysicalKey::LeftSquareBracket, 0x1A);
			bind(PhysicalKey::RightSquareBracket, 0x1B);
			bind(PhysicalKey::Backslash, 0x2B);
			bind(PhysicalKey::AngularBracket, 0x56);
			bind(PhysicalKey::Semicolon, 0x27);
			bind(PhysicalKey::Apostrophe, 0x28);
			bind(PhysicalKey::GraveAccent, 0x29);
			bind(PhysicalKey::Comma, 0x33);
			bind(PhysicalKey::Period, 0x34);
			bind(PhysicalKey::Slash, 0x35);
			bind(PhysicalKey::CapsLock, 0x3A);
			bind(PhysicalKey::F1, 0x3B);
			bind(PhysicalKey::F2, 0x3C);
			bind(PhysicalKey::F3, 0x3D);
			bind(PhysicalKey::F4, 0x3E);
			bind(PhysicalKey::F5, 0x3F);
			bind(PhysicalKey::F6, 0x40);
			bind(PhysicalKey::F7, 0x41);
			bind(PhysicalKey::F8, 0x42);
			bind(PhysicalKey::F9, 0x43);
			bind(PhysicalKey::F10, 0x44);
			bind(PhysicalKey::F11, 0x57);
			bind(PhysicalKey::F12, 0x58);
			bind(PhysicalKey::PrintScreen, 0x137);
			bind(PhysicalKey::ScrollLock, 0x46);
			bind(PhysicalKey::Pause, 0x45);
			bind(PhysicalKey::Insert, 0x152);
			bind(PhysicalKey::Home, 0x147);
			bind(PhysicalKey::PageUp, 0x149);
			bind(PhysicalKey::Delete, 0x153);
			bind(PhysicalKey::End, 0x14F);
			bind(PhysicalKey::PageDown, 0x151);
			bind(PhysicalKey::Right, 0x14D);
			bind(PhysicalKey::Left, 0x14B);
			bind(PhysicalKey::Down, 0x150);
			bind(PhysicalKey::Up, 0x148);
			bind(PhysicalKey::NumLock, 0x145);
			bind(PhysicalKey::NumpadDivide, 0x135);
			bind(PhysicalKey::NumpadMultiply, 0x37);
			bind(PhysicalKey::NumpadSubtract, 0x4A);
			bind(PhysicalKey::NumpadAdd, 0x4E);
			bind(PhysicalKey::NumpadEnter, 0x11C);
			bind(PhysicalKey::NumpadOne, 0x4F);
			bind(PhysicalKey::NumpadTwo, 0x50);
			bind(PhysicalKey::NumpadThree, 0x51);
			bind(PhysicalKey::NumpadFour, 0x4B);
			bind(PhysicalKey::NumpadFive, 0x4C);
			bind(PhysicalKey::NumpadSix, 0x4D);
			bind(PhysicalKey::NumpadSeven, 0x47);
			bind(PhysicalKey::NumpadEight, 0x48);
			bind(PhysicalKey::NumpadNine, 0x49);
			bind(PhysicalKey::NumpadZero, 0x52);
			bind(PhysicalKey::NumpadDecimal, 0x53);
			bind(PhysicalKey::OEM1, 0x2B);
			bind(PhysicalKey::App, 0x15D);
			bind(PhysicalKey::Power, 0x15E);
			bind(PhysicalKey::NumpadEqual, 0x59);
			bind(PhysicalKey::F13, 0x64);
			bind(PhysicalKey::F14, 0x65);
			bind(PhysicalKey::F15, 0x66);
			bind(PhysicalKey::F16, 0x67);
			bind(PhysicalKey::F17, 0x68);
			bind(PhysicalKey::F18, 0x69);
			bind(PhysicalKey::F19, 0x6A);
			bind(PhysicalKey::F20, 0x6B);
			bind(PhysicalKey::F21, 0x6C);
			bind(PhysicalKey::F22, 0x6D);
			bind(PhysicalKey::F23, 0x6E);
			bind(PhysicalKey::F24, 0x76);
			bind(PhysicalKey::LeftControl, 0x1D);
			bind(PhysicalKey::LeftShift, 0x2A);
			bind(PhysicalKey::LeftAlt, 0x38);
			bind(PhysicalKey::LeftSuper, 0x15B);
			bind(PhysicalKey::RightControl, 0x11D);
			bind(PhysicalKey::RightShift, 0x36);
			bind(PhysicalKey::RightAlt, 0x138);
			bind(PhysicalKey::RightSuper, 0x15C);

			if constexpr (ToPhysical) {
				return keys;
			} else {
				return scancodes;
			}

		};

		template<bool ToVirtual>
		constexpr static auto GenerateVirtualKeyTable = []() {

			std::array<u16, 256> virtualKeys {};
			std::array<u8, 256> winVKs {};

			auto bind = [&](u8 virtualKey, u8 winVK) {
				virtualKeys[winVK] = virtualKey;
				winVKs[virtualKey] = winVK;
			};

			for (u32 i : Range(virtualKeys.size())) {
				virtualKeys[i] = i << 8;
			}

			bind(VirtualKey::Back, 0x08);
			bind(VirtualKey::Tab, 0x09);
			bind(VirtualKey::Return, 0x0D);
			bind(VirtualKey::Escape, 0x1B);
			bind(VirtualKey::Space, 0x20);
			bind(VirtualKey::Left, 0x25);
			bind(VirtualKey::Right, 0x27);
			bind(VirtualKey::Up, 0x26);
			bind(VirtualKey::Down, 0x28);
			bind(VirtualKey::PrintScreen, 0x2C);
			bind(VirtualKey::Pause, 0x13);
			bind(VirtualKey::Cancel, 0x03);
			bind(VirtualKey::Clear, 0x0C);
			bind(VirtualKey::Insert, 0x2D);
			bind(VirtualKey::Delete, 0x2E);
			bind(VirtualKey::Home, 0x24);
			bind(VirtualKey::End, 0x23);
			bind(VirtualKey::PageUp, 0x21);
			bind(VirtualKey::PageDown, 0x22);
			bind(VirtualKey::A, 0x41);
			bind(VirtualKey::B, 0x42);
			bind(VirtualKey::C, 0x43);
			bind(VirtualKey::D, 0x44);
			bind(VirtualKey::E, 0x45);
			bind(VirtualKey::F, 0x46);
			bind(VirtualKey::G, 0x47);
			bind(VirtualKey::H, 0x48);
			bind(VirtualKey::I, 0x49);
			bind(VirtualKey::J, 0x4A);
			bind(VirtualKey::K, 0x4B);
			bind(VirtualKey::L, 0x4C);
			bind(VirtualKey::M, 0x4D);
			bind(VirtualKey::N, 0x4E);
			bind(VirtualKey::O, 0x4F);
			bind(VirtualKey::P, 0x50);
			bind(VirtualKey::Q, 0x51);
			bind(VirtualKey::R, 0x52);
			bind(VirtualKey::S, 0x53);
			bind(VirtualKey::T, 0x54);
			bind(VirtualKey::U, 0x55);
			bind(VirtualKey::V, 0x56);
			bind(VirtualKey::W, 0x57);
			bind(VirtualKey::X, 0x58);
			bind(VirtualKey::Y, 0x59);
			bind(VirtualKey::Z, 0x5A);
			bind(VirtualKey::One, 0x31);
			bind(VirtualKey::Two, 0x32);
			bind(VirtualKey::Three, 0x33);
			bind(VirtualKey::Four, 0x34);
			bind(VirtualKey::Five, 0x35);
			bind(VirtualKey::Six, 0x36);
			bind(VirtualKey::Seven, 0x37);
			bind(VirtualKey::Eight, 0x38);
			bind(VirtualKey::Nine, 0x39);
			bind(VirtualKey::Zero, 0x30);
			bind(VirtualKey::App, 0x5D);
			bind(VirtualKey::NumpadDivide, 0x6F);
			bind(VirtualKey::NumpadMultiply, 0x6A);
			bind(VirtualKey::NumpadSubtract, 0x6D);
			bind(VirtualKey::NumpadAdd, 0x6B);
			bind(VirtualKey::NumpadDecimal, 0x6E);
			bind(VirtualKey::NumpadSeparator, 0x6C);
			bind(VirtualKey::NumpadOne, 0x61);
			bind(VirtualKey::NumpadTwo, 0x62);
			bind(VirtualKey::NumpadThree, 0x63);
			bind(VirtualKey::NumpadFour, 0x64);
			bind(VirtualKey::NumpadFive, 0x65);
			bind(VirtualKey::NumpadSix, 0x66);
			bind(VirtualKey::NumpadSeven, 0x67);
			bind(VirtualKey::NumpadEight, 0x68);
			bind(VirtualKey::NumpadNine, 0x69);
			bind(VirtualKey::NumpadZero, 0x60);
			bind(VirtualKey::F1, 0x70);
			bind(VirtualKey::F2, 0x71);
			bind(VirtualKey::F3, 0x72);
			bind(VirtualKey::F4, 0x73);
			bind(VirtualKey::F5, 0x74);
			bind(VirtualKey::F6, 0x75);
			bind(VirtualKey::F7, 0x76);
			bind(VirtualKey::F8, 0x77);
			bind(VirtualKey::F9, 0x78);
			bind(VirtualKey::F10, 0x79);
			bind(VirtualKey::F11, 0x7A);
			bind(VirtualKey::F12, 0x7B);
			bind(VirtualKey::F13, 0x7C);
			bind(VirtualKey::F14, 0x7D);
			bind(VirtualKey::F15, 0x7E);
			bind(VirtualKey::F16, 0x7F);
			bind(VirtualKey::F17, 0x80);
			bind(VirtualKey::F18, 0x81);
			bind(VirtualKey::F19, 0x82);
			bind(VirtualKey::F20, 0x83);
			bind(VirtualKey::F21, 0x84);
			bind(VirtualKey::F22, 0x85);
			bind(VirtualKey::F23, 0x86);
			bind(VirtualKey::F24, 0x87);
			bind(VirtualKey::NumLock, 0x90);
			bind(VirtualKey::ScrollLock, 0x91);
			bind(VirtualKey::CapsLock, 0x14);
			bind(VirtualKey::LeftShift, 0xA0);
			bind(VirtualKey::RightShift, 0xA1);
			bind(VirtualKey::LeftControl, 0xA2);
			bind(VirtualKey::RightControl, 0xA3);
			bind(VirtualKey::LeftSuper, 0x5B);
			bind(VirtualKey::RightSuper, 0x5C);
			bind(VirtualKey::LeftAlt, 0xA4);
			bind(VirtualKey::RightAlt, 0xA5);
			bind(VirtualKey::BrowserBack, 0xA6);
			bind(VirtualKey::BrowserForward, 0xA7);
			bind(VirtualKey::BrowserRefresh, 0xA8);
			bind(VirtualKey::BrowserStop, 0xA9);
			bind(VirtualKey::BrowserSearch, 0xAA);
			bind(VirtualKey::BrowserFavorites, 0xAB);
			bind(VirtualKey::BrowserHome, 0xAC);
			bind(VirtualKey::VolumeMute, 0xAD);
			bind(VirtualKey::VolumeDown, 0xAE);
			bind(VirtualKey::VolumeUp, 0xAF);
			bind(VirtualKey::MediaNextTrack, 0xB0);
			bind(VirtualKey::MediaPreviousTrack, 0xB1);
			bind(VirtualKey::MediaStop, 0xB2);
			bind(VirtualKey::MediaPlay, 0xB3);
			bind(VirtualKey::LaunchMail, 0xB4);
			bind(VirtualKey::LaunchMediaSelect, 0xB5);
			bind(VirtualKey::LaunchApp1, 0xB6);
			bind(VirtualKey::LaunchApp2, 0xB7);

			winVKs[VirtualKey::NumpadEnter] = 0x0D;

			if constexpr (ToVirtual) {
				return virtualKeys;
			} else {
				return winVKs;
			}

		};

		constexpr static std::array<u32, 512> ScancodeToPhysical = GeneratePhysicalKeyTable<true>();
		constexpr static std::array<u16, 256> PhysicalToScancode = GeneratePhysicalKeyTable<false>();
		constexpr static std::array<u16, 256> WinVKToVirtual = GenerateVirtualKeyTable<true>();
		constexpr static std::array<u8, 256> VirtualToWinVK = GenerateVirtualKeyTable<false>();

	}

	constexpr Key scancodeToPhysicalKey(Scancode scancode) {
		return Detail::ScancodeToPhysical[scancode & 0x1FF];
	}

	constexpr Scancode physicalKeyToScancode(Key physicalKey) {

		if (physicalKey >= 0x100) {
			return physicalKey >> 16;
		}

		return Detail::PhysicalToScancode[physicalKey];

	}

	constexpr Key winVKToVirtualKey(Key winVK) {
		return Detail::WinVKToVirtual[winVK & 0xFF];
	}

	constexpr Key virtualKeyToWinVK(Key virtualKey) {

		if (virtualKey >= 0x100) {
			return virtualKey >> 8;
		}

		return Detail::VirtualToWinVK[virtualKey & 0xFF];

	}

	constexpr u32 physicalKeyCount() {
		return Detail::PhysicalToScancode.size();
	}

	constexpr u32 virtualKeyCount() {
		return Detail::VirtualToWinVK.size();
	}

	constexpr u32 scancodeCount() {
		return Detail::ScancodeToPhysical.size();
	}

}