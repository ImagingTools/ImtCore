#pragma once


// ImtCore includes
#include <imtgui/CDocumentCollectionViewDelegateComp.h>


namespace imtauthgui
{


class CContactInfoViewDelegateComp: public imtgui::CDocumentCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtgui::CDocumentCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CContactInfoViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual bool GetSummaryInformation(
				const QByteArray& objectId,
				const QVector<QByteArray> fieldIds,
				ObjectMetaInfo& objectMetaInfo) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtauthgui


