#include <imtlic/CFeatureInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

CFeatureInfo::CFeatureInfo()
	:m_name(QObject::tr("Feature")),
	m_packagePtr(nullptr),
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


// reimplemented (IFeatureInfo)

const imtlic::IFeatureInfoProvider* CFeatureInfo::GetFeaturePackage() const
{
	return m_packagePtr;
}


QByteArray CFeatureInfo::GetFeatureId() const
{
	return m_id;
}


QString CFeatureInfo::GetFeatureName() const
{
	return m_name;
}


bool CFeatureInfo::IsOptional() const
{
	return m_optional;
}


const IFeatureInfo *CFeatureInfo::GetParentFeature() const
{
	return m_parentFeaturePtr;
}


FeatureInfoList CFeatureInfo::GetSubFeatures() const
{
	return m_subFeatures;
}


bool CFeatureInfo::InsertSubFeature(const IFeatureInfo* subFeatureInfo)
{
	bool retVal = false;
	if (this != subFeatureInfo){
		istd::CChangeNotifier notifier(this);

		m_subFeatures.append(subFeatureInfo);

		retVal = true;
	}

	return retVal;
}


void CFeatureInfo::DeleteSubFeature(const QByteArray &subFeatureId)
{
	for (const IFeatureInfo* subFeatureInfo : m_subFeatures){
		QByteArray featureId = subFeatureInfo->GetFeatureId();

		if (subFeatureId == featureId){
			istd::CChangeNotifier notifier(this);

			m_subFeatures.removeAll(subFeatureInfo);

			break;
		}
	}
}


// reimplemented (iser::ISerializable)

bool CFeatureInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag featureIdTag("Id", "ID of the feature", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(featureIdTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(featureIdTag);

	QByteArray featureId = m_id;

	static iser::CArchiveTag featureNameTag("Name", "Feature name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureNameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(featureNameTag);

	static iser::CArchiveTag featureOptionalTag("Optional", "Optional of the feature", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureOptionalTag);
	retVal = retVal && archive.Process(m_optional);
	retVal = retVal && archive.EndTag(featureOptionalTag);

	static iser::CArchiveTag subFeaturesTag("SubFeatures", "Subfeatures of the feature", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag subfeatureTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &subFeaturesTag);

	int subfeaturesCount = m_subFeatures.count();

	retVal = retVal && archive.BeginMultiTag(subFeaturesTag, subfeatureTag, subfeaturesCount);

	if (!archive.IsStoring()){
		m_subFeatures.resize(subfeaturesCount);
	}

	for (const IFeatureInfo* featureInfo : m_subFeatures){
		retVal = retVal && archive.BeginTag(subfeatureTag);
		retVal = retVal && const_cast<IFeatureInfo*>(featureInfo)->Serialize(archive);
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

		m_id = sourcePtr->GetFeatureId();
		m_name = sourcePtr->GetFeatureName();
		m_optional = sourcePtr->IsOptional();
		m_subFeatures = sourcePtr->GetSubFeatures();
		m_parentFeaturePtr = sourcePtr->GetParentFeature();

		return true;
	}

	return false;
}


bool CFeatureInfo::IsEqual(const IChangeable& object) const
{
	const imtlic::CFeatureInfo* sourcePtr = dynamic_cast<const imtlic::CFeatureInfo*>(&object);
	if (sourcePtr != nullptr){
		return (m_id == sourcePtr->GetFeatureId() &&
				m_name == sourcePtr->GetFeatureName() &&
				m_optional == sourcePtr->IsOptional() &&
				m_parentFeaturePtr == sourcePtr->GetParentFeature() &&
				m_subFeatures == sourcePtr->GetSubFeatures());
	}

	return false;
}


istd::IChangeable* CFeatureInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CFeatureInfo> clonePtr(new CFeatureInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CFeatureInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_name.clear();
	m_subFeatures.clear();
	m_parentFeaturePtr = nullptr;

	return true;
}


} // namespace imtlic


