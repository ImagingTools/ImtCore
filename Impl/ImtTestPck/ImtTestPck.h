// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imttest/CTestInfo.h>
#include <imttest/CTestMetaInfoCreatorComp.h>
#include <imttest/CTestDatabaseDelegateComp.h>


/**
    ImtTestPck package
*/
namespace ImtTestPck
{

typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imttest::CTestInfo,
						imttest::ITestInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> TestInfo;
typedef imttest::CTestMetaInfoCreatorComp TestMetaInfoCreator;
typedef imttest::CTestDatabaseDelegateComp TestDatabaseDelegate;


} // namespace ImtTestPck


