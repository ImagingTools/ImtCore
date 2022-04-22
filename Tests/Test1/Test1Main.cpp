#include <itest/CStandardTestExecutor.h>


// Qt includes
#include <QtWidgets/QApplication>

// ACF includes
#include <ipackage/CComponentAccessor.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


int main(int argc, char *argv[])
{
	QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	QString registryFile = imtCorePath + "/Tests/Partitura/ImtTestVoce.arp/Test1.acc";
	QString configFile = imtCorePath + "/Config/ImtCoreTest.awc";
	ipackage::CComponentAccessor accessor(registryFile, configFile);

	imtbase::IObjectCollection* objectCollectionPtr = nullptr;
	objectCollectionPtr = accessor.GetComponentInterface<imtbase::IObjectCollection>();

	QApplication app(argc, argv);
	itest::CStandardTestExecutor instance;
	return instance.RunTests(argc, argv);
}

