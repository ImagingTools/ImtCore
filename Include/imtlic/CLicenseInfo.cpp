#include <imtlic/CLicenseInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


// public methods

QByteArray CLicenseInfo::GetTypeId()
{
	return "LicenseInfo";
}


CLicenseInfo::CLicenseInfo()
	:m_featurePackageCollectionPtr(nullptr)
{
}


// reimplemented (imtlic::ILicenseInfo)

const imtbase::IObjectCollection* CLicenseInfo::GetFeaturePackages() const
{
	return m_featurePackageCollectionPtr;
}


QString CLicenseInfo::GetLicenseName() const
{
	return m_licenseName;
}


void CLicenseInfo::SetLicenseName(const QString& licenseName)
{
	if (m_licenseName != licenseName){
		istd::CChangeNotifier notifier(this);

		m_licenseName = licenseName;
	}
}


QByteArray CLicenseInfo::GetLicenseId() const
{
	return m_licenseId;
}


void CLicenseInfo::SetLicenseId(const QByteArray& licenseId)
{
	if (m_licenseId != licenseId){
		istd::CChangeNotifier notifier(this);

		m_licenseId = licenseId;
	}
}


CLicenseInfo::FeatureInfos CLicenseInfo::GetFeatureInfos() const
{
	return m_featureInfos;
}


void CLicenseInfo::SetFeatureInfos(const FeatureInfos& featureInfos)
{
	if (m_featureInfos != featureInfos){
		istd::CChangeNotifier notifier(this);

		m_featureInfos = featureInfos;
	}
}


// reimplemented (iser::IObject)

QByteArray CLicenseInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CLicenseInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag licenseNameTag("LicenseName", "License name", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(licenseNameTag);
	retVal = retVal && archive.Process(m_licenseName);
	retVal = retVal && archive.EndTag(licenseNameTag);

	static iser::CArchiveTag licenseIdTag("LicenseId", "ID of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseIdTag);
	retVal = retVal && archive.Process(m_licenseId);
	retVal = retVal && archive.EndTag(licenseIdTag);

	if (!archive.IsStoring()){
		m_featureInfos.clear();
	}

	quint32 imtCoreVersion;
	bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);

	bool oldVersion = (imtCoreVersionExists && imtCoreVersion < 3248);

	if (!archive.IsStoring() && oldVersion){
		QByteArrayList featureIds;
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, featureIds, "Features", "Feature");

		if (retVal){
			for (const QByteArray& featureId: featureIds){
				FeatureInfo featureInfo;
				featureInfo.id = featureId;

				m_featureInfos.push_back(featureInfo);
			}
		}
	}
	else{
		static iser::CArchiveTag featuresTag("Features", "List of license features", iser::CArchiveTag::TT_MULTIPLE);
		static iser::CArchiveTag featureInfoTag("Feature", "Feature information", iser::CArchiveTag::TT_GROUP);

		int featuresCount = m_featureInfos.count();
		retVal = retVal && archive.BeginMultiTag(featuresTag, featureInfoTag, featuresCount);

		if (retVal && !archive.IsStoring()){
			m_featureInfos.resize(featuresCount);
		}

		for (int i = 0; i < featuresCount; ++i){
			retVal = retVal && archive.BeginTag(featureInfoTag);

			static iser::CArchiveTag featureIdTag("ID", "Feature ID", iser::CArchiveTag::TT_LEAF, &featureInfoTag);
			retVal = retVal && archive.BeginTag(featureIdTag);
			retVal = retVal && archive.Process(m_featureInfos[i].id);
			retVal = retVal && archive.EndTag(featureIdTag);

			static iser::CArchiveTag featureNameTag("Name", "Feature name", iser::CArchiveTag::TT_LEAF, &featureInfoTag);
			retVal = retVal && archive.BeginTag(featureNameTag);
			retVal = retVal && archive.Process(m_featureInfos[i].name);
			retVal = retVal && archive.EndTag(featureNameTag);

			retVal = retVal && archive.EndTag(featureInfoTag);
		}

		retVal = retVal && archive.EndTag(featuresTag);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CLicenseInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CLicenseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::ILicenseInfo* sourcePtr = dynamic_cast<const imtlic::ILicenseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseName = sourcePtr->GetLicenseName();
		m_licenseId = sourcePtr->GetLicenseId();
		m_featureInfos = sourcePtr->GetFeatureInfos();

		return true;
	}

	return false;
}


istd::IChangeable* CLicenseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CLicenseInfo> clonePtr(new CLicenseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CLicenseInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenseName.clear();
	m_licenseId.clear();
	m_featureInfos.clear();

	return true;
}


} // namespace imtlic


