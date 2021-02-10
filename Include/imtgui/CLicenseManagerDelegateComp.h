#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/IDialog.h>

// ImtCore includes
#include <imtgui/CStandardCollectionDelegateComp.h>


namespace imtgui
{


/**
	\ingroup Collection
*/
class CLicenseManagerDelegateComp: public CStandardCollectionDelegateComp
{
	Q_OBJECT
public:
	typedef CStandardCollectionDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CLicenseManagerDelegateComp);
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
};


} // namespace imtgui


