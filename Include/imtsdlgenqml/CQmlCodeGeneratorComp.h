// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdlgenqml
{

/**
	A base QML generator of SDL types
	\todo make a base class!
*/
class CQmlCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN_MULTI_0(m_codeGeneratorExtenderListCompPtr, "CodeGeneratorExtenderList", "Extenders, used to generate an additional code", false)
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unions used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList");

	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginQmlFile(const imtsdl::CSdlType& sdlType);
	bool EndQmlFile(const imtsdl::CSdlType& sdlType);
	void AbortCurrentProcessing();

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_MULTIREF(iproc::IProcessor, m_codeGeneratorExtenderListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);

	istd::TDelPtr<QFile> m_qmlFilePtr;
};


} // namespace imtsdlgenqml

