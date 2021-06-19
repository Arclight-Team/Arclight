#pragma once
#include "common.h"
#include "control.h"

GUI_BEGIN

inline HandleT nextHandleID = 0;

class IControl;

class IContainer : public IControl
{
public:

	bool controlExists(HandleT handle);
	bool swapControls(HandleT first, HandleT second);

protected:

	HandleT getNextHandle();
	i64 getControlIndex(HandleT handle);
	HandleT addControl(IControl* control);
	IControl& getControl(HandleT handle);

	virtual void update() override;

protected:

	std::vector<std::pair<HandleT,IControl*>> m_Controls;
	bool m_CondAlwaysEnd = false;
};

GUI_END