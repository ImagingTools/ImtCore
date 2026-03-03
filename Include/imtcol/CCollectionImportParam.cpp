// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CCollectionImportParam.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtcol
{


// public methods

// reimplemented (imtcol::ICollectionImportParam)

QByteArray CCollectionImportParam::GetCollectionId() const
{
	return m_collectionId;
}


bool CCollectionImportParam::SetCollectionId(const QByteArray& collectionId)
{
	if (m_collectionId != collectionId){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

		m_collectionId = collectionId;
		return true;
	}
	return false;
}


int CCollectionImportParam::GetFileCount() const
{
	return m_fileImportInfos.size();
}


ICollectionImportParam::FileImportInfo* CCollectionImportParam::GetFileImportInfo(int index) const
{
	if (index < 0 || index >= m_fileImportInfos.size()){
		return nullptr;
	}
	return &m_fileImportInfos[index];
}


bool CCollectionImportParam::InsertFileImportInfo(const FileImportInfo& info, int index)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

	if (index < 0){
		m_fileImportInfos.insert(0, info);
	}
	else if (index >= m_fileImportInfos.size()){
		m_fileImportInfos.append(info);
	}
	else{
		m_fileImportInfos.insert(index, info);
	}

	return true;
}


bool CCollectionImportParam::RemoveFileImportInfo(int index)
{
	if (index < 0 || index >= m_fileImportInfos.size()){
		return false;
	}

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

	m_fileImportInfos.remove(index);
	return true;
}


// reimplemented (iser::ISerializable)

bool CCollectionImportParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &istd::IChangeable::GetAllChanges());

	bool retVal = true;

	iser::CArchiveTag collectionIdTag("CollectionId", "Collection ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(collectionIdTag);
	retVal = retVal && archive.Process(m_collectionId);
	retVal = retVal && archive.EndTag(collectionIdTag);

	// Serialize file import infos
	iser::CArchiveTag fileImportInfosTag("FileImportInfos", "File Import Infos", iser::CArchiveTag::TT_NODE);
	retVal = retVal && archive.BeginTag(fileImportInfosTag);

	int count = m_fileImportInfos.size();
	iser::CArchiveTag countTag("Count", "Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(countTag);
	retVal = retVal && archive.Process(count);
	retVal = retVal && archive.EndTag(countTag);

	if (!archive.IsStoring()){
		m_fileImportInfos.resize(count);
	}

	for (int i = 0; i < count && retVal; ++i){
		iser::CArchiveTag itemTag("FileImportInfo", "File Import Info", iser::CArchiveTag::TT_NODE);
		retVal = retVal && archive.BeginTag(itemTag);

		iser::CArchiveTag filePathTag("FilePath", "File Path", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(filePathTag);
		retVal = retVal && archive.Process(m_fileImportInfos[i].filePath);
		retVal = retVal && archive.EndTag(filePathTag);

		iser::CArchiveTag objectTypeIdTag("ObjectTypeId", "Object Type ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(objectTypeIdTag);
		retVal = retVal && archive.Process(m_fileImportInfos[i].objectTypeId);
		retVal = retVal && archive.EndTag(objectTypeIdTag);

		iser::CArchiveTag proposedIdTag("ProposedId", "Proposed ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(proposedIdTag);
		retVal = retVal && archive.Process(m_fileImportInfos[i].proposedId);
		retVal = retVal && archive.EndTag(proposedIdTag);

		retVal = retVal && archive.EndTag(itemTag);
	}

	retVal = retVal && archive.EndTag(fileImportInfosTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCollectionImportParam::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CCollectionImportParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CCollectionImportParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

		m_collectionId = sourcePtr->m_collectionId;
		m_fileImportInfos = sourcePtr->m_fileImportInfos;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CCollectionImportParam::CloneMe(CompatibilityMode /*mode*/) const
{
	istd::IChangeableUniquePtr clonePtr(new CCollectionImportParam());
	if (clonePtr->CopyFrom(*this)){
		return clonePtr;
	}

	return nullptr;
}


bool CCollectionImportParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_collectionId.clear();
	m_fileImportInfos.clear();

	return true;
}


} // namespace imtcol


