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

void CUrlConnectionParam::AddExternConnection(imtservice::IServiceConnectionParam::IncomingConnectionParam externConnection)
{
	if (!m_externConnectionList.contains(externConnection)){
		istd::CChangeNotifier changeNotifier(this);

		m_externConnectionList << externConnection;
	}
}


// reimplemented (imtservice::IServiceConnectionParam)

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
		
		if (!BaseClass::CopyFrom(object, mode)){
			return false;
		}

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
	
	bool retVal = BaseClass::ResetData(mode);
	m_externConnectionList.clear();
	return retVal;
}


} // namespace agentinodata


