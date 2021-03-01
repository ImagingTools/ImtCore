#include <imtlic/CFeaturePackage.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

CFeaturePackage::CFeaturePackage()
	:BaseClass("FeatureInfo", "Feature info", "FeatureInfoProvider"),
	m_ownerPtr(nullptr)
{
}


void CFeaturePackage::SetDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds)
{
	istd::CChangeNotifier notifier(this);

	if (dependentIds.isEmpty()){
		m_dependencies.remove(featureId);

		return;
	}

	m_dependencies[featureId] = dependentIds;
}


void CFeaturePackage::SetParents(const QByteArrayList& parentIds)
{
	istd::CChangeNotifier notifier(this);

	m_parents.ResetData();

	for (const QByteArray& parentId : parentIds){
		m_parents.InsertItem(parentId, "", "");
	}
}


// reimplemented (IFeatureDependenciesProvider)

QByteArrayList CFeaturePackage::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (m_dependencies.contains(featureId)){
		return m_dependencies[featureId];
	}

	return QByteArrayList();
}


const IFeatureInfoProvider* CFeaturePackage::GetDependencyContainer(const QByteArray& dependencyId) const
{
	if (m_ownerPtr != nullptr){
		imtbase::ICollectionInfo::Ids featureInfoProviderIds = m_ownerPtr->GetElementIds();
		for (const QByteArray& featureInfoProviderId : featureInfoProviderIds){
			IFeatureInfoProvider* featureInfoProviderPtr = dynamic_cast<IFeatureInfoProvider*>(
						const_cast<istd::IChangeable*>(m_ownerPtr->GetObjectPtr(featureInfoProviderId)));

			if (featureInfoProviderPtr != nullptr){
				const imtbase::ICollectionInfo::Ids& featureInfoIds = featureInfoProviderPtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureInfoId : featureInfoIds){
					const IFeatureInfo* featureInfoPtr = featureInfoProviderPtr->GetFeatureInfo(featureInfoId);
					if (featureInfoPtr != nullptr){
						if (featureInfoPtr->GetFeatureId() == dependencyId){
							return featureInfoProviderPtr;
						}
					}
				}
			}
		}
	}

	return nullptr;
}


// reimplemented (IFeatureInfoProvider)

const imtbase::ICollectionInfo& CFeaturePackage::GetFeatureList() const
{
	return m_collection;
}


const IFeatureInfo* CFeaturePackage::GetFeatureInfo(const QByteArray& featureId) const
{
	return dynamic_cast<const IFeatureInfo*>(m_collection.GetObjectPtr(featureId));
}


const IFeatureDependenciesProvider* CFeaturePackage::GetDependenciesInfoProvider() const
{
	return this;
}


const imtbase::ICollectionInfo* CFeaturePackage::GetParentFeatureInfoProviderList() const
{
	return &m_parents;
}


const IFeatureInfoProvider* CFeaturePackage::GetParentFeatureInfoProvider(const QByteArray& parentId) const
{
	if (m_ownerPtr != nullptr){
		if (m_parents.GetElementIds().contains(parentId)){
			imtbase::ICollectionInfo::Ids featureInfoProviderIds = m_ownerPtr->GetElementIds();
			for (const QByteArray& featureInfoProviderId : featureInfoProviderIds){
				IFeatureInfoProvider* featureInfoProviderPtr = dynamic_cast<IFeatureInfoProvider*>(
							const_cast<istd::IChangeable*>(m_ownerPtr->GetObjectPtr(featureInfoProviderId)));

				if (featureInfoProviderPtr != nullptr){
					if (featureInfoProviderId == parentId){
						return featureInfoProviderPtr;
					}
				}
			}
		}
	}

	return nullptr;
}


// reimplemented (iser::ISerializable)

bool CFeaturePackage::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	QByteArrayList dependencyKeys = m_dependencies.keys();
	int dependencyCount = dependencyKeys.count();

	if (!archive.IsStoring()){
		m_dependencies.clear();
		m_parents.ResetData();
		dependencyKeys.clear();
		dependencyCount = 0;
	}

	static iser::CArchiveTag dependenciesTag("Dependencies", "Feature list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag dependencyTag("Dependency", "Dependency", iser::CArchiveTag::TT_GROUP, &dependenciesTag);

	retVal = retVal && archive.BeginMultiTag(dependenciesTag, dependencyTag, dependencyCount);

	for (int dependencyIndex = 0; dependencyIndex < dependencyCount; dependencyIndex++){
		retVal = retVal && archive.BeginTag(dependencyTag);

		QByteArray key;
		QByteArrayList value;
		int valueCount = 0;

		if (archive.IsStoring()){
			key = dependencyKeys[dependencyIndex];
			value = m_dependencies[key];
			valueCount = value.count();
		}

		static iser::CArchiveTag keyTag("FeatureId", "FeatureId", iser::CArchiveTag::TT_LEAF, &dependencyTag);
		retVal = retVal && archive.BeginTag(keyTag);
		retVal = retVal && archive.Process(key);
		retVal = retVal && archive.EndTag(keyTag);

		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, value, "BaseFeatures", "BaseFeatureId");

		if (!archive.IsStoring()){
			m_dependencies[key] = value;
		}

		retVal = retVal && archive.EndTag(dependencyTag);
	}

	retVal = retVal && archive.EndTag(dependenciesTag);

	static iser::CArchiveTag parentsTag("Parents", "Parent feature providers", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(parentsTag);
	retVal = retVal && m_parents.Serialize(archive);
	retVal = retVal && archive.EndTag(parentsTag);

	return retVal;
}


} // namespace imtlic


