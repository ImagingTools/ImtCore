#include <imtcom/CSslEncodingFormatOptionList.h>


// imtcom includes
#include <imtcom/ISslConfigurationManager.h>


namespace imtcom
{


CSslEncodingFormatOptionList::CSslEncodingFormatOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("PEM", ISslConfigurationManager::ToString(ISslConfigurationManager::EF_PEM));
	COptionsManager::InsertOption("DER", ISslConfigurationManager::ToString(ISslConfigurationManager::EF_DER));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


