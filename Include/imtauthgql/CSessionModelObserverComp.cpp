#include <imtauthgql/CSessionModelObserverComp.h>


// Qt includes
#include <QtCore/QtDebug>

// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>


namespace imtauthgql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSessionModelObserverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	QByteArray contextId = GetComponentContext()->GetContextId();

	qDebug() << "OnComponentCreated" << contextId << this;

	if (m_modelCompPtr.IsValid()){
		bool ok = m_modelCompPtr->AttachObserver(this);

		qDebug() << "AttachObserver" << ok;
	}
}


void CSessionModelObserverComp::OnComponentDestroyed()
{
	QByteArray contextId = GetComponentContext()->GetContextId();

	qDebug() << "OnComponentDestroyed" << contextId;

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

		if (accessToken.isEmpty()){
			accessToken = changeSet.GetChangeInfo("token").toByteArray();
		}

		if (!accessToken.isEmpty()){
			sessionChangeNotifierCompPtr->OnSessionModelChanged(changeSet, accessToken);
		}
	}
}


} // namespace imtauthgql


