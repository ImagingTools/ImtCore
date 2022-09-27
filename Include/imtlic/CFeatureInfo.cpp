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
	m_packagePtr(nullptr)
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


QList<const IFeatureInfo*> CFeatureInfo::GetSubFeatures() const
{
	return m_subFeatures;
}


bool CFeatureInfo::InsertSubFeature(const IFeatureInfo* subFeatureInfo)
{
	m_subFeatures.append(subFeatureInfo);

	return true;
}


void CFeatureInfo::DeleteSubFeature(const QByteArray &subFeatureId)
{
	for (const IFeatureInfo* subFeatureInfo : m_subFeatures){
		QByteArray featureId = subFeatureInfo->GetFeatureId();

		if (subFeatureId == featureId){
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

	static iser::CArchiveTag featureNameTag("Name", "Feature name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureNameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(featureNameTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CFeatureInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CFeatureInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::IFeatureInfo* sourcePtr = dynamic_cast<const imtlic::IFeatureInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_id = sourcePtr->GetFeatureId();
		m_name = sourcePtr->GetFeatureName();
		m_subFeatures = sourcePtr->GetSubFeatures();

		return true;
	}

	return false;
}


bool CFeatureInfo::IsEqual(const IChangeable& object) const
{
	const imtlic::IFeatureInfo* sourcePtr = dynamic_cast<const imtlic::IFeatureInfo*>(&object);
	if (sourcePtr != nullptr){
		return (m_id == sourcePtr->GetFeatureId() && m_name == sourcePtr->GetFeatureName() && m_subFeatures == sourcePtr->GetSubFeatures());
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

	return true;
}


} // namespace imtlic


