#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CFeaturePackageViewDelegateComp:
	public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtlicgui


