// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtassist/CAssistantService.h>


namespace imtassist
{


/**
	Component of the assistant service.
	The assistant is disabled unless the attribute \c Enabled is set.

	\ingroup imtassist
*/
class CAssistantServiceComp:
			public ilog::CLoggerComponentBase,
			public CAssistantService
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantServiceComp);
		I_REGISTER_INTERFACE(IAssistantService);
		I_ASSIGN(m_isEnabledAttrPtr, "Enabled", "Enable the assistant. While disabled, no request is sent to a provider; overridden by EnabledParam", true, false);
		I_ASSIGN(m_enabledParamCompPtr, "EnabledParam", "Setting of the server that enables the assistant; Enabled when unset", false, "EnabledParam");
		I_ASSIGN(m_tenantPolicyCompPtr, "TenantPolicy", "Decision per tenant, e.g. the assistant configuration of the tenant; all tenants when unset", false, "TenantPolicy");
		I_ASSIGN(m_providerCompPtr, "Provider", "Adapter to the model provider", false, "Provider");
		I_ASSIGN(m_actionRegistryCompPtr, "ActionRegistry", "Registry of the actions available to the assistant", true, "ActionRegistry");
		I_ASSIGN(m_representationRegistryCompPtr, "RepresentationRegistry", "Registry of the known UI representation types", false, "RepresentationRegistry");
		I_ASSIGN(m_auditLogCompPtr, "AuditLog", "Receiver of technical events and action decisions", false, "AuditLog");
		I_ASSIGN(m_providerTimeoutAttrPtr, "ProviderTimeout", "Timeout of one provider call in milliseconds", true, CAssistantService::DEFAULT_PROVIDER_TIMEOUT_MS);
		I_ASSIGN(m_maxStepsPerTurnAttrPtr, "MaxStepsPerTurn", "Maximum number of provider calls in one turn", true, CAssistantService::DEFAULT_MAX_STEPS_PER_TURN);
		I_ASSIGN(m_previewTimeToLiveAttrPtr, "PreviewTimeToLive", "Time to live of a prepared change in seconds", true, CActionPreviewStore::DEFAULT_TIME_TO_LIVE_SEC);
	I_END_COMPONENT;

protected:
	// reimplemented (imtassist::CAssistantService)
	virtual bool IsAssistantEnabled() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_ATTR(bool, m_isEnabledAttrPtr);
	I_REF(iprm::IEnableableParam, m_enabledParamCompPtr);
	I_REF(IAssistantTenantPolicy, m_tenantPolicyCompPtr);
	I_REF(IAssistantProvider, m_providerCompPtr);
	I_REF(IAssistantActionRegistry, m_actionRegistryCompPtr);
	I_REF(IUiRepresentationRegistry, m_representationRegistryCompPtr);
	I_REF(IAssistantAuditLog, m_auditLogCompPtr);
	I_ATTR(int, m_providerTimeoutAttrPtr);
	I_ATTR(int, m_maxStepsPerTurnAttrPtr);
	I_ATTR(int, m_previewTimeToLiveAttrPtr);
};


} // namespace imtassist


