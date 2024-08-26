#include <imtcom/CSslVerifyModeOptionList.h>


// imtcom includes
#include <imtcom/ISslConfiguration.h>


namespace imtcom
{


CSslVerifyModeOptionList::CSslVerifyModeOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("None", ISslConfiguration::ToString(ISslConfiguration::PVM_NONE));
	COptionsManager::InsertOption("Query", ISslConfiguration::ToString(ISslConfiguration::PVM_QUERY));
	COptionsManager::InsertOption("Verify", ISslConfiguration::ToString(ISslConfiguration::PVM_VERIFY));
	COptionsManager::InsertOption("Auto", ISslConfiguration::ToString(ISslConfiguration::PVM_AUTO_VERIFY));
	SetSelectedOptionIndex(2);
}


} // namespace imtcom


