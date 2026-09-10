// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInfo.h>


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QSet>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TSingleFactory.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/imtbase.h>
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

	// The optional parts taken from this feature go with it: an entry naming a
	// feature the product no longer has is a grant of nothing.
	for (qsizetype index = m_optionalFeatures.size() - 1; index >= 0; --index){
		if (m_optionalFeatures[index].featureId == featureId){
			m_optionalFeatures.remove(index);
		}
	}

	return m_featureCollection.RemoveElements(elementIds);
}


IProductInfo::OptionalFeatureInfos CProductInfo::GetOptionalFeatures() const
{
	return m_optionalFeatures;
}


void CProductInfo::SetOptionalFeatures(const OptionalFeatureInfos& optionalFeatures)
{
	istd::CChangeNotifier changeNotifier(this);

	m_optionalFeatures = optionalFeatures;
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

	// Products written before this field existed carry no such tag; the version
	// number stored in their archive header is what keeps us from looking for one.
	if (imtCoreVersion >= 22921){
		if (!archive.IsStoring()){
			m_optionalFeatures.clear();
		}

		iser::CArchiveTag optionalFeaturesTag("OptionalFeatures", "Optional sub-features taken by this product", iser::CArchiveTag::TT_MULTIPLE);
		iser::CArchiveTag optionalFeatureTag("OptionalFeature", "Optional sub-features of one feature", iser::CArchiveTag::TT_GROUP);

		int optionalFeaturesCount = imtbase::narrow_cast<int>(m_optionalFeatures.count());
		retVal = retVal && archive.BeginMultiTag(optionalFeaturesTag, optionalFeatureTag, optionalFeaturesCount);

		if (retVal && !archive.IsStoring()){
			m_optionalFeatures.resize(optionalFeaturesCount);
		}

		for (int i = 0; i < optionalFeaturesCount; ++i){
			retVal = retVal && archive.BeginTag(optionalFeatureTag);

			iser::CArchiveTag featureIdTag("FeatureId", "ID of the feature these parts belong to", iser::CArchiveTag::TT_LEAF, &optionalFeatureTag);
			retVal = retVal && archive.BeginTag(featureIdTag);
			retVal = retVal && archive.Process(m_optionalFeatures[i].featureId);
			retVal = retVal && archive.EndTag(featureIdTag);

			retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(
						archive, m_optionalFeatures[i].subFeatureIds, "SubFeatures", "SubFeature");

			retVal = retVal && archive.EndTag(optionalFeatureTag);
		}

		retVal = retVal && archive.EndTag(optionalFeaturesTag);
	}
	else{
		if (!archive.IsStoring()){
			m_optionalFeatures.clear();
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
		m_optionalFeatures = sourcePtr->m_optionalFeatures;

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
				m_featureCollection.IsEqual(sourcePtr->m_featureCollection) &&
				m_optionalFeatures == sourcePtr->m_optionalFeatures;
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
	m_optionalFeatures.clear();

	return true;
}


namespace
{


typedef QHash<QByteArray, QByteArrayList> RequirementsByPath;
typedef QHash<QByteArray, QByteArrayList> PathsByFeatureId;


void CollectFeatureRequirements(
			const IFeatureInfo& featureInfo,
			const QByteArray& parentPath,
			RequirementsByPath& requirementsByPath,
			PathsByFeatureId& pathsByFeatureId)
{
	const QByteArray featureId = featureInfo.GetFeatureId();
	if (featureId.isEmpty()){
		return;
	}

	const QByteArray featurePath = parentPath + '/' + featureId;

	requirementsByPath.insert(featurePath, featureInfo.GetRequirements());

	// Permissions stored before paths existed name a feature by its id alone,
	// and such an id holds for every feature carrying it - that is how the
	// permission check itself reads them (\see imtauth::HasPermission), so the
	// requirements of all of them are kept reachable under that id.
	pathsByFeatureId[featureId].append(featurePath);

	const IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	for (const IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
		if (subFeaturePtr.IsValid()){
			CollectFeatureRequirements(*subFeaturePtr, featurePath, requirementsByPath, pathsByFeatureId);
		}
	}
}


void CollectProductRequirements(
			IProductInfo& productInfo,
			RequirementsByPath& requirementsByPath,
			PathsByFeatureId& pathsByFeatureId)
{
	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr == nullptr){
		return;
	}

	// Reading an element hands out a deep copy of a whole feature tree, so the
	// product is flattened once here and every lookup is answered from the maps.
	const imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!featureCollectionPtr->GetObjectData(elementId, dataPtr)){
			continue;
		}

		const IFeatureInfo* featureInfoPtr = dynamic_cast<const IFeatureInfo*>(dataPtr.GetPtr());
		if (featureInfoPtr != nullptr){
			CollectFeatureRequirements(*featureInfoPtr, QByteArray(), requirementsByPath, pathsByFeatureId);
		}
	}
}


QByteArrayList GetPermissionRequirements(
			const QByteArray& permissionId,
			const RequirementsByPath& requirementsByPath,
			const PathsByFeatureId& pathsByFeatureId)
{
	if (permissionId.isEmpty()){
		return QByteArrayList();
	}

	const QByteArray featurePath = permissionId.startsWith('/') ? permissionId : ('/' + permissionId);

	RequirementsByPath::const_iterator requirementsIter = requirementsByPath.constFind(featurePath);
	if (requirementsIter != requirementsByPath.constEnd()){
		return requirementsIter.value();
	}

	// A permission stored in the old format names a feature by its id alone,
	// which every feature carrying that id answers for. A path that names no
	// feature is not resolved by its last segment instead: it says which parent
	// it belongs to, and another branch grants requirements of its own.
	if (permissionId.contains('/')){
		return QByteArrayList();
	}

	QByteArrayList retVal;
	for (const QByteArray& knownPath : pathsByFeatureId.value(permissionId)){
		retVal += requirementsByPath.value(knownPath);
	}

	return retVal;
}


} // anonymous namespace


// public functions

QByteArrayList CollectImpliedPermissions(IProductInfo& productInfo, const QByteArrayList& permissionIds)
{
	if (permissionIds.isEmpty()){
		return QByteArrayList();
	}

	RequirementsByPath requirementsByPath;
	PathsByFeatureId pathsByFeatureId;
	CollectProductRequirements(productInfo, requirementsByPath, pathsByFeatureId);
	if (requirementsByPath.isEmpty()){
		return QByteArrayList();
	}

	QByteArrayList retVal;

	QSet<QByteArray> knownPermissions(permissionIds.begin(), permissionIds.end());
	QByteArrayList pendingPermissions = permissionIds;

	while (!pendingPermissions.isEmpty()){
		const QByteArray permissionId = pendingPermissions.takeFirst();

		const QByteArrayList requirements = GetPermissionRequirements(permissionId, requirementsByPath, pathsByFeatureId);
		for (const QByteArray& requirement : requirements){
			if (requirement.isEmpty() || knownPermissions.contains(requirement)){
				continue;
			}

			knownPermissions.insert(requirement);
			retVal.append(requirement);
			pendingPermissions.append(requirement);
		}
	}

	return retVal;
}


} // namespace imtlic


