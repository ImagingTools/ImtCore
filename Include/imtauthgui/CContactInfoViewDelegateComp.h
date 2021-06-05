#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentCollectionViewDelegateComp.h>


namespace imtauthgui
{


class CContactInfoViewDelegateComp:
	public imtrepogui::CDocumentCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CContactInfoViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtauthgui


