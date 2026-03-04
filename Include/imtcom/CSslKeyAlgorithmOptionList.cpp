// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CSslKeyAlgorithmOptionList.h>


// imtcom includes
#include <imtcom/ISslConfigurationManager.h>


namespace imtcom
{


CSslKeyAlgorithmOptionList::CSslKeyAlgorithmOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("RSA", ISslConfigurationManager::ToString(ISslConfigurationManager::KA_RSA));
	COptionsManager::InsertOption("DSA", ISslConfigurationManager::ToString(ISslConfigurationManager::KA_DSA));
	COptionsManager::InsertOption("Elliptic Curve", ISslConfigurationManager::ToString(ISslConfigurationManager::KA_EC));
	COptionsManager::InsertOption("Diffie-Hellma", ISslConfigurationManager::ToString(ISslConfigurationManager::KA_DH));
	COptionsManager::InsertOption("Unknown", ISslConfigurationManager::ToString(ISslConfigurationManager::KA_OPAQUE));
	SetSelectedOptionIndex(0);
}


} // namespace imtcom


