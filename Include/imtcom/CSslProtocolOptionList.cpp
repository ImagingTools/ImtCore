#include <imtcom/CSslProtocolOptionList.h>


// imtcom includes
#include <imtcom/ISslConfiguration.h>


namespace imtcom
{


CSslProtocolOptionList::CSslProtocolOptionList(): CStaticOptionList()
{
	SetSelectionSerialized(true);

	// we should call direct base-method to insert options
	// clazy:exclude=skipped-base-method
	COptionsManager::InsertOption("Unknown", ISslConfiguration::ToString(ISslConfiguration::SP_UNKNOWN_PROTOCOL));
	COptionsManager::InsertOption("Default", ISslConfiguration::ToString(ISslConfiguration::SP_SECURE_PROTOCOLS));
	COptionsManager::InsertOption("Any", ISslConfiguration::ToString(ISslConfiguration::SP_ANY_PROTOCOL));
	COptionsManager::InsertOption("TLSv1.0", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_0));
	COptionsManager::InsertOption("TLSv1.0 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_0_OR_LATER));
	COptionsManager::InsertOption("TLSv1.1.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_1));
	COptionsManager::InsertOption("TLSv1.1 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_1_OR_LATER));
	COptionsManager::InsertOption("TLSv1.2.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_2));
	COptionsManager::InsertOption("TLSv1.2 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_2_OR_LATER));
	COptionsManager::InsertOption("DTLSv1.0", ISslConfiguration::ToString(ISslConfiguration::SP_DTLS_V1_0));
	COptionsManager::InsertOption("DTLSv1.0 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_DTLS_V1_0_OR_LATER));
	COptionsManager::InsertOption("DTLSv1.2", ISslConfiguration::ToString(ISslConfiguration::SP_DTLS_V1_2));
	COptionsManager::InsertOption("DTLSv1.2 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_DTLS_V1_2_OR_LATER));
	COptionsManager::InsertOption("TLSv1.3.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_3));
	COptionsManager::InsertOption("TLSv1.3 and later versions.", ISslConfiguration::ToString(ISslConfiguration::SP_TLS_V1_3_OR_LATER));

	SetSelectedOptionIndex(GetOptionsCount() - 1);
}


} // namespace imtcom


