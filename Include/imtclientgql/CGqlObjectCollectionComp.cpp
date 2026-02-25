// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlObjectCollectionComp.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtclientgql::ISubscriptionClient)

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
			return m_objectFactoriesCompPtr.CreateInstance(i);
		}
	}

	return DataPtr();
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CGqlObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typeInfos;
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

	for (int i = 0; i < m_typeIdsAttrPtr.GetCount(); i++){
		QByteArray typeId = m_typeIdsAttrPtr[i];
		QString typeName = m_typeNamesAttrPtr[i];
		if (!typeId.isEmpty()){
			m_typeInfos.InsertOption(typeName, typeId);

			if (i < m_metaInfoCreatorListCompPtr.GetCount()){
				imtbase::IMetaInfoCreator* metaInfoCreatorPtr = m_metaInfoCreatorListCompPtr[i];
				if (metaInfoCreatorPtr != nullptr){
					m_metaInfoCreatorMap[typeId] = metaInfoCreatorPtr;
				}
			}
		}
	}
}


} // namespace imtclientgql


