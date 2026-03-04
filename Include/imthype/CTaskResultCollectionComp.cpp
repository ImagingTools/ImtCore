// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskResultCollectionComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imthype
{


// reimplemented (ITaskResultCollection)

void CTaskResultCollectionComp::SetArtifactType(const QByteArray& artifactId, ArtifactType artifactType)
{
	bool wasChanged = false;

	if (m_resultInfoMap.contains(artifactId)){
		if (m_resultInfoMap[artifactId].artifactType != artifactType){
			wasChanged = true;
		}
	}
	else {
		wasChanged = true;
	}

	istd::CChangeNotifier changeNotifier(wasChanged ? this : nullptr);

	m_resultInfoMap[artifactId].artifactType = artifactType;
}


void CTaskResultCollectionComp::SetArtifactStatus(const QByteArray& artifactId, istd::IInformationProvider::InformationCategory status)
{
	bool wasChanged = false;

	if (m_resultInfoMap.contains(artifactId)){
		if (m_resultInfoMap[artifactId].status != status){
			wasChanged = true;
		}
	}
	else {
		wasChanged = true;
	}

	istd::CChangeNotifier changeNotifier(wasChanged ? this : nullptr);

	m_resultInfoMap[artifactId].status = status;
}


QByteArray CTaskResultCollectionComp::InsertNewResult(
			ArtifactType artifactType,
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IInformationProvider* statusProviderPtr,
			const istd::IChangeable* defaultValuePtr)
{
	istd::CChangeGroup changeGroup(this);

	QByteArray retVal = BaseClass::InsertNewObject(typeId, name, description, defaultValuePtr);
	if (!retVal.isEmpty()){
		m_resultInfoMap[retVal].artifactType = artifactType;
		m_resultInfoMap[retVal].dataTypeId = typeId;

		if (statusProviderPtr != nullptr){
			m_resultInfoMap[retVal].status = statusProviderPtr->GetInformationCategory();
			m_resultInfoMap[retVal].statusText = statusProviderPtr->GetInformationDescription();
			m_resultInfoMap[retVal].statusTimestamp = statusProviderPtr->GetInformationTimeStamp();
		}
	}

	return retVal;
}


// reimplemented (ITaskMetaInfo)

ITaskMetaInfo::ArtifactType CTaskResultCollectionComp::GetArtifactType(const QByteArray& artifactId) const
{
	if (m_resultInfoMap.contains(artifactId)){
		return m_resultInfoMap[artifactId].artifactType;
	}

	return AT_NONE;
}


QByteArray CTaskResultCollectionComp::GetArtifactDataTypeId(const QByteArray& artifactId) const
{
	if (m_resultInfoMap.contains(artifactId)){
		return m_resultInfoMap[artifactId].dataTypeId;
	}

	return QByteArray();
}


// reimplemented (imtbase::IMultiStatusProvider)

const imtbase::ICollectionInfo& CTaskResultCollectionComp::GetStatusList() const
{
	return *this;
}


const istd::IInformationProvider* CTaskResultCollectionComp::GetStatus(const QByteArray& statusId) const
{
	ResultInfoMap::ConstIterator statusIter = m_resultInfoMap.constFind(statusId);
	if (statusIter != m_resultInfoMap.constEnd()){
		return &statusIter.value();
	}

	return nullptr;
}


// reimplemented (imtbase::IObjectCollection)

imtbase::ICollectionInfo::Id CTaskResultCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const Id& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray retVal = BaseClass::InsertNewObject(typeId, name, description, defaultValuePtr, proposedObjectId, dataMetaInfoPtr, collectionItemMetaInfoPtr, operationContextPtr);
	if (!retVal.isEmpty()){
		m_resultInfoMap[retVal] = ResultInfo();
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTaskResultCollectionComp::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const ITaskResultCollection* sourcePtr = dynamic_cast<const ITaskResultCollection*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_resultInfoMap.clear();

		if (BaseClass::CopyFrom(object, mode)){
			Ids sourceElementIds = sourcePtr->GetElementIds();

			for (const QByteArray& elementId : sourceElementIds){
				ArtifactType artifactType = sourcePtr->GetArtifactType(elementId);
				QByteArray artifactDataTypeId = sourcePtr->GetArtifactDataTypeId(elementId);
				
				m_resultInfoMap[elementId].artifactType = artifactType;
				m_resultInfoMap[elementId].dataTypeId = artifactDataTypeId;

				const istd::IInformationProvider* statusPtr = sourcePtr->GetStatus(elementId);
				if (statusPtr != nullptr){
					m_resultInfoMap[elementId].status = statusPtr->GetInformationCategory();
					m_resultInfoMap[elementId].statusText = statusPtr->GetInformationDescription();
					m_resultInfoMap[elementId].statusTimestamp = statusPtr->GetInformationTimeStamp();
				}
			}

			return true;
		}
	}
	
	return false;
}


// public methods of the embedded class ResultInfo

// reimplemented (istd::IInformationProvider)

QDateTime CTaskResultCollectionComp::ResultInfo::GetInformationTimeStamp() const
{
	return statusTimestamp;
}


istd::IInformationProvider::InformationCategory CTaskResultCollectionComp::ResultInfo::GetInformationCategory() const
{
	return status;
}


int CTaskResultCollectionComp::ResultInfo::GetInformationId() const
{
	return 0;
}


QString CTaskResultCollectionComp::ResultInfo::GetInformationDescription() const
{
	return statusText;
}


QString CTaskResultCollectionComp::ResultInfo::GetInformationSource() const
{
	return QString();
}


int CTaskResultCollectionComp::ResultInfo::GetInformationFlags() const
{
	return 0;
}


} // namespace imthype


