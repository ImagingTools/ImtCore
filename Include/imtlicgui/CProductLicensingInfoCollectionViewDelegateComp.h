#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CProductLicensingInfoCollectionViewDelegateComp:
			public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoCollectionViewDelegateComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtlicgui


