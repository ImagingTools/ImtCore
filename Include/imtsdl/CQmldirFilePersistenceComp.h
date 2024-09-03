#pragma once


// Qt includes
#include <QtCore/QFileInfo>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFilePersistenceInfo.h>
#include <iprm/IParamsManager.h>


namespace imtsdl
{


class CQmldirFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence,
			virtual public ifile::IFilePersistenceInfo
{
public:
	// common param IDs
	/// defines an ID of the "module name" parameter, represented as \c iprm::INameParam
	const static QByteArray s_moduleNameParamId;
	/// defines an ID of the "Objects" parameter, represented as ilst of parameters i.e. \c iprm::IParamsManager of \c iprm::IParamsSet
	const static QByteArray s_objectsParamId;

	// object param IDs
	/// defines an ID of the "singleton" parameter, represented as \c ipem::IEnableableParam
	const static QByteArray s_objectIsSingletonParamId;
	/// defines an ID of the "type name" parameter, represented as \c iprm::INameParam
	const static QByteArray s_objectTypeNameParamId;
	/// defines an ID of the "object version name" parameter, represented as \c iprm::INameParam
	const static QByteArray s_objectVersionNameParamId;
	/// defines an ID of the "object file name" parameter - is a relative file path, represented as \c iprm::INameParam
	const static QByteArray s_objectFileNameParamId;

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQmldirFilePersistenceComp)
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo)
		I_REGISTER_INTERFACE(ifile::IFilePersistence)
		I_REGISTER_INTERFACE(ifile::IFilePersistenceInfo)

		I_ASSIGN(m_paramsManagerFactComp, "ParamsFactory", "Simple params set factory. Used to create object's entries", true, "ParamsFactory")
	I_END_COMPONENT

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
		const istd::IChangeable* dataObjectPtr,
		const QString* filePathPtr = NULL,
		int flags = -1,
		bool beQuiet = true) const override;
	virtual int LoadFromFile(
		istd::IChangeable& data,
		const QString& filePath = QString(),
		ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual int SaveToFile(
		const istd::IChangeable& data,
		const QString& filePath = QString(),
		ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const override;

	// reimplemented (ifile::IFilePersistenceInfo)
	virtual QString GetLastFilePath(OperationType operationType = OT_UNKNOWN, PathType pathType = PT_COMPLETE) const override;

protected:
	QString GetPathForType(const QFileInfo& fileInfo, PathType pathType) const;

private:
	I_FACT(iprm::IParamsManager, m_paramsManagerFactComp);

	mutable QFileInfo m_lastOpenInfo;
	mutable QFileInfo m_lastSaveInfo;
};


} // namespace imtsdl
