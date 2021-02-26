#pragma once


// ImtCore includes
#include <imtgui/CStandardCollectionViewDelegateComp.h>


namespace imtlicgui
{


/**
	View delegate implementation related to the license info visualization and editing.
	\ingroup LicenseManagement
	\ingroup Collection
*/
class CLicenseManagerViewDelegateComp: public imtgui::CStandardCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtgui::CStandardCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CLicenseManagerViewDelegateComp);
		I_ASSIGN(m_defaultLicenseNameAttrPtr, "DefaultLicenseName", "Default license name", false, "");
		I_ASSIGN(m_defaultLicenseIdAttrPtr, "DefaultLicenseId", "Default license ID", false, "");
	I_END_COMPONENT;

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual bool RenameObject(const QByteArray& objectId, const QString& newName) const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void OnDuplicateObject(const QByteArray& sourceObjectId, const QByteArray& destinationObjectId) override;
	
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;

protected Q_SLOTS:
	//virtual void OnDuplicateObject(const QByteArray& sourceObjectId, const QByteArray& destinationObjectId);

private:
	I_ATTR(QString, m_defaultLicenseNameAttrPtr);
	I_ATTR(QByteArray, m_defaultLicenseIdAttrPtr);
};


} // namespace imtlicgui


