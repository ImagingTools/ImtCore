#include <imtlic/CProductLicensingInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtlic
{


// public methods

CProductLicensingInfo::CProductLicensingInfo()
	:m_featurePackagesPtr(nullptr)
{
}


// reimplemented (imtlic::IProductLicensingInfo)

const imtbase::IObjectCollection* CProductLicensingInfo::GetFeaturePackages() const
{
	return m_featurePackagesPtr;
}


// reimplemented (imtlic::IProductInfo)

const IProductInfo* CProductLicensingInfo::GetProductFamilyInfo() const
{
	return nullptr;
}


QByteArray CProductLicensingInfo::GetProductId() const
{
	return m_productId;
}


void CProductLicensingInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


QByteArray CProductLicensingInfo::GetCategoryId() const
{
	return m_categoryId;
}


void CProductLicensingInfo::SetCategoryId(const QByteArray& categoryId)
{
	if (m_categoryId != categoryId){
		istd::CChangeNotifier changeNotifier(this);

		m_categoryId = categoryId;
	}
}


// reimplemented (imtlic::ILicenseDependenciesManager)

void CProductLicensingInfo::SetLicenseDependencies(const QByteArray& licenseId, const QByteArrayList& dependentIds)
{
	istd::CChangeNotifier notifier(this);

	if (dependentIds.isEmpty()){
		m_dependencies.remove(licenseId);

		return;
	}

	m_dependencies[licenseId] = dependentIds;
}


// reimplemented (imtlic::ILicenseDependenciesProvider)

QByteArrayList CProductLicensingInfo::GetLicenseDependencies(const QByteArray& licenseId) const
{
	if (m_dependencies.contains(licenseId)){
		return m_dependencies[licenseId];
	}

	return QByteArrayList();
}


// reimplemented (iprm::INameParam)

const QString& CProductLicensingInfo::GetName() const
{
	return m_productName;
}


void CProductLicensingInfo::SetName(const QString& name)
{
	if (m_productName != name){
		istd::CChangeNotifier changeNotifier(this);

		m_productName = name;
	}
}


bool CProductLicensingInfo::IsNameFixed() const
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CProductLicensingInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag productNameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProductLicensingInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CProductLicensingInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CProductLicensingInfo* sourcePtr = dynamic_cast<const imtlic::CProductLicensingInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		if (BaseClass::CopyFrom(*sourcePtr)){
			m_productId = sourcePtr->m_productId;
			m_productName = sourcePtr->m_productName;

			return true;
		}
	}

	return false;
}


istd::IChangeable* CProductLicensingInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CProductLicensingInfo> clonePtr(new CProductLicensingInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CProductLicensingInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData();

	m_productId.clear();

	m_productName.clear();

	return true;
}


} // namespace imtlic


