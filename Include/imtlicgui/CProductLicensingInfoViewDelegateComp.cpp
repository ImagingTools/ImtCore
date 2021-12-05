#include <imtlicgui/CProductLicensingInfoViewDelegateComp.h>


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicgui
{


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

QByteArray CProductLicensingInfoViewDelegateComp::CreateNewObject(
			const QByteArray& typeId,
			const QString& objectName,
			const QString& description,
			const istd::IChangeable* defaultDataPtr) const
{
	QByteArray retVal = BaseClass::CreateNewObject(typeId, objectName, description, defaultDataPtr);
	
	return retVal;
}


void CProductLicensingInfoViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace imtlicgui


