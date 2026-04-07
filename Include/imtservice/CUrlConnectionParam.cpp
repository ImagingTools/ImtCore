// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CUrlConnectionParam.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtservice
{


// public methods

CUrlConnectionParam::CUrlConnectionParam()
{
	RegisterProtocol(imtcom::IServerConnectionInterface::PT_HTTP);
	RegisterProtocol(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
}


void CUrlConnectionParam::AddExternConnection(const imtservice::IServiceConnectionParam::IncomingConnectionParam& externConnection)
{
	const QByteArray uuid = externConnection.GetObjectUuid();
	bool alreadyExists = false;
	for (const IncomingConnectionParam& existing : m_externConnectionList){
		if (existing.GetObjectUuid() == uuid){
			alreadyExists = true;
			break;
		}
	}

	if (!alreadyExists){
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

	int objectCount = imtbase::narrow_cast<int>(m_externConnectionList.count());
	if (!archive.IsStoring()){
		objectCount = 0;

		m_externConnectionList.clear();
	}

	iser::CArchiveTag objectListTag("ExternConnections", "Extern connections", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag objectTag("ExternConnection", "ExternConnection", iser::CArchiveTag::TT_GROUP, &objectListTag);

	retVal = retVal && archive.BeginMultiTag(objectListTag, objectTag, objectCount);
	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(objectTag);

		IncomingConnectionParam elementInfo;
		if (archive.IsStoring()){
			elementInfo = m_externConnectionList[i];
		}

		retVal = retVal && elementInfo.Serialize(archive);

		retVal = retVal && archive.EndTag(objectTag);

		if (retVal && !archive.IsStoring()){
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
			newParam.CopyFrom(connectionParam);

			m_externConnectionList << newParam;
		}

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CUrlConnectionParam::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUrlConnectionParam);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
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


