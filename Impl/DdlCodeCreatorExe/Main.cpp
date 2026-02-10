// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
// STL includes
#include <iostream>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtddl/IDdlCodeCreatorArgumentsParser.h>
#include <GeneratedFiles/DdlCodeCreator/CDdlCodeCreator.h>


int main(int argc, char *argv[])
{
	CDdlCodeCreator instance;

	imtddl::IDdlCodeCreatorArgumentsParser* argumentsParser = instance.GetInterface<imtddl::IDdlCodeCreatorArgumentsParser>();
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


