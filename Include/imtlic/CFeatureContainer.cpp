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

			QByteArrayList subfeaturesIds = featurePtr->GetSubFeatureIds();
			if (subfeaturesIds.contains(featureId)){
				return featurePtr->GetSubFeature(featureId);
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


QString CFeatureContainer::GetPackageName() const
{
	return m_packageName;
}


void CFeatureContainer::SetPackageName(const QString& packageName)
{
	if (m_packageName != packageName){
		istd::CChangeNotifier notifier(this);

		m_packageName = packageName;
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


// reimplemented (iser::ISerializable)

bool CFeatureContainer::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag packageIdTag("PackageId", "ID of the feature package", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(packageIdTag);
	retVal = retVal && archive.Process(m_packageId);
	retVal = retVal && archive.EndTag(packageIdTag);

	static iser::CArchiveTag packageNameTag("PackageName", "Name of the feature package", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(packageNameTag);
	retVal = retVal && archive.Process(m_packageName);
	retVal = retVal && archive.EndTag(packageNameTag);

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
		m_packageName = sourcePtr->m_packageName;
		return true;
	}

	return false;
}


bool CFeatureContainer::IsEqual(const IChangeable& object) const
{
	bool retVal = BaseClass::IsEqual(object);
	const CFeatureContainer* sourcePtr = dynamic_cast<const CFeatureContainer*>(&object);
	if (sourcePtr != nullptr && retVal){
		return m_packageId == sourcePtr->m_packageId && m_packageName == sourcePtr->m_packageName ;
	}

	return false;
}


bool CFeatureContainer::ResetData(CompatibilityMode mode)
{
	if (!BaseClass::ResetData(mode)){
		return false;
	}

	m_packageId.clear();
	m_packageName.clear();

	return true;
}


} // namespace imtlic


