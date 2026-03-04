// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CSslProtocolOptionList.h>


// imtcom includes
#include <imtcom/ISslConfigurationManager.h>


namespace imtcom
{


CSslProtocolOptionList::CSslProtocolOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("Unknown", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_UNKNOWN_PROTOCOL));
	COptionsManager::InsertOption("Secure Protocols", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_SECURE_PROTOCOLS));
	COptionsManager::InsertOption("Any", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_ANY_PROTOCOL));
	COptionsManager::InsertOption("TLSv1.0", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_0));
	COptionsManager::InsertOption("TLSv1.0 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_0_OR_LATER));
	COptionsManager::InsertOption("TLSv1.1.", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_1));
	COptionsManager::InsertOption("TLSv1.1 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_1_OR_LATER));
	COptionsManager::InsertOption("TLSv1.2.", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_2));
	COptionsManager::InsertOption("TLSv1.2 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_2_OR_LATER));
	COptionsManager::InsertOption("DTLSv1.0", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_DTLS_V1_0));
	COptionsManager::InsertOption("DTLSv1.0 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_DTLS_V1_0_OR_LATER));
	COptionsManager::InsertOption("DTLSv1.2", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_DTLS_V1_2));
	COptionsManager::InsertOption("DTLSv1.2 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_DTLS_V1_2_OR_LATER));
	COptionsManager::InsertOption("TLSv1.3.", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_3));
	COptionsManager::InsertOption("TLSv1.3 and later versions", ISslConfigurationManager::ToString(ISslConfigurationManager::SP_TLS_V1_3_OR_LATER));

	SetSelectedOptionIndex(8);
}


} // namespace imtcom


