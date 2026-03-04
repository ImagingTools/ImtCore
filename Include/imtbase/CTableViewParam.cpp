// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTableViewParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace imtbase
{


// public methods

bool CTableViewParam::SetHeaderInfo(QByteArray headerId, ITableViewParam::HeaderInfo headerInfo)
{
	istd::CChangeNotifier notifier(this);

	m_headerInfoMap[headerId] = headerInfo;

	return true;
}


// reimplemented (imtbase::ITableViewParam)

QByteArrayList CTableViewParam::GetHeaderIds() const
{
	return m_headerInfoMap.keys();
}


ITableViewParam::HeaderInfo CTableViewParam::GetHeaderInfo(const QByteArray& headerId) const
{
	return m_headerInfoMap.value(headerId);
}


// reimplemented (iser::ISerializable)

bool CTableViewParam::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	QByteArrayList keys = m_headerInfoMap.keys();
	int count = keys.size();

	if (!archive.IsStoring()){
		m_headerInfoMap.clear();
		keys.clear();
		count = 0;
	}

	iser::CArchiveTag infosTag("HeaderInfos", "HeaderInfos", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag infoTag("HeaderInfo", "HeaderInfo", iser::CArchiveTag::TT_GROUP, &infosTag);

	retVal = retVal && archive.BeginMultiTag(infosTag, infoTag, count);

	for (int index = 0; index < count; index++){
		retVal = retVal && archive.BeginTag(infoTag);

		QByteArray key;
		ITableViewParam::HeaderInfo value;

		if (archive.IsStoring()){
			key = keys[index];
			value = m_headerInfoMap[key];
		}

		iser::CArchiveTag keyTag("HeaderId", "HeaderId", iser::CArchiveTag::TT_LEAF, &infoTag);
		retVal = retVal && archive.BeginTag(keyTag);
		retVal = retVal && archive.Process(value.headerId);
		retVal = retVal && archive.EndTag(keyTag);

		iser::CArchiveTag visibleTag("Visible", "Visible", iser::CArchiveTag::TT_LEAF, &infoTag);
		retVal = retVal && archive.BeginTag(visibleTag);
		retVal = retVal && archive.Process(value.visible);
		retVal = retVal && archive.EndTag(visibleTag);

		iser::CArchiveTag sizeTag("Size", "Size", iser::CArchiveTag::TT_LEAF, &infoTag);
		retVal = retVal && archive.BeginTag(sizeTag);
		retVal = retVal && archive.Process(value.size);
		retVal = retVal && archive.EndTag(sizeTag);

		if (imtCoreVersion >= 11983){
			iser::CArchiveTag orderTag("Order", "Order", iser::CArchiveTag::TT_LEAF, &infoTag);
			retVal = retVal && archive.BeginTag(orderTag);
			retVal = retVal && archive.Process(value.order);
			retVal = retVal && archive.EndTag(orderTag);
		}

		if (!archive.IsStoring()){
			m_headerInfoMap[value.headerId] = value;
		}

		retVal = retVal && archive.EndTag(infoTag);
	}

	retVal = retVal && archive.EndTag(infosTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTableViewParam::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CTableViewParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTableViewParam* implPtr = dynamic_cast<const CTableViewParam*>(&object);
	if (implPtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_headerInfoMap = implPtr->m_headerInfoMap;

		return true;
	}

	return false;
}


bool CTableViewParam::IsEqual(const IChangeable& object) const
{
	const CTableViewParam* implPtr = dynamic_cast<const CTableViewParam*>(&object);
	if (implPtr != nullptr){
		return (m_headerInfoMap == implPtr->m_headerInfoMap);
	}

	return false;
}


istd::IChangeableUniquePtr CTableViewParam::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTableViewParam());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTableViewParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_headerInfoMap.clear();

	return true;
}


}


