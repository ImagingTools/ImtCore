// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlTools.h>


namespace iprm
{
class IOptionsList;
}


namespace imtsdlgencpp
{


/**
	Extends the class (file) by adding inheritance
*/
class CBaseClassExtenderComp:
			public iproc::CSyncProcessorCompBase,
			protected imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CBaseClassExtenderComp)
	I_END_COMPONENT;

	// reimplemented (iproc::IProcessor)
	/**
		Adds inheritance to class files
		\param paramsPtr (required)
			- (required) [HeaderFile]				of \c ifile::IFileNameParam		- header file to modify
			- (optional) [AddBaseComponentMacro]	of \c iprm::IEnableableParam	-- if enabled adds 'I_BEGIN_BASE_COMPONENT' and 'I_END_COMPONENT' in HeaderFile
						\note if enabled - base class - is FIRST in \c inputPtr
		\param inputPtr - \c iprm::IOptionsList (required)
							- [id] - class name
							- [name] - include directive
		\param outputPtr - unused
		\param progressManagerPtr - unused
	 */
	virtual iproc::IProcessor::TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected:
	virtual bool ProcessHeaderClassFile(
				const iprm::IParamsSet& paramsPtr,
				const iprm::IOptionsList& baseClassList);
	virtual bool BeginClassFiles();
	virtual bool EndClassFiles();
	virtual bool CloseFiles();
	virtual void AbortCurrentProcessing();

	virtual void AddInheritance(QTextStream& ifStream, QTextStream& ofStream, const QStringList& classNameList);
	virtual void WriteBufferToStream(QTextStream& stream, QStringList& buffer);

protected:
	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_originalHeaderFilePtr;
};


} // namespace imtsdlgencpp

