#include <imtcom/CSslEncodingFormatOptionList.h>


// imtcom includes
#include <imtcom/ISslKey.h>


namespace imtcom
{


CSslEncodingFormatOptionList::CSslEncodingFormatOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("PEM", ISslKey::ToString(ISslKey::EF_PEM));
	COptionsManager::InsertOption("DER", ISslKey::ToString(ISslKey::EF_DER));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


