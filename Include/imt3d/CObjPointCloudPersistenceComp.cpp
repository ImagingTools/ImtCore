#include <imt3d/CObjPointCloudPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


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

				documentPtr->AddPoint(i3d::CVector3d(x, y, z));
			}
		}
	}

	file.close();

	return OS_OK;
}


int CObjPointCloudPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const CPointCloud3d* documentPtr = dynamic_cast<const CPointCloud3d*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		return OS_FAILED;
	}

	QTextStream stream(&file);

	const imt3d::CloudPoints& points = documentPtr->GetPoints();
	for (imt3d::CloudPoints::ConstIterator iter = points.constBegin(); iter != points.constEnd(); ++iter){
		QString textLine = QString("v %1 %2 %3").arg(iter->GetX()).arg(iter->GetY()).arg(iter->GetZ());

		stream << textLine << "\n";
	}
	
	file.close();

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


} // namespace imt3d


