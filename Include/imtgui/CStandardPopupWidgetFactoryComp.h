#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgui/IPopupWidget.h>


namespace imtgui
{


class CStandardPopupWidgetFactoryComp:
	public icomp::CComponentBase,
	virtual public IPopupWidgetFactory
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStandardPopupWidgetFactoryComp);
		I_REGISTER_INTERFACE(IPopupWidgetFactory);
	I_END_COMPONENT;

	// reimplemented (istd::TIFactory)
	virtual IPopupWidget* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	virtual KeyList GetFactoryKeys() const override;
};


} // namespace imtgui


