// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <GeneratedFiles/SdlCodeGenerator/CSdlCodeGenerator.h>


int main(int argc, char *argv[])
{
	CSdlCodeGenerator instance;

	imtsdl::ISdlProcessArgumentsParser* argumentsParserPtr = instance.GetInterface<imtsdl::ISdlProcessArgumentsParser>();
	if (argumentsParserPtr == nullptr){
		qFatal("Component configuration error: ISdlProcessArgumentsParser component is not provided/exported");

		return 1;
	}

	if (!argumentsParserPtr->SetArguments(argc, argv)){
		qFatal("SdlProcessArgumentsParser can't parse arcuments");

		return 2;
	}

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


