// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	SetSelectedOptionIndex(3);
}


} // namespace imtcom


