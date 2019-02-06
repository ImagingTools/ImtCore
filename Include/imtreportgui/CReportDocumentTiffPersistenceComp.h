#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtreportgui/CReportSceneBuilder.h>


namespace imtreportgui
{


class CReportDocumentTiffPersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CReportDocumentTiffPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = NULL,
				int flags = -1,
				bool beQuiet = true) const override;

	int LoadFromFile(istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;

	int SaveToFile(const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const override;
	QString GetTypeDescription(const QString* extensionPtr = NULL) const override;

private:
	OperationState SaveToFile(const CReportSceneBuilder::ReportScenes scenes, const QString& filePath) const;
};


} // namespace imtreportgui


