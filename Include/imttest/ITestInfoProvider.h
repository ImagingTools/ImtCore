// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual const ITestInfo* GetTest(const QByteArray& instanceId) const = 0;
};


} // namespace imttest


