#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgui/CGuiElementContainerComp.h>
#include <imtgui/CGuiElementModelComp.h>
#include <imtgui/IGuiElementContainer.h>
#include <imtbase/ICommandGroup.h>


namespace imtgui
{


class CCommandGroupComp:
			virtual public imtbase::ICommandGroup,
			public imtgui::CGuiElementModelComp
{
public:
	typedef CGuiElementModelComp BaseClass;

	I_BEGIN_COMPONENT(CCommandGroupComp);
		I_REGISTER_INTERFACE(imtbase::ICommandGroup);
		I_ASSIGN(m_alignmentAttrPtr, "Alignment", "1 - Left alignment, 2 - Right alignment, 4 - Center alignment", false, 1);
	I_END_COMPONENT;

	// reimplemented (imtgui::ICommandGroup)
	virtual int GetAlignment() const override;

protected:
	I_ATTR(int, m_alignmentAttrPtr);
};


} // namespace imtgui


