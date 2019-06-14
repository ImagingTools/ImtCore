#pragma once


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


namespace imt3d
{


/**
	Class is meant to load/save a pointcloud object from/into standard obj-format.
	Because of the obj-format restrictions, only first 3 point components (point coordinates) will be saved/loaded.
	For the same reason class is able to load only PF_FLOAT32 point format.
*/
class CObjPointCloudPersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObjPointCloudPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = NULL,
				int flags = -1,
				bool beQuiet = true) const;
	virtual int LoadFromFile(istd::IChangeable& data, 
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const;
	virtual int SaveToFile(const istd::IChangeable& data, 
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const;

private:
	template <typename DataType> int SaveToFileHelper(const CPointCloud3d& pointCloud, QTextStream& stream) const;
};


template<typename DataType>
inline int CObjPointCloudPersistenceComp::SaveToFileHelper(const CPointCloud3d& pointCloud, QTextStream& stream) const
{
	for (int i = 0; i < pointCloud.GetPointsCount(); ++i){
		const DataType* pointDataPtr = static_cast<const DataType*>(pointCloud.GetPointData(i));
		if (!pointDataPtr){
			return OS_FAILED;
		}

		QString textLine;
		textLine = QString("v %1 %2 %3").arg(pointDataPtr->xyz[0]).arg(pointDataPtr->xyz[1]).arg(pointDataPtr->xyz[2]);

		stream << textLine << "\n";
	}

	return OS_OK;
}

} // namespace imt3d


