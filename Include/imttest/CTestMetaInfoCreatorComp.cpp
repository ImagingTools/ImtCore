// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CTestMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imttest/ITestInfo.h>
#include <imttest/CTestMetaInfo.h>


namespace imttest
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CTestMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
		if (m_objectTypeIdsAttrPtr.FindValue(typeId) < 0){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CTestMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const ITestInfo* testPtr = dynamic_cast<const ITestInfo*>(dataPtr);
	if (testPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(ITestInfo::MIT_TEST_ID, testPtr->GetTestId());
	metaInfoPtr->SetMetaInfo(ITestInfo::MIT_TEST_NAME, testPtr->GetTestName());

	return true;
}


} // namespace imtlic


