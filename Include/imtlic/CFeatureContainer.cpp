#include <imtlic/CFeatureContainer.h>


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

CFeatureContainer::CFeatureContainer()
    :BaseClass("FeatureInfo", "Feature info", "Features")
{
}


const IFeatureInfo* CFeatureContainer::FindFeatureById(const QByteArray& featureId) const
{
    imtbase::ICollectionInfo::Ids featureIds = GetFeatureList().GetElementIds();
    for (QByteArray id : featureIds){
        const IFeatureInfo* featurePtr = GetFeatureInfo(id);
        Q_ASSERT(featurePtr != nullptr);
        if (featurePtr != nullptr){
            if (featurePtr->GetFeatureId() == featureId){
                return featurePtr;
            }
        }
    }

    return nullptr;
}


QByteArray CFeatureContainer::GetFeatureCollectionId(const QByteArray& featureId) const
{
    imtbase::ICollectionInfo::Ids featureIds = GetFeatureList().GetElementIds();
    for (QByteArray id : featureIds){
        const IFeatureInfo* featurePtr = GetFeatureInfo(id);
        Q_ASSERT(featurePtr != nullptr);
        if (featurePtr != nullptr){
            if (featurePtr->GetFeatureId() == featureId){
                return id;
            }
        }
    }

    return QByteArray();
}


// reimplemented (IFeaturePackage)

QByteArray CFeatureContainer::GetPackageId() const
{
    return m_packageId;
}


void CFeatureContainer::SetPackageId(const QByteArray& packageId)
{
    if (m_packageId != packageId){
        istd::CChangeNotifier notifier(this);

        m_packageId = packageId;
    }
}


// reimplemented (IFeatureInfoProvider)

const imtbase::ICollectionInfo& CFeatureContainer::GetFeatureList() const
{
    return m_collection;
}


const IFeatureInfo* CFeatureContainer::GetFeatureInfo(const QByteArray& featureId) const
{
    return dynamic_cast<const IFeatureInfo*>(m_collection.GetObjectPtr(featureId));
}


const imtbase::IObjectCollection* CFeatureContainer::GetFeaturePackages() const
{
	return nullptr;
}


const IFeatureDependenciesProvider* CFeatureContainer::GetDependenciesInfoProvider() const
{
	return nullptr;
}


const imtbase::ICollectionInfo* CFeatureContainer::GetParentFeatureInfoProviderList() const
{
	return nullptr;
}


const IFeatureInfoProvider* CFeatureContainer::GetParentFeatureInfoProvider(const QByteArray& parentId) const
{
	return nullptr;
}


// reimplemented (iser::ISerializable)

bool CFeatureContainer::Serialize(iser::IArchive& archive)
{
    istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

    bool retVal = BaseClass::Serialize(archive);

    static iser::CArchiveTag packageIdTag("PackageId", "ID of the feature package", iser::CArchiveTag::TT_LEAF);
    retVal = retVal && archive.BeginTag(packageIdTag);
    retVal = retVal && archive.Process(m_packageId);
    retVal = retVal && archive.EndTag(packageIdTag);

    return retVal;
}


// reimplemented (istd::IChangeable)

int CFeatureContainer::GetSupportedOperations() const
{
    return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CFeatureContainer::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
    istd::CChangeNotifier changeNotifier(this);

    bool retVal = BaseClass::CopyFrom(object, mode);
    if (!retVal){
        return false;
    }

    const CFeatureContainer* sourcePtr = dynamic_cast<const CFeatureContainer*>(&object);
    if (sourcePtr != nullptr){
        m_packageId = sourcePtr->m_packageId;
        return true;
    }

    return false;
}


bool CFeatureContainer::IsEqual(const IChangeable& object) const
{
    const CFeatureContainer* sourcePtr = dynamic_cast<const CFeatureContainer*>(&object);
    if (sourcePtr != nullptr){
        return m_packageId == sourcePtr->m_packageId;
    }

    return false;
}


bool CFeatureContainer::ResetData(CompatibilityMode mode)
{
    if (!BaseClass::ResetData(mode)){
        return false;
    }

    m_packageId.clear();

    return true;
}


} // namespace imtlic


