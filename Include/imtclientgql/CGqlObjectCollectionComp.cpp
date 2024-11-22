#include <imtclientgql/CGqlObjectCollectionComp.h>


namespace imtclientgql
{


// public methods

// reimplemented (IObjectCollectionInfo)

QByteArray CGqlObjectCollectionComp::GetObjectTypeId(const imtbase::ICollectionInfo::Id& objectId) const
{
	QByteArrayList ids;
	if (m_typeIdsAttrPtr.IsValid()) {
		for (int i = 0; i < m_typeIdsAttrPtr.GetCount(); i++){
			QByteArray id = m_typeIdsAttrPtr[i];
			if (!id.isEmpty()) {
				if (!ids.contains(id)) {
					ids.append(id);
				}
			}
		}
	}

	if (ids.count() == 1){
		return ids.at(0);
	}

	return QByteArray();
}


// reimplemented (gmgaws::ISubscriptionClient)

void CGqlObjectCollectionComp::OnResponseReceived(const QByteArray& /*subscriptionId*/, const QByteArray& /*subscriptionData*/)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
	istd::CChangeNotifier notifier(this, &changeSet);
}


void CGqlObjectCollectionComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	for (int i = 0; i < m_typeIdsAttrPtr.GetCount(); i++){
		if (m_typeIdsAttrPtr[i] == typeId){
			icomp::IComponent* compPtr = m_objectFactoriesCompPtr.CreateComponent(i);
			return DataPtr(DataPtr::RootObjectPtr(compPtr), [compPtr, this](){
				return m_objectFactoriesCompPtr.ExtractInterface(compPtr);
			});
		}
	}

	return DataPtr();
}


// reimplemented (icomp::CComponentBase)

void CGqlObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_clientCompPtr.IsValid()){
		m_gqlClientPtr = m_clientCompPtr.GetPtr();
	}

	if (m_delegateCompPtr.IsValid()){
		m_gqlObjectCollectionDelegatePtr = m_delegateCompPtr.GetPtr();
	}

	if (m_metaInfoCreatorCompPtr.IsValid()){
		m_metaInfoCreatorPtr = m_metaInfoCreatorCompPtr.GetPtr();
	}
}


} // namespace imtclientgql


