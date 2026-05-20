// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <idoc/CStandardDocumentMetaInfo.h>


// Qt includes
#include <QtCore/QIODevice>
#include <QtCore/QDataStream>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace idoc
{


const istd::IChangeable::ChangeSet s_setMetaInfoChangeSet(idoc::IDocumentMetaInfo::CF_METAINFO, QObject::tr("Change document information"));


// public methods

// reimplemented (idoc::IDocumentMetaInfo)

CStandardDocumentMetaInfo::MetaInfoTypes CStandardDocumentMetaInfo::GetMetaInfoTypes(bool /*allowReadOnly*/) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QList<int> keys = m_infosMap.keys();

	return QSet<int>(keys.begin(), keys.end());
#else
	return m_infosMap.keys().toSet();
#endif
}


QVariant CStandardDocumentMetaInfo::GetMetaInfo(int metaInfoType) const
{
	static QVariant emptyValue;

	if (m_infosMap.contains(metaInfoType)){
		return m_infosMap[metaInfoType];
	}

	return emptyValue;
}


bool CStandardDocumentMetaInfo::SetMetaInfo(int metaInfoType, const QVariant& metaInfo)
{
	if (		!m_infosMap.contains(metaInfoType) ||
				(m_infosMap.contains(metaInfoType) &&
				(m_infosMap[metaInfoType].type() == metaInfo.type()) &&
				(m_infosMap[metaInfoType] != metaInfo)) ||
				(m_infosMap.contains(metaInfoType) &&
				(m_infosMap[metaInfoType].type() != metaInfo.type()))
				){
		istd::CChangeNotifier notifier(this, &s_setMetaInfoChangeSet);
		Q_UNUSED(notifier);

		m_infosMap[metaInfoType] = metaInfo;
	}

	return true;
}


void CStandardDocumentMetaInfo::RemoveMetaInfo(int metaInfoType)
{
	m_infosMap.remove(metaInfoType);
}


QByteArray CStandardDocumentMetaInfo::GetMetaInfoId(int metaInfoType) const
{
	static QByteArray emptyId;

	switch (metaInfoType){
	case MIT_TITLE:
		return ("Title");

	case MIT_AUTHOR:
		return ("Author");

	case MIT_CREATOR:
		return ("Creator");

	case MIT_DESCRIPTION:
		return ("Description");

	case MIT_CREATION_TIME:
		return ("CreationTime");

	case MIT_MODIFICATION_TIME:
		return ("ModificationTime");

	case MIT_CREATOR_VERSION:
		return ("CreatorVersion");

	case MIT_DOCUMENT_VERSION:
		return ("Version");

	case MIT_CONTENT_CHECKSUM:
		return ("Checksum");

	default:
		return emptyId;
	}
}


QString CStandardDocumentMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	static QString emptyName;

	switch (metaInfoType){
	case MIT_TITLE:
		return QObject::tr("Title");

	case MIT_AUTHOR:
		return QObject::tr("Author");

	case MIT_CREATOR:
		return QObject::tr("Creator");

	case MIT_DESCRIPTION:
		return QObject::tr("Description");

	case MIT_CREATION_TIME:
		return QObject::tr("Creation Time");

	case MIT_MODIFICATION_TIME:
		return QObject::tr("Modification Time");

	case MIT_CREATOR_VERSION:
		return QObject::tr("Creator Version");

	case MIT_DOCUMENT_VERSION:
		return QObject::tr("Version");

	case MIT_CONTENT_CHECKSUM:
		return QObject::tr("Checksum");

	default:
		return emptyName;
	}
}


QString CStandardDocumentMetaInfo::GetMetaInfoDescription(int metaInfoType) const
{
	static QString emptyName;

	switch (metaInfoType){
	case MIT_TITLE:
		return QObject::tr("Title of the document");

	case MIT_AUTHOR:
		return QObject::tr("Author of the document");

	case MIT_CREATOR:
		return QObject::tr("Creator of the document");

	case MIT_DESCRIPTION:
		return QObject::tr("Document description");

	case MIT_CREATION_TIME:
		return QObject::tr("Time of document's creation");

	case MIT_MODIFICATION_TIME:
		return QObject::tr("Time of last document's modification");

	case MIT_CREATOR_VERSION:
		return QObject::tr("Version of the creation tool");

	case MIT_DOCUMENT_VERSION:
		return QObject::tr("Version of the document");

	case MIT_CONTENT_CHECKSUM:
		return QObject::tr("Checksum of the document content");

	default:
		return emptyName;
	}
}


bool CStandardDocumentMetaInfo::IsMetaInfoWritable(int metaInfoType) const
{
	switch (metaInfoType){
		case MIT_CREATION_TIME:
		case MIT_MODIFICATION_TIME:
			return false;
		default:
			return true;
	}
}


// reimplemented (iser::ISerializable)

bool CStandardDocumentMetaInfo::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag metaInfoMapTag("MetaInfoMap", "Mapping between meta info and its type", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag metaInfoTag("MetaInfo", "Single meta information", iser::CArchiveTag::TT_GROUP, &metaInfoMapTag);
	static iser::CArchiveTag typeTag("Type", "Type of the meta information", iser::CArchiveTag::TT_LEAF, &metaInfoTag);
	static iser::CArchiveTag valueTag("Value", "Value of the meta information", iser::CArchiveTag::TT_LEAF, &metaInfoTag);
	static iser::CArchiveTag sizeTag("Size", "Size of the data block", iser::CArchiveTag::TT_LEAF, &metaInfoTag);

	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this, &GetAllChanges());
	Q_UNUSED(notifier);

	bool retVal = true;

	int metaInfosCount = m_infosMap.size();
	retVal = retVal && archive.BeginMultiTag(metaInfoMapTag, metaInfoTag, metaInfosCount);

	if (archive.IsStoring()){
		for (		MetaInfoMap::ConstIterator index = m_infosMap.constBegin();
					index != m_infosMap.constEnd();
					++index){
			retVal = retVal && archive.BeginTag(metaInfoTag);

			int type = index.key();
			retVal = retVal && archive.BeginTag(typeTag);
			retVal = retVal && archive.Process(type);
			retVal = retVal && archive.EndTag(typeTag);

			QByteArray variantData;
			QDataStream variantStream(&variantData, QIODevice::ReadWrite);
			QVariant value = index.value();
			variantStream << value;

			const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();	// TODO: Remove it when backward compatibility to older versions will not be no more important
			quint32 frameworkVersion = 0;
			if (versionInfo.GetVersionNumber(iser::IVersionInfo::AcfVersionId, frameworkVersion) && (frameworkVersion < 4001)){
				retVal = retVal && archive.BeginTag(valueTag);
				retVal = retVal && archive.Process(variantData);
				retVal = retVal && archive.EndTag(valueTag);
			}
			else{
				int dataSize = variantData.size();
				retVal = retVal && archive.BeginTag(sizeTag);
				retVal = retVal && archive.Process(dataSize);
				retVal = retVal && archive.EndTag(sizeTag);

				retVal = retVal && archive.BeginTag(valueTag);
				retVal = retVal && archive.ProcessData(variantData.data(), dataSize);
				retVal = retVal && archive.EndTag(valueTag);
			}

			retVal = retVal && archive.EndTag(metaInfoTag);
		}
	}
	else{
		m_infosMap.clear();

		for (int itemIndex = 0; itemIndex < metaInfosCount; ++itemIndex){
			retVal = retVal && archive.BeginTag(metaInfoTag);

			int type;
			retVal = retVal && archive.BeginTag(typeTag);
			retVal = retVal && archive.Process(type);
			retVal = retVal && archive.EndTag(typeTag);

			QByteArray variantData;

			const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();	// TODO: Remove it when backward compatibility to older versions will not be no more important
			quint32 frameworkVersion = 0;
			if (versionInfo.GetVersionNumber(iser::IVersionInfo::AcfVersionId, frameworkVersion) && (frameworkVersion < 4001)){
				retVal = retVal && archive.BeginTag(valueTag);
				retVal = retVal && archive.Process(variantData);
				retVal = retVal && archive.EndTag(valueTag);
			}
			else{
				int dataSize = 0;
				retVal = retVal && archive.BeginTag(sizeTag);
				retVal = retVal && archive.Process(dataSize);
				retVal = retVal && archive.EndTag(sizeTag);

				variantData.resize(dataSize);
				retVal = retVal && archive.BeginTag(valueTag);
				retVal = retVal && archive.ProcessData(variantData.data(), dataSize);
				retVal = retVal && archive.EndTag(valueTag);
			}

			QVariant variantValue;
			QDataStream variantStream(variantData);
			variantStream >> variantValue;

			if (retVal){
				m_infosMap[type] = variantValue;
			}

			retVal = retVal && archive.EndTag(metaInfoTag);
		}
	}

	retVal = retVal && archive.EndTag(metaInfoMapTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CStandardDocumentMetaInfo::IsEqual(const IChangeable& object) const
{
	const CStandardDocumentMetaInfo* infoPtr = dynamic_cast<const CStandardDocumentMetaInfo*>(&object);
	if (infoPtr != NULL){
		return (m_infosMap == infoPtr->m_infosMap);
	}

	return false;
}


bool CStandardDocumentMetaInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode = CM_WITHOUT_REFS*/)
{
	const CStandardDocumentMetaInfo* infoPtr = dynamic_cast<const CStandardDocumentMetaInfo*>(&object);
	if (infoPtr != NULL){
		istd::CChangeNotifier notifier(this, &s_setMetaInfoChangeSet);

		m_infosMap = infoPtr->m_infosMap;
		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CStandardDocumentMetaInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CStandardDocumentMetaInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return istd::IChangeableUniquePtr();
}


bool CStandardDocumentMetaInfo::ResetData(CompatibilityMode /*mode*/)
{
	if (!m_infosMap.isEmpty()){
		istd::CChangeNotifier notifier(this, &s_setMetaInfoChangeSet);

		m_infosMap.clear();
	}

	return true;
}


} // namespace idoc


