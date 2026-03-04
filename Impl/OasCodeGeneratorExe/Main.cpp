// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


