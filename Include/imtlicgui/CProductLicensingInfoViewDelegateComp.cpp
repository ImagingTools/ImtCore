#include <imtlicgui/CProductLicensingInfoViewDelegateComp.h>


namespace imtlicgui
{


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CProductLicensingInfoViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace aculainspgui


