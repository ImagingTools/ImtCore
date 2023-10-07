#include <imtlic/CProductInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

CProductInfo::CProductInfo()
	:m_featureInfoProviderPtr(nullptr)
{
}


// reimplemented (IProductInfo)

const IProductInfo* CProductInfo::GetProductFamilyInfo() const
{
	return nullptr;
}


QByteArray CProductInfo::GetProductId() const
{
	return m_productId;
}


void CProductInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier notifier(this);

		m_productId = productId;
	}
}


QByteArray CProductInfo::GetCategoryId() const
{
	return m_categoryId;
}


void CProductInfo::SetCategoryId(const QByteArray& categoryId)
{
	if (m_categoryId != categoryId){
		istd::CChangeNotifier notifier(this);

		m_categoryId = categoryId;
	}
}


QByteArrayList CProductInfo::GetFeatureIds() const
{
	return m_featureIds;
}


void CProductInfo::SetFeatureIds(QByteArrayList featureIds)
{
	if (m_featureIds != featureIds){
		istd::CChangeNotifier notifier(this);

		m_featureIds = featureIds;
	}
}


IFeatureInfoProvider* CProductInfo::GetFeatureInfoProvider() const
{
	return m_featureInfoProviderPtr;
}


// reimplemented (iprm::INameParam)

const QString& CProductInfo::GetName() const
{
	return m_productName;
}


void CProductInfo::SetName(const QString& name)
{
	if (m_productName != name){
		istd::CChangeNotifier notifier(this);

		m_productName = name;
	}
}


bool CProductInfo::IsNameFixed() const
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CProductInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	iser::CArchiveTag productNameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	iser::CArchiveTag categoryIdTag("CategoryId", "ID of the product category", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(categoryIdTag);
	retVal = retVal && archive.Process(m_categoryId);
	retVal = retVal && archive.EndTag(categoryIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_featureIds, "Features", "Feature");

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CProductInfo::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	const CProductInfo* sourcePtr = dynamic_cast<const CProductInfo*>(&object);
	if (sourcePtr != nullptr){
		m_productId = sourcePtr->m_productId;
		m_productName = sourcePtr->m_productName;
		m_categoryId = sourcePtr->m_categoryId;
		m_featureIds = sourcePtr->m_featureIds;

		return true;
	}

	return false;
}


bool CProductInfo::IsEqual(const IChangeable& object) const
{
	const CProductInfo* sourcePtr = dynamic_cast<const CProductInfo*>(&object);
	if (sourcePtr != nullptr){
		return	m_productId == sourcePtr->m_productId &&
				m_productName == sourcePtr->m_productName &&
				m_categoryId == sourcePtr->m_categoryId &&
				m_featureIds == sourcePtr->m_featureIds ;
	}

	return false;
}


bool CProductInfo::ResetData(CompatibilityMode mode)
{
	m_productId.clear();
	m_productName.clear();
	m_categoryId.clear();
	m_featureIds.clear();

	return true;
}


} // namespace imtlic


