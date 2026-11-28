// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassist/IAssistantConfiguration.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtassistdb
{


/**
	SQL delegate of the assistant configurations of the tenants (table "AssistantConfigurations").
	The object ID is the tenant ID. The API key is stored only in the protected form of the
	configuration, inside the serialized data.

	\ingroup imtassistdb
*/
class CAssistantConfigurationDbDelegateComp: public imtauthdb::CSerializedObjectDbDelegateCompBase
{
public:
	typedef imtauthdb::CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantConfigurationDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "ConfigurationFactory", "Factory of the stored objects", true, "AssistantConfiguration");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;

private:
	I_FACT(imtassist::IAssistantConfiguration, m_dataFactoryCompPtr);
};


} // namespace imtassistdb


