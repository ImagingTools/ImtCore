// Qt includes
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

// ACF includes
#include <ibase/IApplication.h>
#include <GeneratedFiles/ServerDemo/CServerDemo.h>


int main(int argc, char *argv[])
{
    CServerDemo instance;

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


