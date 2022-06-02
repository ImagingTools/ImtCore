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
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CTestMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imttest::ITestInfo* testPtr = dynamic_cast<const imttest::ITestInfo*>(dataPtr);
	if (testPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(imttest::ITestInfo::MIT_TEST_ID, testPtr->GetTestId());
	metaInfoPtr->SetMetaInfo(imttest::ITestInfo::MIT_TEST_NAME, testPtr->GetTestName());

	return true;
}


} // namespace imtlic


