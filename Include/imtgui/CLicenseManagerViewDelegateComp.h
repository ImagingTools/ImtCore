#pragma once


// ImtCore includes
#include <imtgui/CStandardCollectionDelegateComp.h>


namespace imtgui
{


/**
	View delegate implementation related to the license info visualization and editing.
	\ingroup LicenseManagement
	\ingroup Collection
*/
class CLicenseManagerViewDelegateComp: public CStandardCollectionDelegateComp
{
	Q_OBJECT
public:
	typedef CStandardCollectionDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CLicenseManagerViewDelegateComp);
		I_ASSIGN(m_defaultLicenseNameAttrPtr, "DefaultLicenseName", "Default license name", false, "");
		I_ASSIGN(m_defaultLicenseIdAttrPtr, "DefaultLicenseId", "Default license ID", false, "");
		I_ASSIGN(m_defaultPackageIdAttrPtr, "DefaultPackageId", "Default package ID", false, "");
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;

private:
	I_ATTR(QString, m_defaultLicenseNameAttrPtr);
	I_ATTR(QByteArray, m_defaultLicenseIdAttrPtr);
	I_ATTR(QByteArray, m_defaultPackageIdAttrPtr);
};


} // namespace imtgui


