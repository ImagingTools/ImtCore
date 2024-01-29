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
	The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel
	Extends generated classes by add methods, allows to save and load it from/to \c imtbase::CTreeItemModel
	\todo make a base class based on this (typical management, files, dirs, error handling...)
 */
class CSdlClassTreeModelModificatorComp:
			public iproc::CSyncProcessorCompBase,
			public CGqlSchemaParser,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef CGqlSchemaParser BaseClass2;

	I_BEGIN_COMPONENT(CSdlClassTreeModelModificatorComp)
		I_ASSIGN(m_processorModificatorName, "ProcessModificatorName", "The name, allows to do processing, received from arguments in 'modificators' section", true, "TreeModelAccess")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool BeginClassFiles(const CSdlType& sdlType);
	bool BeginHeaderClassFile(const CSdlType& sdlType);
	bool BeginSourceClassFile(const CSdlType& sdlType);
	bool EndClassFiles(const CSdlType& sdlType);
	bool CloseFiles();
	void AbortCurrentProcessing();
	void AddFieldWriteToModelCode(QTextStream& stream, const CSdlField& field);
	void AddFieldReadFromModelCode(QTextStream& stream, const CSdlField& field);

private:
	I_ATTR(QString, m_processorModificatorName);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;

	istd::TDelPtr<QFile> m_originalHeaderFilePtr;
	istd::TDelPtr<QFile> m_originalSourceFilePtr;
};


} // namespace imtsdl

