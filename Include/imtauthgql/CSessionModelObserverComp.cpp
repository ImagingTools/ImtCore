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

	if (m_modelCompPtr.IsValid()) {
		m_modelCompPtr->AttachObserver(this);
	}
}


void CSessionModelObserverComp::OnComponentDestroyed()
{
	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CSessionModelObserverComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (sessionChangeNotifierCompPtr.IsValid()){
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

		sessionChangeNotifierCompPtr->OnSessionModelChanged(changeSet, accessToken);
	}
}


} // namespace imtauthgql


