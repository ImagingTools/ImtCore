#include <imttest/CTestMetaInfo.h>


// ImtCore includes
#include <imttest/ITestInfo.h>


namespace imttest
{


// reimplemented (idoc::IDocumentMetaInfo)

QString CTestMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case imttest::ITestInfo::MIT_TEST_ID:
		return QObject::tr("Test Id");
	case imttest::ITestInfo::MIT_TEST_NAME:
		return QObject::tr("Test Name");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


