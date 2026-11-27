// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantServiceComp.h>


namespace imtassist
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CAssistantServiceComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetProvider(m_providerCompPtr.GetPtr());
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

	if (isEnabled && !m_providerCompPtr.IsValid()){
		SendWarningMessage(0, QStringLiteral("The assistant is enabled, but no provider is configured; requests are rejected."), "AssistantService");
	}
}


void CAssistantServiceComp::OnComponentDestroyed()
{
	SetEnabled(false);
	SetProvider(nullptr);
	SetActionRegistry(nullptr);
	SetRepresentationRegistry(nullptr);
	SetAuditLog(nullptr);

	BaseClass::OnComponentDestroyed();
}


} // namespace imtassist


