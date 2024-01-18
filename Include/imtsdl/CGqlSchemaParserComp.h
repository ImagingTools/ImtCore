#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
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
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	istd::TDelPtr<QFile> m_currentInputFile;
};


} // namespace imtsdl









