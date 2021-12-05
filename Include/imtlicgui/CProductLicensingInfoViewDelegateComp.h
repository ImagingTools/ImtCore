#pragma once


// ImtCore includes
#include <imtgui/CDocumentCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CProductLicensingInfoViewDelegateComp:
			public imtgui::CDocumentCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtgui::CDocumentCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoViewDelegateComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual QByteArray CreateNewObject(
				const QByteArray& typeId,
				const QString& objectName,
				const QString& description,
				const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual void SetupSummaryInformation() override;
};


} // namespace imtlicgui


