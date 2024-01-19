#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CGqlSchemaParser.h>


namespace imtsdl
{


class CGqlSchemaParserComp:
			public iproc::CSyncProcessorCompBase,
			public CGqlSchemaParser
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef CGqlSchemaParser BaseClass2;

	I_BEGIN_COMPONENT(CGqlSchemaParserComp)
		I_REGISTER_INTERFACE(ISdlTypeListProvider)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	istd::TDelPtr<QFile> m_currentInputFilePtr;
};


} // namespace imtsdl









