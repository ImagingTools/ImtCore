#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imttest
{


class ITestInfo;


/**
	Interface for a collection of installed test instances.
	\ingroup TestManagement
*/
class ITestInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of license instance infos given as QStringList.
		*/
		MIT_TEST_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	/**
		Get list of all available test instances.
	*/
	virtual const imtbase::ICollectionInfo& GetTestList() const = 0;

	/**
		Get information for a test installation instance.
	*/
	virtual const imttest::ITestInfo* GetTest(const QByteArray& instanceId) const = 0;
};


} // namespace imttest


