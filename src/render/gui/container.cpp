#include "container.h"

GUI_BEGIN

bool IContainer::swapControls(HandleT first, HandleT second) {

	i64 indexA = getControlIndex(first);
	if (indexA < 0) {
		gui_force_assert("First control handle is invalid");
		return false;
	}

	i64 indexB = getControlIndex(second);
	if (indexB < 0) {
		gui_force_assert("Second control handle is invalid");
		return false;
	}

	auto tmp = m_Controls[indexA];
	m_Controls[indexA] = m_Controls[indexB];
	m_Controls[indexB] = tmp;

	return true;

}

bool IContainer::controlExists(HandleT handle) {
	return getControlIndex(handle) >= 0;
}



HandleT IContainer::getNextHandle() {
	return ++nextHandleID;
}

i64 IContainer::getControlIndex(HandleT handle) {

	if (handle == InvalidHandle)
		return -1;

	for (i64 i = 0; auto& p : m_Controls) {
		if (p.first == handle) {
			return i;
		}
		i++;
	}

	return -1;

}

HandleT IContainer::addControl(IControl* control) {

	if (control == this) {
		gui_force_assert("Control can't be added to itself");
		return InvalidHandle;
	}

	HandleT handle = getNextHandle();

	m_Controls.emplace_back(handle, control);

	return handle;

}

IControl& IContainer::getControl(HandleT handle) {

	i64 index = getControlIndex(handle);
	if (index < 0) {
		gui_force_assert("Control handle is invalid");
		arc_abort();
	}

	if (!m_Controls[index].second) {
		gui_force_assert("Control handle is not linked to an active control");
		arc_abort();
	}

	return *m_Controls[index].second;

}



void IContainer::update() {

	if (begin()) {
		for (auto& p : m_Controls) {
			if (p.second) {
				p.second->update();
			}
		}
	
		end();
		return;
	}

	if (m_CondAlwaysEnd) {
		end();
	}

}

GUI_END