#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


namespace imtauthgui
{


class CPersonInfoViewDelegateComp:
	public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CPersonInfoViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtauthgui


