// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TSingleFactory.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


// public methods

CProductInfo::CProductInfo()
	:m_featureInfoProviderPtr(nullptr)
{
	typedef istd::TSingleFactory<istd::IChangeable, CIdentifiableFeatureInfo> FactoryFeatureInfoImpl;
	m_featureCollection.RegisterFactory<FactoryFeatureInfoImpl>("Feature");
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


QString CProductInfo::GetProductDescription() const
{
	return m_productDescription;
}


void CProductInfo::SetProductDescription(const QString& description)
{
	if (m_productDescription != description){
		istd::CChangeNotifier notifier(this);

		m_productDescription = description;
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


imtbase::IObjectCollection* CProductInfo::GetFeatures()
{
	return &m_featureCollection;
}


bool CProductInfo::AddFeature(const QByteArray& featureId, const IFeatureInfo& featureInfo)
{
	QByteArray retVal = m_featureCollection.InsertNewObject("Feature", "", "", &featureInfo, featureId);

	return !retVal.isEmpty();
}


bool CProductInfo::RemoveFeature(const QByteArray& featureId)
{
	imtbase::ICollectionInfo::Ids elementIds;
	elementIds << featureId;

	return m_featureCollection.RemoveElements(elementIds);
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

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	iser::CArchiveTag productNameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	iser::CArchiveTag productDescriptionTag("ProductDescription", "Description of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productDescriptionTag);
	retVal = retVal && archive.Process(m_productDescription);
	retVal = retVal && archive.EndTag(productDescriptionTag);

	iser::CArchiveTag categoryIdTag("CategoryId", "ID of the product category", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(categoryIdTag);
	retVal = retVal && archive.Process(m_categoryId);
	retVal = retVal && archive.EndTag(categoryIdTag);

	if (imtCoreVersion >= 11786){
		retVal = retVal && m_featureCollection.Serialize(archive);
	}
	else{
		if (!archive.IsStoring()){
			m_featureCollection.ResetData();

			QByteArrayList featureIds;
			retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, featureIds, "Features", "Feature");

			if (m_featureInfoProviderPtr != nullptr){
				for (const QByteArray& featureId : featureIds){
					IFeatureInfoSharedPtr featureInfoPtr = m_featureInfoProviderPtr->GetFeatureInfo(featureId);
					if (featureInfoPtr.IsValid()){
						AddFeature(featureId, *featureInfoPtr.GetPtr());
					}
				}
			}
		}
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CProductInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	const CProductInfo* sourcePtr = dynamic_cast<const CProductInfo*>(&object);
	if (sourcePtr != nullptr){
		m_productId = sourcePtr->m_productId;
		m_productName = sourcePtr->m_productName;
		m_productDescription = sourcePtr->m_productDescription;
		m_categoryId = sourcePtr->m_categoryId;
		m_featureCollection.CopyFrom(sourcePtr->m_featureCollection);

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
				m_productDescription == sourcePtr->m_productDescription &&
				m_categoryId == sourcePtr->m_categoryId &&
				m_featureCollection.IsEqual(sourcePtr->m_featureCollection);
	}

	return false;
}


bool CProductInfo::ResetData(CompatibilityMode mode)
{
	m_productId.clear();
	m_productName.clear();
	m_productDescription.clear();
	m_categoryId.clear();
	m_featureCollection.ResetData(mode);

	return true;
}


} // namespace imtlic


