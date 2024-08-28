#include <imtcom/CSslVerifyModeOptionList.h>


// imtcom includes
#include <imtcom/ISslConfigurationManager.h>


namespace imtcom
{


CSslVerifyModeOptionList::CSslVerifyModeOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("None", ISslConfigurationManager::ToString(ISslConfigurationManager::PVM_NONE));
	COptionsManager::InsertOption("Query", ISslConfigurationManager::ToString(ISslConfigurationManager::PVM_QUERY));
	COptionsManager::InsertOption("Verify", ISslConfigurationManager::ToString(ISslConfigurationManager::PVM_VERIFY));
	COptionsManager::InsertOption("Auto", ISslConfigurationManager::ToString(ISslConfigurationManager::PVM_AUTO_VERIFY));
	SetSelectedOptionIndex(2);
}


} // namespace imtcom


