// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDependentTableMetaInfoControllerComp.h>


namespace imtdb
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CDependentTableMetaInfoControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_dependentMetaInfoControllerCompPtr.IsValid()){
		return;
	}

	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE) &&
		!changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		return;
	}

	if (changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		QVariant changeInfo = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED);
		if (changeInfo.isValid()){
			imtbase::ICollectionInfo::MultiElementNotifierInfo info = changeInfo.value<imtbase::ICollectionInfo::MultiElementNotifierInfo>();

			imtdb::IDependentMetaInfoController::MetaFieldCleanupPlan metaFieldCleanupPlan;
			metaFieldCleanupPlan.objectIds = info.elementIds;
			metaFieldCleanupPlan.dependentKey = *m_metaInfoIdAttrPtr;

			for (int i = 0; i < m_metaInfoNameAttrPtr.GetCount(); i++){
				metaFieldCleanupPlan.metaInfoIds << m_metaInfoNameAttrPtr[i];
			}

			if (!m_dependentMetaInfoControllerCompPtr->ClearDependentMetaInfo(metaFieldCleanupPlan)){
				SendErrorMessage(0, QStringLiteral("Unable to clear dependent meta info"), "CDependentTableMetaInfoControllerComp");
			}

			return;
		}
	}

	QByteArray elementId;
	if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_DESCRIPTION_CHANGED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_RENAMED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_STATE).toByteArray();
	}
	else if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
		elementId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
	}

	if (elementId.isEmpty()){
		return;
	}

	imtdb::IDependentMetaInfoController::DependentMetaInfo dependentMetaInfo;

	dependentMetaInfo.objectId = elementId;
	dependentMetaInfo.dependentKey = *m_metaInfoIdAttrPtr;
	if (m_objectCollectionCompPtr.IsValid()) {
		const auto metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(elementId);
		if (metaInfo.IsValid()) {
			const auto value = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION);
			bool validRevision = false;
			const qlonglong revision = value.toLongLong(&validRevision);
			if (validRevision && revision >= 0) {
				dependentMetaInfo.sourceRevisionGuard = IDependentMetaInfoController::DependentMetaInfo::SourceRevisionGuard{
					*m_dependentTableNameAttrPtr, static_cast<quint64>(revision)};
			}
		}
	}

	bool isDocumentSource = m_isDocumentSourceAttrPtr.IsValid() ? *m_isDocumentSourceAttrPtr : true;
	bool metaInfoValuesAreJson =
		m_metaInfoValuesAreJsonAttrPtr.IsValid() ? *m_metaInfoValuesAreJsonAttrPtr : false;

	for (int i = 0; i < m_metaInfoNameAttrPtr.GetCount(); i++){
		QString metaInfoName = m_metaInfoNameAttrPtr[i];
		QString dependentMetaInfoName = m_dependentMetaInfoNameAttrPtr[i];

		dependentMetaInfo.metaInfoIds << metaInfoName;
		const QString valueOperator = metaInfoValuesAreJson ? QStringLiteral("->") : QStringLiteral("->>");
		QString selectValue = QStringLiteral(
			"(SELECT \"%1\"%2'%3' FROM \"%4\" WHERE \"State\" = 'Active' AND \"DocumentId\" = '%5' LIMIT 1)")
						.arg(isDocumentSource ? QStringLiteral("Document") : QStringLiteral("DataMetaInfo"),
							 valueOperator,
							 dependentMetaInfoName,
							 *m_dependentTableNameAttrPtr,
							 elementId);

		dependentMetaInfo.metaInfoValues << selectValue;
	}

	if (!m_dependentMetaInfoControllerCompPtr->UpdateDependentMetaInfo(dependentMetaInfo)){
		SendErrorMessage(0, QStringLiteral("Unable to update dependent meta info"), "CDependentTableMetaInfoControllerComp");
	}
}


// reimplemented (icomp::CComponentBase)

void CDependentTableMetaInfoControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CDependentTableMetaInfoControllerComp::OnComponentDestroyed()
{
	EnsureModelDetached();
	
	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


