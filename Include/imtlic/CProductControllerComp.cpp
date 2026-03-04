// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>

// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


// public methods

// reimplemented (imtrest::IBinaryDataProvider)

bool CProductControllerComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
	if (!m_productCollectionCompPtr.IsValid() || !m_featureCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error."), "CProductControllerComp");
		return false;
	}

	imtlic::IProductInfo* productInfoPtr = nullptr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_productCollectionCompPtr->GetObjectData(dataId, dataPtr)){
		productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get product with ID %1.").arg(qPrintable(dataId)), "CProductControllerComp");
		return false;
	}

	QString filePathTmp = QDir::tempPath() + "/" + QUuid::createUuid().toString() + ".xml";
	{
		ifile::CCompactXmlFileWriteArchive writeArchive(filePathTmp, m_versionInfoCompPtr.GetPtr());
		if (!productInfoPtr->Serialize(writeArchive)){
			SendErrorMessage(0, QString("Error when trying to serialize a feature container. Product-ID: %1.").arg(qPrintable(dataId)), "CProductControllerComp");

			return false;
		}
	}

	QFile file(filePathTmp);
	if (file.open(QIODevice::ReadOnly)){
		data = file.readAll();
		file.close();
		QFile::remove(filePathTmp);

		SendInfoMessage(0, QString("The product features have been successfully exported. Product-ID: %1.").arg(qPrintable(dataId)), "CProductControllerComp");

		return true;
	}

	SendErrorMessage(0, QString("Error when trying to export product features. Product-ID: %1.").arg(qPrintable(dataId)), "CProductControllerComp");

	return false;
}


// reimplemented (imtrest::IBinaryDataController)

bool CProductControllerComp::SetData(const QByteArray& data, QByteArray& dataId) const
{
	if (!m_productCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ProductCollection' was not set", "CProductControllerComp");
		return false;
	}

	if (!m_featureCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureCollection' was not set", "CProductControllerComp");
		return false;
	}

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".xml";

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CProductControllerComp");
		return false;
	}

	file.write(data);
	file.close();

	imtlic::CIdentifiableProductInfo productInfo;

	ifile::CCompactXmlFileReadArchive archive(filePathTmp);
	if (!productInfo.Serialize(archive)){
		SendErrorMessage(0, QString("Error when trying to serialize a feature container. Product-ID: %1.").arg(qPrintable(dataId)), "CProductControllerComp");

		return false;
	}

	QByteArray objectUuid = productInfo.GetObjectUuid();

	if (m_productCollectionCompPtr->GetElementIds().contains(objectUuid)){
		return false;
	}

	QByteArray retVal = m_productCollectionCompPtr->InsertNewObject("", "", "", &productInfo, objectUuid);
	if (retVal.isEmpty()){
		return false;
	}

	imtbase::IObjectCollection* featureCollectionPtr = productInfo.GetFeatures();
	if (featureCollectionPtr != nullptr){
		imtbase::ICollectionInfo::Ids featureIds = m_featureCollectionCompPtr->GetElementIds();
		imtbase::ICollectionInfo::Ids productFeatureIds = featureCollectionPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& productFeatureId : productFeatureIds){
			if (!featureIds.contains(productFeatureId)){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (featureCollectionPtr->GetObjectData(productFeatureId, dataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
					if (featureInfoPtr != nullptr){
						QByteArray result = m_featureCollectionCompPtr->InsertNewObject("", "", "", featureInfoPtr, productFeatureId);
						if (result.isEmpty()){
							SendWarningMessage(0, QString("Unable to insert new feature with ID '%1' from product serialization").arg(qPrintable(productFeatureId)), "CProductControllerComp");
						}
					}
				}
			}
		}
	}

	return true;
}


bool CProductControllerComp::RemoveData(const QByteArray& /*dataId*/) const
{
	return false;
}


bool CProductControllerComp::EnsureDataExists(const QByteArray& /*dataId*/) const
{
	return false;
}


} // namespace imtlic


