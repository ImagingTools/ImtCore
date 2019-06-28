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

	std::vector<imt3d::IPointCloud3d::PointXyz32> points;

	while (!stream.atEnd()){
		QString line = stream.readLine();

		QStringList components = line.split(" ", QString::SkipEmptyParts);
		if (components.count() >= 4){

			QString valueKey = components[0].simplified();
			if (valueKey == "v"){
				bool ok = false;

				float x = components[1].toFloat(&ok);
				if (!ok){
					continue;
				}

				float y = components[2].toFloat(&ok);
				if (!ok){
					continue;
				}

				float z = components[3].toFloat(&ok);
				if (!ok){
					continue;
				}

				imt3d::IPointCloud3d::PointXyz32 point;
				point.data[0] = x;
				point.data[1] = y;
				point.data[2] = z;

				points.push_back(point);
			}
		}
	}

	file.close();

	int pointsCount = static_cast<int>(points.size());

	imt3d::IPointCloud3d::PointXyz32* pointsDataPtr = new imt3d::IPointCloud3d::PointXyz32[pointsCount];
	memcpy(pointsDataPtr, points.data(), pointsCount * sizeof(imt3d::IPointCloud3d::PointXyz32));

	return documentPtr->CreateCloud(IPointCloud3d::PF_XYZ_32, pointsCount, pointsDataPtr, true) ? OS_OK : OS_FAILED;
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

	switch (documentPtr->GetPointFormat()){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			SaveToFileHelper<imt3d::IPointCloud3d::PointXyz32>(*documentPtr, stream);
			break;
		case imt3d::IPointCloud3d::PF_XYZ_64:
			SaveToFileHelper<imt3d::IPointCloud3d::PointXyz64>(*documentPtr, stream);
			break;
		case imt3d::IPointCloud3d::PF_XYZW_32:
			SaveToFileHelper<imt3d::IPointCloud3d::PointXyzw32>(*documentPtr, stream);
			break;
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			SaveToFileHelper<imt3d::IPointCloud3d::PointXyzAbc32>(*documentPtr, stream);
			break;
	}

	return OS_OK;
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


template<typename PointType>
void CObjPointCloudPersistenceComp::SaveToFileHelper(const CPointCloud3d& pointCloud, QTextStream& stream) const
{
	for (int i = 0; i < pointCloud.GetPointsCount(); ++i){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(i));
		Q_ASSERT(pointDataPtr != nullptr);

		QString textLine;
		textLine = QString("v %1 %2 %3").arg(pointDataPtr->data[0]).arg(pointDataPtr->data[1]).arg(pointDataPtr->data[2]);

		stream << textLine << "\n";
	}
}


} // namespace imt3d


