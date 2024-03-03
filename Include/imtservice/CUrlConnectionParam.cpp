#include <imtservice/CUrlConnectionParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtservice
{


// public methods

CUrlConnectionParam::CUrlConnectionParam()
	: m_connectionType(CT_INPUT),
	m_connectionStatus(CS_OK)
{

}


CUrlConnectionParam::CUrlConnectionParam(const QByteArray& serviceTypeName, const QByteArray &usageId, ConnectionType connectionType, QUrl url)
	: m_connectionType(connectionType),
	m_serviceTypeName(serviceTypeName),
	m_usageId(usageId),
	m_connectionStatus(CS_OK),
	m_defaultUrl(url)
{
	m_url = url;
}


void CUrlConnectionParam::AddExternConnection(imtservice::IServiceConnectionParam::IncomingConnectionParam externConnection)
{
	if (!m_externConnectionList.contains(externConnection)){
		istd::CChangeNotifier changeNotifier(this);

		m_externConnectionList << externConnection;
	}
}


// reimplemented (imtservice::IServiceConnectionParam)

imtservice::IServiceConnectionParam::ConnectionType CUrlConnectionParam::GetConnectionType() const
{
	return m_connectionType;
}


QByteArray CUrlConnectionParam::GetServiceTypeName() const
{
	return m_serviceTypeName;
}


QByteArray CUrlConnectionParam::GetUsageId() const
{
	return m_usageId;
}


QUrl CUrlConnectionParam::GetDefaultUrl() const
{
	return m_defaultUrl;
}


QList<imtservice::IServiceConnectionParam::IncomingConnectionParam> CUrlConnectionParam::GetIncomingConnections() const
{
	return m_externConnectionList;
}


// reimplemented (imtservice::IConnectionStatus)
imtservice::IConnectionStatus::ConnectionStatus CUrlConnectionParam::GetConnectionStatus() const
{
	return m_connectionStatus;
}


// reimplemented (iser::ISerializable)

bool CUrlConnectionParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	iser::CArchiveTag serviceTypeNameTag("ServiceTypeName", "Service TypeName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceTypeNameTag);
	retVal = retVal && archive.Process(m_serviceTypeName);
	retVal = retVal && archive.EndTag(serviceTypeNameTag);

	iser::CArchiveTag serviceUsageIdTag("UsageId", "UsageId", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceUsageIdTag);
	retVal = retVal && archive.Process(m_usageId);
	retVal = retVal && archive.EndTag(serviceUsageIdTag);

	iser::CArchiveTag connectionTypeTag("ConnectionType", "Connection Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionTypeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				 IServiceConnectionInfo::ConnectionType,
				 IServiceConnectionInfo::ToString,
				 IServiceConnectionInfo::FromString>(archive, m_connectionType);
	retVal = retVal && archive.EndTag(connectionTypeTag);

	iser::CArchiveTag connectionStatusTag("ConnectionStatus", "Connection Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionStatusTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				 IConnectionStatus::ConnectionStatus,
				 IConnectionStatus::ToString,
				 IConnectionStatus::FromString>(archive, m_connectionStatus);
	retVal = retVal && archive.EndTag(connectionStatusTag);

	iser::CArchiveTag defaultUrlTag("DefaultUrl", "DefaultUrl", iser::CArchiveTag::TT_LEAF);
	QString defaultUrlStr = m_defaultUrl.toString();

	retVal = retVal && archive.BeginTag(defaultUrlTag);
	retVal = retVal && archive.Process(defaultUrlStr);
	retVal = retVal && archive.EndTag(defaultUrlTag);

	if (retVal && !archive.IsStoring()){
		m_defaultUrl = QUrl(defaultUrlStr);
	}

	int objectCount = m_externConnectionList.count();
	if (!archive.IsStoring()){
		objectCount = 0;

		m_externConnectionList.clear();
	}

	iser::CArchiveTag objectListTag("ExternConnections", "Extern connections", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag objectTag("Connection", "Connection", iser::CArchiveTag::TT_GROUP, &objectListTag);

	retVal = retVal && archive.BeginMultiTag(objectListTag, objectTag, objectCount);
	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(objectTag);

		IncomingConnectionParam elementInfo;
		if (archive.IsStoring()){
			elementInfo = m_externConnectionList[i];
		}

		iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(idTag);
		retVal = retVal && archive.Process(elementInfo.id);
		retVal = retVal && archive.EndTag(idTag);

		iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(nameTag);
		retVal = retVal && archive.Process(elementInfo.name);
		retVal = retVal && archive.EndTag(nameTag);

		iser::CArchiveTag urlTag("Url", "URL", iser::CArchiveTag::TT_LEAF, &objectTag);

		QString urlStr = elementInfo.url.toString();

		retVal = retVal && archive.BeginTag(urlTag);
		retVal = retVal && archive.Process(urlStr);
		retVal = retVal && archive.EndTag(urlTag);

		iser::CArchiveTag descriptionTag("Description", "Connection description", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(descriptionTag);
		retVal = retVal && archive.Process(elementInfo.description);
		retVal = retVal && archive.EndTag(descriptionTag);

		retVal = retVal && archive.EndTag(objectTag);

		if (retVal && !archive.IsStoring()){
			elementInfo.url = QUrl(urlStr);
			m_externConnectionList.append(elementInfo);
		}
	}

	retVal = retVal && archive.EndTag(objectListTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CUrlConnectionParam::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CUrlConnectionParam* sourcePtr = dynamic_cast<const CUrlConnectionParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object, mode);

		m_serviceTypeName = sourcePtr->m_serviceTypeName;
		m_usageId = sourcePtr->m_usageId;
		m_connectionType = sourcePtr->m_connectionType;
		m_connectionStatus = sourcePtr->m_connectionStatus;
		m_defaultUrl = sourcePtr->m_defaultUrl;

		m_externConnectionList.clear();

		for (const IncomingConnectionParam& connectionParam : sourcePtr->m_externConnectionList){
			IncomingConnectionParam newParam;
			newParam.description = connectionParam.description;
			newParam.url = connectionParam.url;
			newParam.id = connectionParam.id;

			m_externConnectionList << newParam;
		}

		return true;
	}

	return false;
}


istd::IChangeable* CUrlConnectionParam::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUrlConnectionParam> clonePtr(new CUrlConnectionParam);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUrlConnectionParam::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_serviceTypeName.clear();
	m_usageId.clear();
	m_externConnectionList.clear();
	m_connectionType = CT_INPUT;
	m_connectionStatus = CS_OK;
	m_defaultUrl.clear();

	return true;
}


} // namespace agentinodata


