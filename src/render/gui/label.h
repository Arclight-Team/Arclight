#pragma once
#include "common.h"
#include "control.h"

GUI_BEGIN

class Label : public IControl
{
public:

	Label();
	Label(const std::string& text);

	void create(const std::string& text);

	std::string getText() const;
	void setText(const std::string& text);

protected:

	virtual void update() override;

private:

	std::string m_Text;

};

GUI_END