// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInfo.h>


// STL includes
#include <functional>

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

			for (const QByteArray& featureId : featureIds){
				IFeatureInfoSharedPtr featureInfoPtr;
				if (m_featureInfoProviderPtr != nullptr){
					featureInfoPtr = m_featureInfoProviderPtr->GetFeatureInfo(featureId);
				}

				if (featureInfoPtr.IsValid()){
					AddFeature(featureId, *featureInfoPtr.GetPtr());
				}
				else{
					// Without a provider only the ID is known - keep the membership anyway.
					CIdentifiableFeatureInfo featureInfo;
					featureInfo.SetObjectUuid(featureId);
					featureInfo.SetFeatureId(featureId);

					AddFeature(featureId, featureInfo);
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
typedef QHash<QByteArray, QByteArrayList> LeavesByPath;


/**
	\return The leaves of the feature: the feature itself if it has no sub-features.
*/
QByteArrayList CollectFeatureRequirements(
			const IFeatureInfo& featureInfo,
			const QByteArray& parentPath,
			RequirementsByPath& requirementsByPath,
			LeavesByPath& leavesByPath,
			const QByteArrayList& inheritedRequirements = QByteArrayList())
{
	const QByteArray featureId = featureInfo.GetFeatureId();
	if (featureId.isEmpty()){
		return QByteArrayList();
	}

	const QByteArray featurePath = parentPath + '/' + featureId;

	// Requiring the own branch would hand out the siblings, so such requirements are dropped.
	QByteArrayList requirements = inheritedRequirements;
	for (const QByteArray& requirement : featureInfo.GetRequirements()){
		if (!requirement.startsWith('/') || IsSameOrNestedFeaturePath(requirement, featurePath)){
			continue;
		}

		if (!requirements.contains(requirement)){
			requirements.append(requirement);
		}
	}
	requirementsByPath.insert(featurePath, requirements);

	QByteArrayList leaves;
	const IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	for (const IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
		if (subFeaturePtr.IsValid()){
			leaves += CollectFeatureRequirements(*subFeaturePtr, featurePath, requirementsByPath, leavesByPath, requirements);
		}
	}

	if (subFeatures.isEmpty()){
		leaves.append(featurePath);
	}
	leavesByPath.insert(featurePath, leaves);

	return leaves;
}


void CollectProductRequirements(
			IProductInfo& productInfo,
			RequirementsByPath& requirementsByPath,
			LeavesByPath& leavesByPath)
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
			CollectFeatureRequirements(*featureInfoPtr, QByteArray(), requirementsByPath, leavesByPath);
		}
	}
}


} // anonymous namespace


// public functions

PermissionRequirements CollectPermissionRequirements(IProductInfo& productInfo)
{
	RequirementsByPath requirementsByPath;
	LeavesByPath leavesByPath;
	CollectProductRequirements(productInfo, requirementsByPath, leavesByPath);

	PermissionRequirements retVal;
	for (RequirementsByPath::const_iterator it = requirementsByPath.constBegin(); it != requirementsByPath.constEnd(); ++it){
		QByteArrayList requiredLeaves;
		for (const QByteArray& requirement : it.value()){
			// A required group stands for every leaf in it: only leaves are ever held.
			for (const QByteArray& leafPath : leavesByPath.value(requirement)){
				if (!requiredLeaves.contains(leafPath)){
					requiredLeaves.append(leafPath);
				}
			}
		}

		if (!requiredLeaves.isEmpty()){
			retVal.insert(it.key(), requiredLeaves);
		}
	}

	return retVal;
}


QByteArrayList CollectImpliedPermissions(IProductInfo& productInfo, const QByteArrayList& permissionIds)
{
	if (permissionIds.isEmpty()){
		return QByteArrayList();
	}

	return CollectImpliedPermissions(CollectPermissionRequirements(productInfo), permissionIds);
}


QByteArrayList CollectImpliedPermissions(const PermissionRequirements& requirements, const QByteArrayList& permissionIds)
{
	QByteArrayList retVal;
	if (requirements.isEmpty()){
		return retVal;
	}

	QSet<QByteArray> knownPermissions(permissionIds.begin(), permissionIds.end());
	QByteArrayList pendingPermissions = permissionIds;

	while (!pendingPermissions.isEmpty()){
		const QByteArray permissionId = pendingPermissions.takeFirst();

		for (const QByteArray& leafPath : requirements.value(permissionId)){
			if (knownPermissions.contains(leafPath)){
				continue;
			}

			knownPermissions.insert(leafPath);
			retVal.append(leafPath);
			pendingPermissions.append(leafPath);
		}
	}

	return retVal;
}


QByteArrayList CollectProductPermissions(IProductInfo& productInfo)
{
	QByteArrayList retVal;

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr == nullptr){
		return retVal;
	}

	std::function<void (const IFeatureInfo&, const QByteArray&)> collect = [&retVal, &collect](const IFeatureInfo& featureInfo, const QByteArray& parentPath){
		const QByteArray featureId = featureInfo.GetFeatureId();
		if (featureId.isEmpty()){
			return;
		}

		const QByteArray featurePath = parentPath + '/' + featureId;
		const IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
		if (subFeatures.isEmpty()){
			if (featureInfo.IsPermission() && !retVal.contains(featurePath)){
				retVal.append(featurePath);
			}

			return;
		}

		for (const IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
			if (subFeaturePtr.IsValid()){
				collect(*subFeaturePtr, featurePath);
			}
		}
	};

	for (const imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
			const IFeatureInfo* featureInfoPtr = dynamic_cast<const IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				collect(*featureInfoPtr, QByteArray());
			}
		}
	}

	return retVal;
}


} // namespace imtlic

