#include <imtcom/CSslKeyTypeOptionList.h>


// imtcom includes
#include <imtcom/ISslKey.h>


namespace imtcom
{


CSslKeyTypeOptionList::CSslKeyTypeOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("Private", ISslKey::ToString(ISslKey::KT_PRIVATE_KEY));
	COptionsManager::InsertOption("Public", ISslKey::ToString(ISslKey::KT_PUBLIC_KEY));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


