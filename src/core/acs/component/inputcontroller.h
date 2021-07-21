#pragma once

#include "component.h"
#include "input/inputhandler.h"


class InputContext;

class InputController : public IComponent {

public:

    InputController() = default;

    void setActionCallback(InputHandler::ActionListener callback) { handler.setActionListener(callback); }
    void setContinuousCallback(InputHandler::CoActionListener callback) { handler.setCoActionListener(callback); }
    void setKeyCallback(InputHandler::KeyListener callback) { handler.setKeyListener(callback); }
    void setCharCallback(InputHandler::CharListener callback) { handler.setCharListener(callback); }
    void setCursorCallback(InputHandler::CursorListener callback) { handler.setCursorListener(callback); }
    void setScrollCallback(InputHandler::ScrollListener callback) { handler.setScrollListener(callback); }

    void disableAll() {

        setActionCallback(nullptr);
        setContinuousCallback(nullptr);
        setKeyCallback(nullptr);
        setCharCallback(nullptr);
        setCursorCallback(nullptr);
        setScrollCallback(nullptr);

    }

    void link(InputContext& context);

    InputHandler handler;

};