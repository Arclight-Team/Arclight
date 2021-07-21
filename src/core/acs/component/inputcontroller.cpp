#include "inputcontroller.h"
#include "input/inputcontext.h"



void InputController::link(InputContext& context) {
    context.linkHandler(handler);
}