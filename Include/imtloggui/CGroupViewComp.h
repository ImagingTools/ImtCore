#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroupVisualSettingsProvider.h>
#include <imtloggui/CGroupComp.h>


namespace imtloggui
{


class CGroupViewComp: public CGroupComp, virtual public  IGroupVisualSettingsProvider
{
public:
	typedef CGroupComp BaseClass;

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_ASSIGN(m_colorAttrPtr, "BackgroundColor", "Group background color", false, "#080808");
		I_ASSIGN(m_heightAttrPtr, "Height", "Group pixel height", false, 200);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupVisualSettingsProvider)
	virtual QColor GetBackgroundColor() const override;
	virtual int GetHeight() const override;

private:
	I_ATTR(QString, m_colorAttrPtr);
	I_ATTR(int, m_heightAttrPtr);
};


} // namespace imtloggui


