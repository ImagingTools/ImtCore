// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CUrlConnectionLinkParam.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtservice
{


// public methods

CUrlConnectionLinkParam::CUrlConnectionLinkParam()
{
	RegisterProtocol(imtcom::IServerConnectionInterface::PT_HTTP);
	RegisterProtocol(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
}


void CUrlConnectionLinkParam::SetDependantServiceConnectionId(const QByteArray& connectionId)
{
	if (m_dependantServiceConnectionId != connectionId){
		istd::CChangeNotifier notifier(this);
		
		m_dependantServiceConnectionId = connectionId;
	}
}


// reimplemented (imtservice::IServiceConnectionLinkParam)

QByteArray CUrlConnectionLinkParam::GetDependantServiceConnectionId() const
{
	return m_dependantServiceConnectionId;
}


// reimplemented (iser::ISerializable)

bool CUrlConnectionLinkParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	iser::CArchiveTag dependantServiceConnectionIdTag("DependantServiceConnectionId", "Dependant service Connection-Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(dependantServiceConnectionIdTag);
	retVal = retVal && archive.Process(m_dependantServiceConnectionId);
	retVal = retVal && archive.EndTag(dependantServiceConnectionIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CUrlConnectionLinkParam::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CUrlConnectionLinkParam* sourcePtr = dynamic_cast<const CUrlConnectionLinkParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);
		
		if (!BaseClass::CopyFrom(object, mode)){
			return false;
		}
		
		m_dependantServiceConnectionId = sourcePtr->m_dependantServiceConnectionId;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CUrlConnectionLinkParam::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUrlConnectionLinkParam);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUrlConnectionLinkParam::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
	
	bool retVal = BaseClass::ResetData(mode);
	
	m_dependantServiceConnectionId.clear();

	return retVal;
}


} // namespace agentinodata


