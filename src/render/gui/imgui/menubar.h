#pragma once
#include "../common.h"
#include "../control.h"
#include "../container.h"

GUI_BEGIN

class MenuBar : public IContainer
{
public:

	MenuBar();

	template<Control T>
	HandleT addControl(T& control) {
		return IContainer::addControl(&control);
	}
	IControl& getControl(HandleT handle) {
		return IContainer::getControl(handle);
	}

	using IContainer::update;

protected:

	virtual void update() override {
		IContainer::update();
	}
	virtual bool begin() override;
	virtual void end() override;


private:

};

GUI_END