// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CIdentifiable.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>


namespace imtbase
{


// reimplemented (IIdentifiable)

QByteArray CIdentifiable::GetObjectUuid() const
{
	return m_identifier;
}


bool CIdentifiable::SetObjectUuid(const QByteArray& identifier)
{
	Q_ASSERT(!identifier.isEmpty());
	if (identifier.isEmpty()){
		return false;
	}

	if (!IsIdentifierAccepted(identifier)){
		return false;
	}

	if (!IsMutable()){
		return false;
	}

	if (m_identifier != identifier){
		istd::CChangeNotifier changeNotifier(this);
		m_identifier = identifier;
	}

	return true;
}


// reimplemented (iser::ISerializable)

bool CIdentifiable::Serialize(iser::IArchive& archive)
{
	iser::CArchiveTag identifierTag("Uuid", "Unique identifier of the object", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(identifierTag);
	retVal = retVal && archive.Process(m_identifier);
	retVal = retVal && archive.EndTag(identifierTag);

	return retVal;
}


// reimplemented (IChangeable)

int CIdentifiable::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CIdentifiable::CopyFrom(const istd::IChangeable& object,  CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CIdentifiable* sourcePtr = dynamic_cast<const CIdentifiable*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_identifier = sourcePtr->m_identifier;

		return true;
	}

	return false;
}


bool CIdentifiable::IsEqual(const istd::IChangeable& object) const
{
	const CIdentifiable* sourcePtr = dynamic_cast<const CIdentifiable*>(&object);
	if (sourcePtr != nullptr){
		return (m_identifier == sourcePtr->m_identifier);
	}

	return false;
}


istd::IChangeableUniquePtr CIdentifiable::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIdentifiable());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIdentifiable::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_identifier.clear();

	return true;
}


// protected methods

bool CIdentifiable::IsIdentifierAccepted(const QByteArray& identifier) const
{
	return !identifier.isEmpty();
}


bool CIdentifiable::IsMutable() const
{
	return true;
}


} // namespace imtbase


