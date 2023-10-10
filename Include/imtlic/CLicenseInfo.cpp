#include <imtlic/CLicenseInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

QByteArray CLicenseInfo::GetTypeId()
{
	return "LicenseInfo";
}


CLicenseInfo::CLicenseInfo()
{
}


// reimplemented (imtlic::ILicenseInfo)


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


QString CLicenseInfo::GetLicenseDescription() const
{
	return m_description;
}


void CLicenseInfo::SetLicenseDescription(const QString& licenseDescription)
{
	if (m_description != licenseDescription){
		istd::CChangeNotifier notifier(this);

		m_description = licenseDescription;
	}
}


QByteArray CLicenseInfo::GetProductId() const
{
	return m_productId;
}


void CLicenseInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier notifier(this);

		m_productId = productId;
	}
}


QByteArrayList CLicenseInfo::GetFeatures() const
{
	return m_featureIds;
}


void CLicenseInfo::SetFeatures(QByteArrayList features)
{
	if (m_featureIds != features){
		istd::CChangeNotifier notifier(this);

		m_featureIds = features;
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

	bool retVal = true;

	iser::CArchiveTag licenseIdTag("LicenseId", "ID of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseIdTag);
	retVal = retVal && archive.Process(m_licenseId);
	retVal = retVal && archive.EndTag(licenseIdTag);

	iser::CArchiveTag licenseNameTag("LicenseName", "License name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseNameTag);
	retVal = retVal && archive.Process(m_licenseName);
	retVal = retVal && archive.EndTag(licenseNameTag);

	iser::CArchiveTag descriptionTag("LicenseDescription", "Description of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag productIdTag("ProductId", "Product ID of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_featureIds, "Features", "Feature");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CLicenseInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CLicenseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CLicenseInfo* sourcePtr = dynamic_cast<const imtlic::CLicenseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseName = sourcePtr->m_licenseName;
		m_licenseId = sourcePtr->m_licenseId;
		m_description = sourcePtr->m_description;
		m_productId = sourcePtr->m_productId;
		m_featureIds = sourcePtr->m_featureIds;

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
	m_description.clear();
	m_licenseId.clear();
	m_productId.clear();
	m_featureIds.clear();

	return true;
}


} // namespace imtlic


