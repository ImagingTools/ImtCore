#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CProductLicensingInfoViewDelegateComp:
			public imtrepogui::CDocumentCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoViewDelegateComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtlicgui


