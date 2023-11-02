#include <imtlicgql/CFeaturesExportProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CFeatureContainer.h>
#include <ifile/CCompactXmlFileWriteArchive.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

bool CFeaturesExportProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
	if (!m_productCollectionCompPtr.IsValid() || !m_featureCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error."), "CFeaturesExportProviderComp");
		return false;
	}

	const imtlic::IProductInfo* productInfoPtr = nullptr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_productCollectionCompPtr->GetObjectData(dataId, dataPtr)){
		productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get product with ID %1.").arg(qPrintable(dataId)), "CFeaturesExportProviderComp");
		return false;
	}

	imtlic::CFeatureContainer featureContainer;

	QByteArrayList featureUuids = productInfoPtr->GetFeatureIds();
	for (const QByteArray& featureUuid : featureUuids){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_featureCollectionCompPtr->GetObjectData(featureUuid, dataPtr)){
			const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				if (featureContainer.InsertNewObject(QByteArray("FeatureInfo"), "", "", featureInfoPtr, featureUuid) == QByteArray("")){
					SendWarningMessage(0, QString("Unable to insert feature %1 to the container.").arg(qPrintable(featureUuid)), "CFeaturesExportProviderComp");
				}
			}
		}
	}

	QString filePathTmp = QDir::tempPath() + "/"  + QUuid::createUuid().toString() + ".xml";
	{
		ifile::CCompactXmlFileWriteArchive writeArchive(filePathTmp);
		if (!featureContainer.Serialize(writeArchive)){
			SendErrorMessage(0, QString("Error when trying to serialize a feature container. Product-ID: %1.").arg(qPrintable(dataId)), "CFeaturesExportProviderComp");

			return false;
		}
	}

	QFile file(filePathTmp);
	if (file.open(QIODevice::ReadOnly)){
		data = file.readAll();
		file.close();
		QFile::remove(filePathTmp);

		SendInfoMessage(0, QString("The product features have been successfully exported. Product-ID: %1.").arg(qPrintable(dataId)), "CFeaturesExportProviderComp");

		return true;
	}

	SendErrorMessage(0, QString("Error when trying to export product features. Product-ID: %1.").arg(qPrintable(dataId)), "CFeaturesExportProviderComp");

	return false;
}


} // namespace imtqml





