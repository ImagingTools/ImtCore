#include <imtlicgui/CProductLicensingInfoCollectionViewDelegateComp.h>


namespace imtlicgui
{


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CProductLicensingInfoCollectionViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace aculainspgui


