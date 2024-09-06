#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <iproc/TSyncProcessorCompBase.h>
#include <icomp/TMakeComponentWrap.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CGqlExtSchemaParser.h>


// forward declaration

namespace iprm
{

class IOptionsManager;

} // namespace iprm


namespace imtsdl
{


class CGqlSchemaParserComp:
			public ilog::TLoggerCompWrap<icomp::TMakeComponentWrap<CGqlExtSchemaParser>>,
			public iproc::TSyncProcessorWrap<iproc::IProcessor>
{
public:
	static const QByteArray s_processedFilesParamId;
	static const QByteArray s_typeListParamId;
	static const QByteArray s_requestListParamId;
	static const QByteArray s_documentTypeListParamId;

public:
	typedef ilog::TLoggerCompWrap<icomp::TMakeComponentWrap<CGqlExtSchemaParser>> BaseClass;
	typedef iproc::TSyncProcessorWrap<iproc::IProcessor> BaseClass2;

	I_BEGIN_COMPONENT(CGqlSchemaParserComp)
		I_REGISTER_INTERFACE(iproc::IProcessor)
		I_REGISTER_INTERFACE(ISdlTypeListProvider)
		I_REGISTER_INTERFACE(ISdlRequestListProvider)
		I_REGISTER_INTERFACE(ISdlDocumentTypeListProvider)
		I_ASSIGN(m_useFilesImportAttrPtr, "UseJavaStyleImport", "If enabled, all imports will be procesed by resloving paths in file a system", true, true)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", false, "ArgumentParser")
		I_ASSIGN(m_fileSchemaParserCompFactPtr, "FileSchemaParserFactory", "Factory used to create shema parser", false, "FileSchemaParserFactory")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	/**
		\param paramsPtr - unused
		\param inputPtr (optional) of \c ifile::IFileNameParam if set, use it to parse schema or from \c ArgumentParser otherwise
		\param outputPtr - (optional) of \c iprm::CParamsSet
						- [ProcessedFiles]	(\c s_processedFilesParamId)	of \c iprm::IOptionsManager if set, will contain list of processed files(absolute path list)
						- [TypeList]		(\c s_typeListParamId)			of \c iprm::CParamsSet of \c CSdlType - SDL type list extracted from schema
						- [RequestList]		(\c s_requestListParamId)		of \c iprm::CParamsSet of \c CSdlRequest - SDL request list extracted from schema
						- [DocumentTypeList](\c s_documentTypeListParamId)	of \c iprm::CParamsSet of \c CSdlDocumentType - SDL document type list extracted from schema
		\param progressManagerPtr - unused
	 */
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected:
	QStringList GetPathsFromImportEntry(QString importEntry, const QStringList& searchPathList) const;
	QString FindFileInList(const QString& relativePath, const QStringList& searchPathList) const;
	QStringList FindFilesFromDir(const QString& relativeDirPath, const QStringList& searchPathList) const;
	bool ExtractTypesFromImport(const QStringList& importFilesList);

	virtual bool ProcessFilesImports();
	virtual bool ProcessJavaStyleImports();

	// reimplemented (CGqlExtSchemaParser)
	virtual bool ProcessSchemaImports() override;


private:
	I_ATTR(bool, m_useFilesImportAttrPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_FACT(iproc::IProcessor, m_fileSchemaParserCompFactPtr);
	istd::TDelPtr<QFile> m_currentInputFilePtr;
	istd::TDelPtr<imtbase::CTempDir> m_tempDirPtr;
	iprm::IOptionsManager* m_processedFilesPtr;
	QStringList m_includePathList;
	QString m_currentSchemaFilePath;
};


} // namespace imtsdl

