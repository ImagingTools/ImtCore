// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iproc/TSyncProcessorCompBase.h>
#include <iprm/IOptionsManager.h>
#include <iprm/ITextParam.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CGqlExtSchemaParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


class CGqlSchemaParserComp:
			public ilog::TLoggerCompWrap<icomp::TMakeComponentWrap<CGqlExtSchemaParser>>,
			public iproc::TSyncProcessorWrap<iproc::IProcessor>,
			private CSdlTools
{
public:
	static const QByteArray s_processedFilesParamId;
	static const QByteArray s_typeListParamId;
	static const QByteArray s_requestListParamId;
	static const QByteArray s_documentTypeListParamId;
	static const QByteArray s_enumListParamId;
	static const QByteArray s_unionListParamId;

public:
	typedef ilog::TLoggerCompWrap<icomp::TMakeComponentWrap<CGqlExtSchemaParser>> BaseClass;
	typedef iproc::TSyncProcessorWrap<iproc::IProcessor> BaseClass2;

	I_BEGIN_COMPONENT(CGqlSchemaParserComp)
		I_REGISTER_INTERFACE(iproc::IProcessor)
		I_REGISTER_INTERFACE(ISdlTypeListProvider)
		I_REGISTER_INTERFACE(ISdlEnumListProvider)
		I_REGISTER_INTERFACE(ISdlRequestListProvider)
		I_REGISTER_INTERFACE(ISdlUnionListProvider)
		I_REGISTER_INTERFACE(ISdlDocumentTypeListProvider)
		I_REGISTER_SUBELEMENT(SchemaCustomParams)
		I_REGISTER_SUBELEMENT_INTERFACE(SchemaCustomParams, iprm::IParamsSet, ExtractSchemaCustomParams);
		I_REGISTER_SUBELEMENT(DependentSchemaList)
		I_REGISTER_SUBELEMENT_INTERFACE(DependentSchemaList, iprm::IOptionsManager, ExtractDependentSchemaList);

		I_ASSIGN(m_useFilesImportAttrPtr, "UseJavaStyleImport", "If enabled, all imports will be procesed by resloving paths in file a system", true, true)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_schemaNamespaceCompPtr, "SchemaNamespace", "The namespace of the schema being processed", false, "SchemaNamespace")
		I_ASSIGN(m_fileSchemaParserCompFactPtr, "FileSchemaParserFactory", "Factory used to create shema parser", false, "FileSchemaParserFactory")
	I_END_COMPONENT;

	// reimplemented(iproc::IProcessor)
	virtual TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected:
	QStringList GetPathsFromImportEntry(QString importDirective, const QStringList& searchPathList) const;
	QString FindFileInList(const QString& relativePath, const QStringList& searchPathList) const;
	QStringList FindFilesFromDir(const QString& relativeDirPath, const QStringList& searchPathList) const;
	bool ExtractTypesFromImport(const QStringList& importFilesList);
	bool SetupSchemaFilePath(const istd::IPolymorphic* inputPtr);

	virtual bool ProcessFilesImports();
	virtual bool ProcessJavaStyleImports();

	// reimplemented (CGqlExtSchemaParser)
	virtual bool ProcessSchemaImports() override;
	virtual bool ValidateSchema() override;


private:
	// static template methods for sub element access
	template <class InterfaceType>
	static InterfaceType* ExtractSchemaCustomParams(CGqlSchemaParserComp& component)
	{
		return component.m_schemaParamsPtr.get();
	}
	template <class InterfaceType>
	static InterfaceType* ExtractDependentSchemaList(CGqlSchemaParserComp& component)
	{
		return component.m_processedFilesPtr;
	}

protected:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);

private:
	I_ATTR(bool, m_useFilesImportAttrPtr);
	I_REF(iprm::ITextParam, m_schemaNamespaceCompPtr);
	I_FACT(iproc::IProcessor, m_fileSchemaParserCompFactPtr);

	istd::TDelPtr<QFile> m_currentInputFilePtr;
	istd::TDelPtr<imtbase::CTempDir> m_tempDirPtr;
	iprm::IOptionsManager* m_processedFilesPtr;
	QStringList m_includePathList;
	QString m_currentSchemaFilePath;
};


} // namespace imtsdl


