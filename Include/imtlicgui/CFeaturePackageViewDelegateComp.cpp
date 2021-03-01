#include <imtlicgui/CFeaturePackageViewDelegateComp.h>


namespace imtlicgui
{


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CFeaturePackageViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


} // namespace aculainspgui


