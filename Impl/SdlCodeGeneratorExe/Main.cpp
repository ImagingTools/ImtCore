// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		qFatal("Component configuration error: imtsdlgenqml::ISdlProcessArgumentsParser component is not provided/exported");

		return 1;
	}

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr == nullptr){
		qFatal("Component configuration error: IApplication component is not provided/exported");

		return 2;
	}
	applicationPtr->InitializeApplication(argc, argv);

	if (!argumentsParserPtr->SetArguments(argc, argv)){
		qFatal("SdlProcessArgumentsParser can't parse arcuments");

		return 3;
	}

	return applicationPtr->Execute(argc, argv);
}


