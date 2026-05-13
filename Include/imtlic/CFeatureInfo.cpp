// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CFeatureInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <imod/TModelWrap.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/imtbase.h>


namespace imtlic
{


// public methods

CFeatureInfo::CFeatureInfo() :
	m_optional(false),
	m_isPermission(true),
	m_parentFeaturePtr(nullptr)
{
}


void CFeatureInfo::SetFeatureId(const QByteArray& featureId)
{
	if (m_id != featureId){
		istd::CChangeNotifier notifier(this);

		m_id = featureId;
	}
}


void CFeatureInfo::SetFeatureName(const QString& featureName)
{
	if (m_name != featureName){
		istd::CChangeNotifier notifier(this);

		m_name = featureName;
	}
}


void CFeatureInfo::SetFeatureDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);

		m_description = description;
	}
}


void CFeatureInfo::SetOptional(bool optional)
{
	if (m_optional != optional){
		istd::CChangeNotifier notifier(this);

		m_optional = optional;
	}
}


void CFeatureInfo::SetParentFeature(const IFeatureInfo *parentFeaturePtr)
{
	if (m_parentFeaturePtr != parentFeaturePtr){
		istd::CChangeNotifier notifier(this);

		m_parentFeaturePtr = parentFeaturePtr;
	}
}


void CFeatureInfo::SetDependencies(const QByteArrayList& dependencies)
{
	if (m_dependencies != dependencies){
		istd::CChangeNotifier notifier(this);

		m_dependencies = dependencies;
	}
}


void CFeatureInfo::SetIsPermission(bool isPermission)
{
	if (m_isPermission != isPermission){
		istd::CChangeNotifier notifier(this);

		m_isPermission = isPermission;
	}
}


// reimplemented (IFeatureInfo)

QByteArray CFeatureInfo::GetFeatureId() const
{
	return m_id;
}


QString CFeatureInfo::GetFeatureName() const
{
	return m_name;
}


QString CFeatureInfo::GetFeatureDescription() const
{
	return m_description;
}


bool CFeatureInfo::IsOptional() const
{
	return m_optional;
}


bool CFeatureInfo::IsPermission() const
{
	return m_isPermission;
}


const IFeatureInfo* CFeatureInfo::GetParentFeature() const
{
	return m_parentFeaturePtr;
}


QByteArrayList CFeatureInfo::GetSubFeatureIds(int maxDepth) const
{
	QByteArrayList retVal;

	int currentDepth = 1;

	GetSubFeaturesRecursive(m_subFeatures, maxDepth, currentDepth, retVal);

	return retVal;
}


IFeatureInfo::FeatureInfoPtr CFeatureInfo::GetSubFeature(const QByteArray& subfeatureId, int maxDepth) const
{
	return GetSubFeatureRecursive(m_subFeatures, subfeatureId, maxDepth);
}


const IFeatureInfo::FeatureInfoList& CFeatureInfo::GetSubFeatures() const
{
	return m_subFeatures;
}


bool CFeatureInfo::InsertSubFeature(FeatureInfoPtr subFeatureInfo)
{
	bool retVal = false;
	if (this != subFeatureInfo.GetPtr()){
		istd::CChangeNotifier notifier(this);

		m_subFeatures.push_back(subFeatureInfo);

		retVal = true;
	}

	return retVal;
}


void CFeatureInfo::DeleteSubFeature(const QByteArray& subFeatureId)
{
	for (int i = 0; i < m_subFeatures.size(); i++){
		FeatureInfoPtr subFeatureInfoPtr = m_subFeatures.at(i);
		QByteArray featureId = subFeatureInfoPtr->GetFeatureId();

		if (subFeatureId == featureId){
			istd::CChangeNotifier notifier(this);

			m_subFeatures.removeAt(i);

			break;
		}
	}
}


QByteArrayList CFeatureInfo::GetDependencies() const
{
	return m_dependencies;
}


// reimplemented (iser::IObject)

QByteArray CFeatureInfo::GetFactoryId() const
{
	return QByteArray("Feature");
}


// reimplemented (iser::ISerializable)

bool CFeatureInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	iser::CArchiveTag featureIdTag("FeatureId", "ID of the feature", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(featureIdTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(featureIdTag);

	iser::CArchiveTag featureNameTag("FeatureName", "Feature name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureNameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(featureNameTag);

	iser::CArchiveTag featureDescriptionTag("FeatureDescription", "Feature description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureDescriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(featureDescriptionTag);

	iser::CArchiveTag featureOptionalTag("Optional", "Optional of the feature", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureOptionalTag);
	retVal = retVal && archive.Process(m_optional);
	retVal = retVal && archive.EndTag(featureOptionalTag);

	if (imtCoreVersion >= 11828){
		iser::CArchiveTag permissionTag("IsPermission", "Is permission", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(permissionTag);
		retVal = retVal && archive.Process(m_isPermission);
		retVal = retVal && archive.EndTag(permissionTag);
	}

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_dependencies, "Dependencies", "Dependency");

	iser::CArchiveTag subFeaturesTag("SubFeatures", "Subfeatures of the feature", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag subfeatureTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &subFeaturesTag);

	int subfeaturesCount = imtbase::narrow_cast<int>(m_subFeatures.size());

	retVal = retVal && archive.BeginMultiTag(subFeaturesTag, subfeatureTag, subfeaturesCount);

	if (!archive.IsStoring()){
		int oldSubfeaturesCount = imtbase::narrow_cast<int>(m_subFeatures.size());

		m_subFeatures.resize(subfeaturesCount);

		for (int i = oldSubfeaturesCount; i < subfeaturesCount; ++i){
			m_subFeatures[i] = new imod::TModelWrap<CFeatureInfo>();
		}
	}

	for (int i = 0; i < m_subFeatures.size(); i++){
		FeatureInfoPtr featureInfoPtr = m_subFeatures[i];
		retVal = retVal && archive.BeginTag(subfeatureTag);
		retVal = retVal && featureInfoPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(subfeatureTag);
	}

	retVal = retVal && archive.EndTag(subFeaturesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CFeatureInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CFeatureInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CFeatureInfo* sourcePtr = dynamic_cast<const imtlic::CFeatureInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_id = sourcePtr->m_id;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_optional = sourcePtr->m_optional;
		m_dependencies =  sourcePtr->m_dependencies;

		m_subFeatures.clear();

		qsizetype subfeaturesCount = sourcePtr->m_subFeatures.count();

		m_subFeatures.resize(subfeaturesCount);

		for (int i = 0; i < subfeaturesCount; ++i){
			m_subFeatures[i] = new imod::TModelWrap<CFeatureInfo>();
		}

		for (int i = 0; i < subfeaturesCount; ++i){
			FeatureInfoPtr featureInfoPtr = m_subFeatures[i];
			bool result = featureInfoPtr->CopyFrom(*sourcePtr->m_subFeatures[i]);
			if (!result){
				return false;
			}
		}

		m_parentFeaturePtr = sourcePtr->GetParentFeature();

		return true;
	}

	return false;
}


bool CFeatureInfo::IsEqual(const IChangeable& object) const
{
	const imtlic::CFeatureInfo* sourcePtr = dynamic_cast<const imtlic::CFeatureInfo*>(&object);
	if (sourcePtr != nullptr){
		const FeatureInfoList& subFeatures = sourcePtr->GetSubFeatures();

		if (m_subFeatures.count() != subFeatures.count()){
			return false;
		}

		for (int i = 0; i < subFeatures.count(); i++){
			FeatureInfoPtr sourceFeatureInfoPtr = subFeatures.at(i);
			bool result = sourceFeatureInfoPtr->IsEqual(*m_subFeatures.at(i));
			if (!result){
				return false;
			}
		}

		return (m_id == sourcePtr->m_id &&
				m_name == sourcePtr->m_name &&
				m_description == sourcePtr->m_description &&
				m_optional == sourcePtr->m_optional &&
				m_parentFeaturePtr == sourcePtr->m_parentFeaturePtr &&
				m_dependencies == sourcePtr->m_dependencies);
	}

	return false;
}


istd::IChangeableUniquePtr CFeatureInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CFeatureInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CFeatureInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_name.clear();
	m_description.clear();
	m_subFeatures.clear();
	m_parentFeaturePtr = nullptr;
	m_dependencies.clear();
	m_optional = false;

	return true;
}


void CFeatureInfo::GetSubFeaturesRecursive(const FeatureInfoList& subFeatures, int maxDepth, int currentDepth, QByteArrayList& featureList) const
{
	if (maxDepth != -1 && maxDepth < currentDepth){
		return;
	}

	for (int i = 0; i < subFeatures.count(); i++){
		FeatureInfoPtr featureInfoPtr = subFeatures.at(i);
		if (featureInfoPtr.IsValid()){
			QByteArray featureId = featureInfoPtr->GetFeatureId();

			featureList << featureId;

			const FeatureInfoList& subfeatureInfoList = featureInfoPtr->GetSubFeatures();

			if (!subfeatureInfoList.isEmpty()){
				GetSubFeaturesRecursive(subfeatureInfoList, maxDepth, currentDepth + 1, featureList);
			}
		}
	}
}


IFeatureInfo::FeatureInfoPtr CFeatureInfo::GetSubFeatureRecursive(const FeatureInfoList& subFeatures, const QByteArray& subfeatureId, int maxDepth, int currentDepth) const
{
	if (maxDepth != -1 && maxDepth < currentDepth){
		return nullptr;
	}

	for (int i = 0; i < subFeatures.count(); i++){
		FeatureInfoPtr featureInfoPtr = subFeatures.at(i);
		QByteArray featureId = featureInfoPtr->GetFeatureId();

		if (featureId == subfeatureId){
			return featureInfoPtr;
		}

		const FeatureInfoList& subfeatureInfoList = featureInfoPtr->GetSubFeatures();
		FeatureInfoPtr subFeatureInfoPtr = GetSubFeatureRecursive(subfeatureInfoList, subfeatureId, maxDepth, currentDepth + 1);
		if (subFeatureInfoPtr.IsValid()){
			return subFeatureInfoPtr;
		}
	}

	return nullptr;
}


} // namespace imtlic


