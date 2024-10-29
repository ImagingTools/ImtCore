#include <imtgql/Test/CGqlHandlerTest.h>


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


// protected methods

bool CGqlHandlerTest::VerifyResponse(const imtbase::CTreeItemModel& actualModel, const imtbase::CTreeItemModel& expectedModel) const
{
	return actualModel.IsEqualWithModel(&expectedModel);
}


ipackage::CComponentAccessor* CGqlHandlerTest::GetComponentAccessor() const
{
	if (!m_componentAccessorPtr.IsValid()){
		m_componentAccessorPtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	}

	return m_componentAccessorPtr.GetPtr();
}


// private slots

void CGqlHandlerTest::initTestCase()
{
}


void CGqlHandlerTest::CreateInternalResponseTest(){
	ipackage::CComponentAccessor* componentAccessorPtr = GetComponentAccessor();
	if (componentAccessorPtr == nullptr){
		QFAIL("Get interface from composite component failed");
	}

	imtgql::IGqlRequestHandler* gqlRequestHandlerPtr = componentAccessorPtr->GetComponentInterface<imtgql::IGqlRequestHandler>();
	if (gqlRequestHandlerPtr == nullptr){
		QFAIL("GraphQL request handler is invalid");
	}

	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr = CreateGqlRequest();
	if (!gqlRequestPtr.IsValid()){
		QFAIL("GraphQL request is invalid");
	}

	istd::TDelPtr<imtbase::CTreeItemModel> expectedModelPtr = CreateExpectedModel();
	if (!expectedModelPtr.IsValid()){
		QFAIL("Expected model is invalid");
	}

	QString errorMessage;
	istd::TDelPtr<imtbase::CTreeItemModel> responseModelPtr = gqlRequestHandlerPtr->CreateResponse(*gqlRequestPtr.GetPtr(), errorMessage);
	if (!responseModelPtr.IsValid()){
		QFAIL("Response model is invalid");
	}

	qDebug() << "expectedModel" << expectedModelPtr->ToJson();
	qDebug() << "responseModel" << responseModelPtr->ToJson();

	QVERIFY(VerifyResponse(*responseModelPtr, *expectedModelPtr));
}


void CGqlHandlerTest::cleanupTestCase()
{
}


} // namespace imtgql


