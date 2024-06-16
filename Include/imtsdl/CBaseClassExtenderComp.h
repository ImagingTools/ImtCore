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


namespace iprm
{
class IOptionsList;
}


namespace imtsdl
{


/**
	The base class extender
*/
class CBaseClassExtenderComp:
			public iproc::CSyncProcessorCompBase,
			protected CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CBaseClassExtenderComp)
	I_END_COMPONENT;

	// reimplemented (iproc::IProcessor)
	/**
		Adds
		\param paramsPtr (required)
			- (optional) [IncludePaths]		of \c iprm::IOptionsList		- [id] - directory to lookup source files
			- (required) [HeaderFile]		of \c iprm::IFileNameParam		- header file to modify
			- (optional) [SourceFile]		of \c iprm::IFileNameParam		- source file to modify
			- (optional) [Override]			of \c iprm::IEnableableParam -	- if enabled add override of all virtual methods if SourceFile is set - adds in SourceFile and in HeaderFile otherwise
		\param inputPtr - \c iprm::IOptionsList (required)
							- [id] - class name
							- [name] - include directive
		\param outputPtr - unused
		\param progressManagerPtr - unused
	 */
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected:
	virtual bool ProcessHeaderClassFile(
				const iprm::IParamsSet* paramsPtr,
				const iprm::IOptionsList* baseClassList);
	virtual bool ProcessSourceClassFile(
				const iprm::IParamsSet* paramsPtr,
				const iprm::IOptionsList* baseClassList);
	virtual bool BeginClassFiles();
	virtual bool EndClassFiles();
	virtual bool CloseFiles();
	virtual void AbortCurrentProcessing();

protected:
	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;

	istd::TDelPtr<QFile> m_originalHeaderFilePtr;
	istd::TDelPtr<QFile> m_originalSourceFilePtr;
};


} // namespace imtsdl

