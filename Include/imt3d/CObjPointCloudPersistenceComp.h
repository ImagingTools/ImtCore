#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>


namespace imt3d
{


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
};


} // namespace imt3d


