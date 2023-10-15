#include <imtlic/CLicenseDefinition.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

QByteArray CLicenseDefinition::GetTypeId() const
{
	return "LicenseInfo";
}


CLicenseDefinition::CLicenseDefinition()
{
}


// reimplemented (imtlic::ILicenseDefinition)


QString CLicenseDefinition::GetLicenseName() const
{
	return m_licenseName;
}


void CLicenseDefinition::SetLicenseName(const QString& licenseName)
{
	if (m_licenseName != licenseName){
		istd::CChangeNotifier notifier(this);

		m_licenseName = licenseName;
	}
}


QByteArray CLicenseDefinition::GetLicenseId() const
{
	return m_licenseId;
}


void CLicenseDefinition::SetLicenseId(const QByteArray& licenseId)
{
	if (m_licenseId != licenseId){
		istd::CChangeNotifier notifier(this);

		m_licenseId = licenseId;
	}
}


QString CLicenseDefinition::GetLicenseDescription() const
{
	return m_description;
}


void CLicenseDefinition::SetLicenseDescription(const QString& licenseDescription)
{
	if (m_description != licenseDescription){
		istd::CChangeNotifier notifier(this);

		m_description = licenseDescription;
	}
}


QByteArray CLicenseDefinition::GetProductId() const
{
	return m_productId;
}


void CLicenseDefinition::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier notifier(this);

		m_productId = productId;
	}
}


//QByteArrayList CLicenseDefinition::GetFeatures() const
//{
//	return m_featureIds;
//}


//void CLicenseDefinition::SetFeatures(QByteArrayList features)
//{
//	if (m_featureIds != features){
//		istd::CChangeNotifier notifier(this);

//		m_featureIds = features;
//	}
//}


QByteArrayList CLicenseDefinition::GetDependencies() const
{
	return m_dependencies;
}


void CLicenseDefinition::SetDependencies(QByteArrayList dependencies)
{
	if (m_dependencies != dependencies){
		istd::CChangeNotifier notifier(this);

		m_dependencies = dependencies;
	}
}


ILicenseDefinition::FeatureInfos CLicenseDefinition::GetFeatureInfos() const
{
	return m_featureInfos;
}


void CLicenseDefinition::SetFeatureInfos(const ILicenseDefinition::FeatureInfos& featureInfos)
{
	if (m_featureInfos != featureInfos){
		istd::CChangeNotifier notifier(this);

		m_featureInfos = featureInfos;
	}
}


// reimplemented (iser::IObject)

QByteArray CLicenseDefinition::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CLicenseDefinition::Serialize(iser::IArchive& archive)
{
	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag licenseIdTag("LicenseId", "ID of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseIdTag);
	retVal = retVal && archive.Process(m_licenseId);
	retVal = retVal && archive.EndTag(licenseIdTag);

	iser::CArchiveTag licenseNameTag("LicenseName", "License name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseNameTag);
	retVal = retVal && archive.Process(m_licenseName);
	retVal = retVal && archive.EndTag(licenseNameTag);

	if (imtCoreVersion > 7605){
		iser::CArchiveTag descriptionTag("LicenseDescription", "Description of the license", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(descriptionTag);
		retVal = retVal && archive.Process(m_description);
		retVal = retVal && archive.EndTag(descriptionTag);

		iser::CArchiveTag productIdTag("ProductId", "Product ID of the license", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(productIdTag);
		retVal = retVal && archive.Process(m_productId);
		retVal = retVal && archive.EndTag(productIdTag);
	}

	if (!archive.IsStoring()){
		m_featureInfos.clear();
	}

	iser::CArchiveTag featuresTag("Features", "List of license features", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag featureInfoTag("Feature", "Feature information", iser::CArchiveTag::TT_GROUP);

	int featuresCount = m_featureInfos.count();
	retVal = retVal && archive.BeginMultiTag(featuresTag, featureInfoTag, featuresCount);

	if (retVal && !archive.IsStoring()){
		m_featureInfos.resize(featuresCount);
	}

	for (int i = 0; i < featuresCount; ++i){
		retVal = retVal && archive.BeginTag(featureInfoTag);

		iser::CArchiveTag featureIdTag("ID", "Feature ID", iser::CArchiveTag::TT_LEAF, &featureInfoTag);
		retVal = retVal && archive.BeginTag(featureIdTag);
		retVal = retVal && archive.Process(m_featureInfos[i].id);
		retVal = retVal && archive.EndTag(featureIdTag);

		iser::CArchiveTag featureNameTag("Name", "Feature name", iser::CArchiveTag::TT_LEAF, &featureInfoTag);
		retVal = retVal && archive.BeginTag(featureNameTag);
		retVal = retVal && archive.Process(m_featureInfos[i].name);
		retVal = retVal && archive.EndTag(featureNameTag);

		retVal = retVal && archive.EndTag(featureInfoTag);
	}

	retVal = retVal && archive.EndTag(featuresTag);

	if (imtCoreVersion > 7605){
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_dependencies, "Dependencies", "Dependency");
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CLicenseDefinition::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CLicenseDefinition::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CLicenseDefinition* sourcePtr = dynamic_cast<const imtlic::CLicenseDefinition*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseName = sourcePtr->m_licenseName;
		m_licenseId = sourcePtr->m_licenseId;
		m_description = sourcePtr->m_description;
		m_productId = sourcePtr->m_productId;
		m_featureInfos = sourcePtr->m_featureInfos;
		m_dependencies = sourcePtr->m_dependencies;

		return true;
	}

	return false;
}


istd::IChangeable* CLicenseDefinition::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CLicenseDefinition> clonePtr(new CLicenseDefinition);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CLicenseDefinition::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenseName.clear();
	m_licenseId.clear();
	m_description.clear();
	m_licenseId.clear();
	m_productId.clear();
	m_featureInfos.clear();
	m_dependencies.clear();

	return true;
}


} // namespace imtlic


