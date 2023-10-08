#pragma once


// ImtCore includes
#include <imtgui/CStandardCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CFeatureCollectionViewDelegateComp:
			public imtgui::CStandardCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtgui::CStandardCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionViewDelegateComp);
	I_END_COMPONENT;

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual bool GetSummaryInformation(
				const QByteArray& objectId,
				const QVector<QByteArray> fieldIds,
				ObjectMetaInfo& objectMetaInfo) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtlicgui


