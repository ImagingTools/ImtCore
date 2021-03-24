#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


namespace imtauthgui
{


class CAccountInfoViewDelegateComp:
	public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CAccountInfoViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtauthgui


