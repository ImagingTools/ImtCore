#include <imt3d/CObjPointCloudPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imt3d
{


// reimplemented (ifile::IFilePersistence)

bool CObjPointCloudPersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool /*beQuiet*/) const
{
	if ((filePathPtr != NULL) && (flags & QF_LOAD)){
		QFile stlFile(*filePathPtr);

		if (!stlFile.exists()){
			return false;
		}

		if (!stlFile.open(QIODevice::ReadOnly)){
			return false;
		}
	}

	if (dataObjectPtr != NULL){
		const CPointCloud3d* documentPtr = dynamic_cast<const CPointCloud3d*>(dataObjectPtr);
		if (documentPtr == NULL){
			return false;
		}
	}

	return true;
}


int CObjPointCloudPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	istd::CChangeGroup changeGroup(&data);

	data.ResetData();

	CPointCloud3d* documentPtr = dynamic_cast<CPointCloud3d*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return OS_FAILED;
	}

	QTextStream stream(&file);

	QVector<float> pointComponents;

	while (!stream.atEnd()){
		QString line = stream.readLine();

		QStringList components = line.split(" ", QString::SkipEmptyParts);
		if (components.count() >= 4){

			QString valueKey = components[0].simplified();
			if (valueKey == "v"){
				bool ok = false;

				double x = components[1].toDouble(&ok);
				if (!ok){
					continue;
				}

				double y = components[2].toDouble(&ok);
				if (!ok){
					continue;
				}

				double z = components[3].toDouble(&ok);
				if (!ok){
					continue;
				}

				pointComponents.push_back(x);
				pointComponents.push_back(y);
				pointComponents.push_back(z);
			}
		}
	}

	file.close();

	int pointsCount = pointComponents.size() / 3;

	float* pointsDataPtr = new float[pointComponents.size()];
	memcpy(pointsDataPtr, pointComponents.constData(), pointComponents.size() * sizeof(float));

	return documentPtr->CreateCloud(IPointCloud3d::PF_XYZF, pointsCount, pointsDataPtr, true) ? OS_OK : OS_FAILED;
}


int CObjPointCloudPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const CPointCloud3d* documentPtr = dynamic_cast<const CPointCloud3d*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("File could not be written: '%1'").arg(filePath));

		return OS_FAILED;
	}

	QTextStream stream(&file);

	CALL_TEMPLATED_POINTCLOUD_METHOD(documentPtr, SaveToFileHelper, OS_FAILED, *documentPtr, stream);
}


// reimplemented (ifile::IFileTypeInfo)

bool CObjPointCloudPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("obj");

	return true;
}


QString CObjPointCloudPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "3D-object files";
}


} // namespace imt3d


