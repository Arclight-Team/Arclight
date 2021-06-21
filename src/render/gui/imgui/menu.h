#pragma once
#include "../common.h"
#include "../control.h"
#include "../container.h"

GUI_BEGIN

class Menu : public IContainer
{
public:

	Menu();
	Menu(const std::string& text, bool enabled = true);

	void create(const std::string& text, bool enabled = true);

	bool isEnabled() const;
	void setEnabled(bool enabled);

	std::string getText() const;
	void setText(const std::string& text);

	template<Control T>
	HandleT addControl(T& control) {
		return IContainer::addControl(&control);
	}
	IControl& getControl(HandleT handle) {
		return IContainer::getControl(handle);
	}

	IMPL_ACTION_SETTER(OnExpanded)
	IMPL_ACTION_SETTER(OnCollapsed)

protected:

	virtual bool begin() override;
	virtual void end() override;

private:

	std::string m_Text;
	bool m_Enabled;

	bool m_Open;
	Action m_OnExpanded;
	Action m_OnCollapsed;

};

GUI_END