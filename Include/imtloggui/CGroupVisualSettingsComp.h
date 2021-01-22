#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroupVisualSettings.h>


namespace imtloggui
{


class CGroupVisualSettingsComp: public icomp::CComponentBase, virtual public IGroupVisualSettings
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupVisualSettingsComp)
		I_ASSIGN(m_colorAttrPtr, "BackgroundColor", "Group background color", false, "#080808");
		I_ASSIGN(m_heightAttrPtr, "Height", "Group pixel height", false, 200);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupVisualSettings)
	virtual QColor GetBackgroundColor() const override;
	virtual int GetHeight() const override;

private:
	I_ATTR(QString, m_colorAttrPtr);
	I_ATTR(int, m_heightAttrPtr);
};


} // namespace imtloggui


