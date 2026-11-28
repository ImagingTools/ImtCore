// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantLogicControllerComp.h>


namespace imtassist
{


// protected methods

// reimplemented (imtassist::CAssistantLogicController)

bool CAssistantLogicControllerComp::IsAssistantEnabled() const
{
	if (m_enabledParamCompPtr.IsValid()){
		return m_enabledParamCompPtr->IsEnabled();
	}

	return CAssistantLogicController::IsAssistantEnabled();
}


// reimplemented (icomp::CComponentBase)

void CAssistantLogicControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetProvider(m_providerCompPtr.GetPtr());
	SetTenantPolicy(m_tenantPolicyCompPtr.GetPtr());
	SetActionRegistry(m_actionRegistryCompPtr.GetPtr());
	SetRepresentationRegistry(m_representationRegistryCompPtr.GetPtr());
	SetAuditLog(m_auditLogCompPtr.GetPtr());

	if (m_providerTimeoutAttrPtr.IsValid()){
		SetProviderTimeout(*m_providerTimeoutAttrPtr);
	}

	if (m_maxStepsPerTurnAttrPtr.IsValid()){
		SetMaxStepsPerTurn(*m_maxStepsPerTurnAttrPtr);
	}

	if (m_previewTimeToLiveAttrPtr.IsValid()){
		GetPreviewStore().SetTimeToLive(*m_previewTimeToLiveAttrPtr);
	}

	bool isEnabled = m_isEnabledAttrPtr.IsValid() && *m_isEnabledAttrPtr;
	SetEnabled(isEnabled);

	if (IsEnabled() && !m_providerCompPtr.IsValid()){
		SendWarningMessage(0, QStringLiteral("The assistant is enabled, but no provider is configured; requests are rejected."), "AssistantLogicController");
	}
}


void CAssistantLogicControllerComp::OnComponentDestroyed()
{
	SetEnabled(false);
	SetProvider(nullptr);
	SetTenantPolicy(nullptr);
	SetActionRegistry(nullptr);
	SetRepresentationRegistry(nullptr);
	SetAuditLog(nullptr);

	BaseClass::OnComponentDestroyed();
}


} // namespace imtassist


