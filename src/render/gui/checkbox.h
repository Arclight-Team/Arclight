#pragma once
#include "common.h"
#include "control.h"
#include "action.h"

GUI_BEGIN

class CheckBox : public IControl
{
public:

	CheckBox();
	CheckBox(const std::string& text, bool checked = false);

	void create(const std::string& text, bool checked = false);

	bool isChecked() const;
	void setChecked(bool checked);

	std::string getText() const;
	void setText(const std::string& text);

	IMPL_ACTION_SETTER(OnClicked)

protected:

	virtual void update() override;

private:

	std::string m_Text;
	bool m_Checked;

	Action m_OnClicked;

};

GUI_END