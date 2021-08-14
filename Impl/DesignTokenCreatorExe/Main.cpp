// ACF includes
#include <ibase/IApplication.h>
#include <iprm/IParamsSet.h>

// progect includes
#include <designtokencreator/CDesignTokenArgumentsParser.h>

#include <GeneratedFiles/DesignTokenCreatorExe/CDesignTokenCreatorExe.h>


int main(int argc, char *argv[])
{
	CDesignTokenCreatorExe instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


