// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CObjectTypeIdFilter.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtcol
{


// public methods

// reimplemented (imtcol::IObjectTypeIdFilter)

QByteArray CObjectTypeIdFilter::GetObjectTypeId() const
{
	return m_typeId;
}


void CObjectTypeIdFilter::SetObjectTypeId(const QByteArray& typeId)
{
	if (m_typeId != typeId){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

		m_typeId = typeId;
	}
}


// reimplemented (iser::ISerializable)

bool CObjectTypeIdFilter::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &istd::IChangeable::GetAllChanges());

	bool retVal = true;

	iser::CArchiveTag typeIdTag("ObjectTypeId", "Object Type ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CObjectTypeIdFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CObjectTypeIdFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CObjectTypeIdFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

		m_typeId = sourcePtr->m_typeId;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CObjectTypeIdFilter::CloneMe(CompatibilityMode /*mode*/) const
{
	istd::IChangeableUniquePtr clonePtr(new CObjectTypeIdFilter());
	if (clonePtr->CopyFrom(*this)){
		return clonePtr;
	}

	return nullptr;
}


bool CObjectTypeIdFilter::ResetData(CompatibilityMode /*mode*/)
{
	m_typeId.clear();

	return true;
}


} // namespace imtcol


