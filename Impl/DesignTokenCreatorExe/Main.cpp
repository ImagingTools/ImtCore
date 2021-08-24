// STL includes
#include <iostream>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtstyle/IDesignTokenArgumentsParser.h>
#include <GeneratedFiles/DesignTokenCreatorExe/CDesignTokenCreatorExe.h>


int main(int argc, char *argv[])
{
	CDesignTokenCreatorExe instance;

	imtstyle::IDesignTokenArgumentsParser* argumentsParser = instance.GetInterface<imtstyle::IDesignTokenArgumentsParser>();
	if (argumentsParser == nullptr){
		std::cerr << "Component configuration failed" << std::endl;
		return -1;
	}
	else {
		argumentsParser->SetArguments(argc, argv);
	}

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


