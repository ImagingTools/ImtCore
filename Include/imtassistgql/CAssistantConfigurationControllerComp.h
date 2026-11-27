// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassist/IAssistantConfigurationStore.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace imtassistgql
{


/**
	GraphQL controller of the assistant configuration of a tenant, used by the page
	"Assistant" of the administration.

	Commands:
	- \c AssistantGetConfiguration: no input; returns the configuration of the tenant of the
	  request (\c configurationJson) without API key, only \c hasApiKey, together with the
	  providers allowed by the server;
	- \c AssistantSetConfiguration: \c input with \c enabled, \c presetId, \c model, \c endpoint,
	  \c region, \c project, \c resource, \c maxOutputTokens, \c additionalInstructions,
	  \c apiKeyChange (\c keep, \c set or \c clear) and \c apiKey (only for \c set);
	  returns \c success and the stored configuration.

	The tenant is always the one of the authenticated request, never one of the input.
	Besides the command permissions of the composite, the controller requires the permissions
	\c ViewPermission and \c EditPermission itself, so that a composite without command
	permissions does not open the configuration to every user. The API key is never returned
	and never logged.

	\ingroup imtassistgql
*/
class CAssistantConfigurationControllerComp: public imtservergql::CPermissibleGqlRequestHandlerComp
{
public:
	typedef imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_COMPONENT(CAssistantConfigurationControllerComp);
		I_ASSIGN(m_configurationStoreCompPtr, "ConfigurationStore", "Assistant configurations of the tenants", true, "ConfigurationStore");
		I_ASSIGN(m_viewPermissionAttrPtr, "ViewPermission", "Permission needed to read the configuration", true, "/Administration/Assistant/View");
		I_ASSIGN(m_editPermissionAttrPtr, "EditPermission", "Permission needed to change the configuration", true, "/Administration/Assistant/Edit");
	I_END_COMPONENT;

	static const QByteArray s_getConfigurationCommandId;
	static const QByteArray s_setConfigurationCommandId;

protected:
	/**
		Check that the caller is authenticated in a tenant and holds the permission.
		\return The tenant ID, empty if refused.
	*/
	QByteArray CheckAccess(const imtgql::CGqlRequest& gqlRequest, const QByteArray& permission, QString& errorMessage) const;

	QJsonObject CreateConfigurationResponse(const imtassist::IAssistantConfiguration& configuration) const;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtassist::IAssistantConfigurationStore, m_configurationStoreCompPtr);
	I_ATTR(QByteArray, m_viewPermissionAttrPtr);
	I_ATTR(QByteArray, m_editPermissionAttrPtr);
};


} // namespace imtassistgql


