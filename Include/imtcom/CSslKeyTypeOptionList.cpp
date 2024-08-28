#include <imtcom/CSslKeyTypeOptionList.h>


// imtcom includes
#include <imtcom/ISslConfigurationManager.h>


namespace imtcom
{


CSslKeyTypeOptionList::CSslKeyTypeOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("Private", ISslConfigurationManager::ToString(ISslConfigurationManager::KT_PRIVATE_KEY));
	COptionsManager::InsertOption("Public", ISslConfigurationManager::ToString(ISslConfigurationManager::KT_PUBLIC_KEY));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


