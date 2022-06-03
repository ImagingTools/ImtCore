#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imttest/CTestInfoComp.h>
#include <imttest/CTestCollection.h>
#include <imttest/CTestMetaInfoCreatorComp.h>
#include <imttest/CTestDatabaseDelegateComp.h>


/**
    ImtTestPck package
*/
namespace ImtTestPck
{

typedef icomp::TModelCompWrap<imttest::CTestInfoComp> TestInfo;
typedef icomp::TModelCompWrap<
            icomp::TMakeComponentWrap<
                        imttest::CTestCollection,
                        imttest::ITestInfoProvider,
                        imttest::ITestInfo,
                        iser::ISerializable>> TestCollection;
typedef imttest::CTestMetaInfoCreatorComp TestMetaInfoCreator;
typedef imttest::CTestDatabaseDelegateComp TestDatabaseDelegate;


} // namespace ImtTestPck


