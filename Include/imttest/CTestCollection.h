#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imttest/ITestInfoProvider.h>
#include <imttest/CTestInfo.h>


namespace imttest
{


/**
	\ingroup TestManagement
*/
class CTestCollection: public imtbase::TAggergatedObjectCollectionWrap<imttest::ITestInfoProvider, CTestInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imttest::ITestInfoProvider, CTestInfo> BaseClass;
	
	CTestCollection();

	// reimplemented (imtbase::ITestInfoProvider)
	virtual const imtbase::ICollectionInfo& GetTestList() const override;
	virtual const imttest::ITestInfo* GetTest(const QByteArray& instanceId) const override;
};


} // namespace imttest


