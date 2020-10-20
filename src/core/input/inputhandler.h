#pragma once

#include "core/input/keytrigger.h"
#include "core/input/inputevent.h"



class InputContext;

class InputHandler {

public:

	typedef bool(*ActionListener)(KeyAction);
	typedef bool(*KeyListener)(Key, KeyState);
	typedef bool(*CharListener)(KeyChar);
	typedef bool(*CursorListener)(double, double);
	typedef bool(*ScrollListener)(double, double);

	inline constexpr InputHandler() : context(nullptr), actionListener(nullptr), keyListener(nullptr), charListener(nullptr), cursorListener(nullptr), scrollListener(nullptr) {};

	virtual ~InputHandler();

	InputHandler(const InputHandler& handler) = delete;
	InputHandler& operator=(const InputHandler& handler) = delete;

	inline void setActionListener(ActionListener listener) {
		actionListener = listener;
	}

	inline void setKeyListener(KeyListener listener) {
		keyListener = listener;
	}

	inline void setCharListener(CharListener listener) {
		charListener = listener;
	}

	inline void setCursorListener(CursorListener listener) {
		cursorListener = listener;
	}

	inline void setScrollListener(ScrollListener listener) {
		scrollListener = listener;
	}

private:

	friend class InputContext;

	InputContext* context;
	ActionListener actionListener;
	KeyListener keyListener;
	CharListener charListener;
	CursorListener cursorListener;
	ScrollListener scrollListener;

};