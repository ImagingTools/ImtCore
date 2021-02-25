#include <imtlic/CFeatureInfoProvider.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

CFeatureInfoProvider::CFeatureInfoProvider()
	:BaseClass("FeatureInfo", "Feature info", "FeatureInfo"),
	m_ownerPtr(nullptr)
{
}


void CFeatureInfoProvider::SetDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds)
{
	istd::CChangeNotifier notifier(this);

	if (dependentIds.isEmpty()){
		m_dependencies.remove(featureId);

		return;
	}

	m_dependencies[featureId] = dependentIds;
}


void CFeatureInfoProvider::SetParents(const QByteArrayList& parentIds)
{
	istd::CChangeNotifier notifier(this);

	m_parents.ResetData();

	for (const QByteArray& parentId : parentIds){
		m_parents.InsertItem(parentId, "", "");
	}
}


// reimplemented (IFeatureDependenciesProvider)

QByteArrayList CFeatureInfoProvider::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (m_dependencies.contains(featureId)){
		return m_dependencies[featureId];
	}

	return QByteArrayList();
}


const IFeatureInfoProvider* CFeatureInfoProvider::GetDependencyContainer(const QByteArray& dependencyId) const
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

const imtbase::ICollectionInfo& CFeatureInfoProvider::GetFeatureList() const
{
	return m_collection;
}


const IFeatureInfo* CFeatureInfoProvider::GetFeatureInfo(const QByteArray& featureId) const
{
	return dynamic_cast<const IFeatureInfo*>(m_collection.GetObjectPtr(featureId));
}


const IFeatureDependenciesProvider* CFeatureInfoProvider::GetDependenciesInfoProvider() const
{
	return this;
}


const imtbase::ICollectionInfo* CFeatureInfoProvider::GetParentFeatureInfoProviderList() const
{
	return &m_parents;
}


const IFeatureInfoProvider* CFeatureInfoProvider::GetParentFeatureInfoProvider(const QByteArray& parentId) const
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

bool CFeatureInfoProvider::Serialize(iser::IArchive& archive)
{
	istd::CChangeGroup changeGroup(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	QByteArrayList ownFeatureIds = m_dependencies.keys();
	int ownFeatureCount = ownFeatureIds.count();

	if (!archive.IsStoring()){
		m_dependencies.clear();
		m_parents.ResetData();
		ownFeatureIds.clear();
		ownFeatureCount = 0;
	}

	static iser::CArchiveTag ownFeatureListTag("Dependencies", "Feature list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag ownFeatureTag("Feature", "Feature", iser::CArchiveTag::TT_GROUP, &ownFeatureListTag);
	retVal = retVal && archive.BeginMultiTag(ownFeatureListTag, ownFeatureTag, ownFeatureCount);

	for (int ownFeatureIndex = 0; ownFeatureIndex < ownFeatureCount; ownFeatureIndex++){
		retVal = retVal && archive.BeginTag(ownFeatureTag);

		QByteArray ownFeatureId;
		if (archive.IsStoring()){
			ownFeatureId = ownFeatureIds[ownFeatureIndex];
		}

		static iser::CArchiveTag ownFeatureIdTag("Id", "Feature ID", iser::CArchiveTag::TT_LEAF, &ownFeatureTag);
		retVal = retVal && archive.BeginTag(ownFeatureIdTag);
		retVal = retVal && archive.Process(ownFeatureId);
		retVal = retVal && archive.EndTag(ownFeatureIdTag);

		QByteArrayList featureIds;
		int featureCount;

		if (archive.IsStoring()){
			featureIds = m_dependencies[ownFeatureId];
			featureCount = featureIds.count();
		}

		static iser::CArchiveTag dependenciesTag("DependsOn", "Dependency feature list", iser::CArchiveTag::TT_MULTIPLE, &ownFeatureTag);
		static iser::CArchiveTag featureIdTag("Id", "Dependency feature ID", iser::CArchiveTag::TT_GROUP, &dependenciesTag);
		retVal = retVal && archive.BeginMultiTag(dependenciesTag, featureIdTag, featureCount);

		for (int featureIdIndex = 0; featureIdIndex < featureCount; featureIdIndex++){
			QByteArray featureId;
			if (archive.IsStoring()){
				featureId = featureIds[featureIdIndex];
			}

			retVal = retVal && archive.BeginTag(featureIdTag);
			retVal = retVal && archive.Process(featureId);
			retVal = retVal && archive.EndTag(featureIdTag);

			if (!archive.IsStoring()){
				featureIds.append(featureId);
			}
		}

		retVal = retVal && archive.EndTag(dependenciesTag);

		retVal = retVal && archive.EndTag(ownFeatureTag);

		if (!archive.IsStoring()){
			SetDependencies(ownFeatureId, featureIds);
		}
	}

	retVal = retVal && archive.EndTag(ownFeatureListTag);

	// parent ids
	QByteArrayList parentIds;
	int parentIdCount;

	if (archive.IsStoring()){
		parentIds = m_parents.GetElementIds().toList();
		parentIdCount = parentIds.count();
	}

	static iser::CArchiveTag parentsTag("Parents", "Parent feature providers", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag parentIdTag("Id", "Parent feature provider ID", iser::CArchiveTag::TT_GROUP, &parentsTag);
	retVal = retVal && archive.BeginMultiTag(parentsTag, parentIdTag, parentIdCount);

	for (int parentIdIndex = 0; parentIdIndex < parentIdCount; parentIdIndex++){
		QByteArray parentId;
		if (archive.IsStoring()){
			parentId = parentIds[parentIdIndex];
		}

		retVal = retVal && archive.BeginTag(parentIdTag);
		retVal = retVal && archive.Process(parentId);
		retVal = retVal && archive.EndTag(parentIdTag);

		if (!archive.IsStoring()){
			parentIds.append(parentId);
		}
	}

	if (!archive.IsStoring()){
		SetParents(parentIds);
	}

	retVal = retVal && archive.EndTag(parentsTag);

	return retVal;
}


} // namespace imtlic


