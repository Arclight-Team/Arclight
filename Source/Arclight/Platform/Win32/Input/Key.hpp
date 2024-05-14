/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Key.hpp
 */

#pragma once

#include "Common/Types.hpp"



using Key       = u32;
using KeyChar   = u32;
using KeyAction = u32;
using Scancode  = u32;


/*
 *	Arclight distinguishes multiple types of key codes:
 *	- Scancode:				Raw key code emitted by hardware;
 *							This code is almost always locked to a specific physical position, but exceptions exist.
 *							Certain key combinations trigger an emulated scancode that is not produced by hardware.
 *	- Physical Key Code:	Key codes emitted by Arclight, consisting of transformed scancodes.
 *							If a given scancode is not representable by a constant, the raw scancode (flagged) is emitted instead.
 *	- Virtual Key Code:		Key codes emitted by Arclight, consisting of transformed virtual OS keys.
 *							The OS may merge/split certain keys into one constant not necessarily bound to a physical key.
 *							These depend on the OS and the currently used keyboard layout.
 */
struct PhysicalKey {

	enum : Key {

		A = 0x4,
		B = 0x5,
		C = 0x6,
		D = 0x7,
		E = 0x8,
		F = 0x9,
		G = 0xA,
		H = 0xB,
		I = 0xC,
		J = 0xD,
		K = 0xE,
		L = 0xF,
		M = 0x10,
		N = 0x11,
		O = 0x12,
		P = 0x13,
		Q = 0x14,
		R = 0x15,
		S = 0x16,
		T = 0x17,
		U = 0x18,
		V = 0x19,
		W = 0x1A,
		X = 0x1B,
		Y = 0x1C,
		Z = 0x1D,
		One = 0x1E,
		Two = 0x1F,
		Three = 0x20,
		Four = 0x21,
		Five = 0x22,
		Six = 0x23,
		Seven = 0x24,
		Eight = 0x25,
		Nine = 0x26,
		Zero = 0x27,
		Return = 0x28,
		Escape = 0x29,
		Backspace = 0x2A,
		Tab = 0x2B,
		Space = 0x2C,
		Minus = 0x2D,
		Equal = 0x2E,
		LeftSquareBracket = 0x2F,
		RightSquareBracket = 0x30,
		Backslash = 0x31,
		AngularBracket = 0x32,
		Semicolon = 0x33,
		Apostrophe = 0x34,
		GraveAccent = 0x35,
		Comma = 0x36,
		Period = 0x37,
		Slash = 0x38,
		CapsLock = 0x39,
		F1 = 0x3A,
		F2 = 0x3B,
		F3 = 0x3C,
		F4 = 0x3D,
		F5 = 0x3E,
		F6 = 0x3F,
		F7 = 0x40,
		F8 = 0x41,
		F9 = 0x42,
		F10 = 0x43,
		F11 = 0x44,
		F12 = 0x45,
		PrintScreen = 0x46,
		ScrollLock = 0x47,
		Pause = 0x48,
		Insert = 0x49,
		Home = 0x4A,
		PageUp = 0x4B,
		Delete = 0x4C,
		End = 0x4D,
		PageDown = 0x4E,
		Right = 0x4F,
		Left = 0x50,
		Down = 0x51,
		Up = 0x52,
		NumLock = 0x53,
		NumpadDivide = 0x54,
		NumpadMultiply = 0x55,
		NumpadSubtract = 0x56,
		NumpadAdd = 0x57,
		NumpadEnter = 0x58,
		NumpadOne = 0x59,
		NumpadTwo = 0x5A,
		NumpadThree = 0x5B,
		NumpadFour = 0x5C,
		NumpadFive = 0x5D,
		NumpadSix = 0x5E,
		NumpadSeven = 0x5F,
		NumpadEight = 0x60,
		NumpadNine = 0x61,
		NumpadZero = 0x62,
		NumpadDecimal = 0x63,
		OEM1 = 0x64,
		App = 0x65,
		Power = 0x66,
		NumpadEqual = 0x67,
		F13 = 0x68,
		F14 = 0x69,
		F15 = 0x6A,
		F16 = 0x6B,
		F17 = 0x6C,
		F18 = 0x6D,
		F19 = 0x6E,
		F20 = 0x6F,
		F21 = 0x70,
		F22 = 0x71,
		F23 = 0x72,
		F24 = 0x73,
		LeftControl = 0xE0,
		LeftShift = 0xE1,
		LeftAlt = 0xE2,
		LeftSuper = 0xE3,
		RightControl = 0xE4,
		RightShift = 0xE5,
		RightAlt = 0xE6,
		RightSuper = 0xE7,

		MouseButton1 = 0xF0,
		MouseButton2 = 0xF1,
		MouseButton3 = 0xF2,
		MouseButton4 = 0xF3,
		MouseButton5 = 0xF4,

		MouseLeft = MouseButton1,
		MouseRight = MouseButton2,
		MouseMiddle = MouseButton3

	};

};


struct VirtualKey {

	enum : Key {

		Back = 0x1,
		Tab,
		Return,
		Escape,
		Space,
		Left,
		Right,
		Up,
		Down,
		PrintScreen,
		Pause,
		Cancel,
		Clear,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Zero,
		App,
		NumpadDivide,
		NumpadMultiply,
		NumpadSubtract,
		NumpadAdd,
		NumpadEnter,
		NumpadDecimal,
		NumpadSeparator,
		NumpadOne,
		NumpadTwo,
		NumpadThree,
		NumpadFour,
		NumpadFive,
		NumpadSix,
		NumpadSeven,
		NumpadEight,
		NumpadNine,
		NumpadZero,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
		NumLock,
		ScrollLock,
		CapsLock,
		LeftShift,
		RightShift,
		LeftControl,
		RightControl,
		LeftSuper,
		RightSuper,
		LeftAlt,
		RightAlt,
		BrowserBack,
		BrowserForward,
		BrowserRefresh,
		BrowserStop,
		BrowserSearch,
		BrowserFavorites,
		BrowserHome,
		VolumeMute,
		VolumeDown,
		VolumeUp,
		MediaNextTrack,
		MediaPreviousTrack,
		MediaStop,
		MediaPlay,
		LaunchMail,
		LaunchMediaSelect,
		LaunchApp1,
		LaunchApp2,

		MouseButton1,
		MouseButton2,
		MouseButton3,
		MouseButton4,
		MouseButton5,

		MouseLeft = MouseButton1,
		MouseRight = MouseButton2,
		MouseMiddle = MouseButton3

	};

};



enum class KeyState {
	Released,
	Pressed,
	Held
};



struct KeyModifier {

	enum : u32 {
		None = 0x0,
		Control = 0x1,
		Shift = 0x2,
		Alt = 0x4,
		Super = 0x8
	};

};