// ACF includes
#include <ibase/IApplication.h>

// ImtCore includes
#include <GeneratedFiles/OasCodeGeneratorExe/COasCodeGeneratorExe.h>


int main(int argc, char *argv[])
{
	COasCodeGeneratorExe instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


