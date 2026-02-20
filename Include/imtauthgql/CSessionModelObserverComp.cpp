// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CSessionModelObserverComp.h>


// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>


namespace imtauthgql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSessionModelObserverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->AttachObserver(this);
	}
}


void CSessionModelObserverComp::OnComponentDestroyed()
{
	QByteArray contextId = GetComponentContext()->GetContextId();

	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CSessionModelObserverComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_sessionChangeNotifierCompPtr.IsValid()){
		QByteArray accessToken;
		imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(this);
		if (gqlRequestProviderPtr != nullptr){
			const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
			if (gqlRequestPtr != nullptr){
				const imtgql::IGqlContext* gqlContextPtr = gqlRequestPtr->GetRequestContext();
				if (gqlContextPtr != nullptr){
					accessToken = gqlContextPtr->GetToken();
				}
			}
		}

		if (accessToken.isEmpty()){
			accessToken = changeSet.GetChangeInfo("token").toByteArray();
		}

		if (!accessToken.isEmpty()){
			m_sessionChangeNotifierCompPtr->OnSessionModelChanged(changeSet, accessToken);
		}
	}
}


} // namespace imtauthgql


