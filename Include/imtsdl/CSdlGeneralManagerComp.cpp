#include "CSdlGeneralManagerComp.h"


// ACF includes
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>


namespace imtsdl
{


void CSdlGeneralManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_sdlParserCompPtr.IsValid());

	// parse schema
	int parsingResult = m_sdlParserCompPtr->DoProcessing(nullptr, nullptr, nullptr);
	if (parsingResult != iproc::IProcessor::TS_OK){
		SendErrorMessage(0, "Unable to parse schema");

		::exit(-1);
	}

	// create code
	const int sdlProcessorsCount = m_sdlProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendErrorMessage(0, "Unable to process");

				::exit(-1);
			}
		}
	}

	::exit(0);
}



} // namespace imtsdl

