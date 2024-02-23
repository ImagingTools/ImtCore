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
	:m_connectionType(CT_INPUT)
{
}


CUrlConnectionParam::CUrlConnectionParam(const QByteArray& serviceName, ConnectionType connectionType, QUrl url)
{
	m_serviceName = serviceName;
	m_connectionType = connectionType;
	SetUrl(url);
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


QByteArray CUrlConnectionParam::GetServiceName() const
{
	return m_serviceName;
}


QList<imtservice::IServiceConnectionParam::IncomingConnectionParam> CUrlConnectionParam::GetIncomingConnections() const
{
	return m_externConnectionList;
}


// reimplemented (iser::ISerializable)

bool CUrlConnectionParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	iser::CArchiveTag serviceNameTag("ServiceName", "Service Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceNameTag);
	retVal = retVal && archive.Process(m_serviceName);
	retVal = retVal && archive.EndTag(serviceNameTag);

	qDebug() << "m_serviceName" << m_serviceName;

	iser::CArchiveTag connectionTypeTag("ConnectionType", "Connection Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(ConnectionType, archive, m_connectionType);
	retVal = retVal && archive.EndTag(connectionTypeTag);

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

		iser::CArchiveTag urlTag("Url", "URL", iser::CArchiveTag::TT_LEAF, &objectTag);

		QString urlStr = elementInfo.url.toString();

		qDebug() << "urlStr" << urlStr;

		bool retVal = archive.BeginTag(urlTag);
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

		m_serviceName = sourcePtr->m_serviceName;
		m_connectionType = sourcePtr->m_connectionType;

		m_externConnectionList.clear();

		for (const IncomingConnectionParam& connectionParam : sourcePtr->m_externConnectionList){
			IncomingConnectionParam newParam;
			newParam.description = connectionParam.description;
			newParam.url = connectionParam.url;

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

	m_serviceName.clear();
	m_externConnectionList.clear();
	m_connectionType = CT_INPUT;

	return true;
}


} // namespace agentinodata


