#include <imtcom/CSslKeyAlgorithmOptionList.h>


// imtcom includes
#include <imtcom/ISslKey.h>


namespace imtcom
{


CSslKeyAlgorithmOptionList::CSslKeyAlgorithmOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("RSA", ISslKey::ToString(ISslKey::KA_RSA));
	COptionsManager::InsertOption("DSA", ISslKey::ToString(ISslKey::KA_DSA));
	COptionsManager::InsertOption("Elliptic Curve", ISslKey::ToString(ISslKey::KA_EC));
	COptionsManager::InsertOption("Diffie-Hellma", ISslKey::ToString(ISslKey::KA_DH));
	COptionsManager::InsertOption("Unknown", ISslKey::ToString(ISslKey::KA_OPAQUE));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


