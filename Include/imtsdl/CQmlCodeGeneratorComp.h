#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CGqlSchemaParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A base QML generator of SDL types
*/
class CQmlCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			public CGqlSchemaParser,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef CGqlSchemaParser BaseClass2;

	I_BEGIN_COMPONENT(CQmlCodeGeneratorComp)
		I_ASSIGN(m_qmlKeysFilePrefixAttrPtr, "QmlKeysFilePrefix", "QML Keys file prefix for file name", true, "")
		I_ASSIGN(m_qmlKeysFileSuffixAttrPtr, "QmlKeysFileSuffix", "QML Keys file suffix for file name.\nNote: defauld value is in upper keys to avoid ambiguity", true, "KEYS")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN_MULTI_0(m_codeGeneratorExtenderListCompPtr, "CodeGeneratorExtenderList", "Extenders, used to generate an additional code", false)
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginQmlFile(const CSdlType& sdlType);
	bool EndQmlFile(const CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GetQmlKeysWrappedName(const QString& originalName) const;

private:
	I_ATTR(QString, m_qmlKeysFilePrefixAttrPtr);
	I_ATTR(QString, m_qmlKeysFileSuffixAttrPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_MULTIREF(iproc::IProcessor, m_codeGeneratorExtenderListCompPtr);

	istd::TDelPtr<QFile> m_qmlFilePtr;
	istd::TDelPtr<QFile> m_qmlKeysFilePtr;
};


} // namespace imtsdl

