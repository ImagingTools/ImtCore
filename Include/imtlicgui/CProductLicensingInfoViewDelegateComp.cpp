// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


