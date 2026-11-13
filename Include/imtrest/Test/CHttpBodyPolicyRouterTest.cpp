// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpBodyPolicyRouterTest.h"


// STL includes
#include <memory>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/TSimComponentWrap.h>

// ImtCore includes
#include <imtrest/CHttpBodyPolicyRouterComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IRequestServlet.h>


namespace
{


typedef imtrest::IHttpRequestBodyPolicy IHttpRequestBodyPolicy;
typedef imtrest::CHttpBodyPolicyRouterComp RouterComp;
typedef icomp::TSimComponentWrap<RouterComp> Router;

const int s_reject = int(IHttpRequestBodyPolicy::BM_REJECT);
const int s_internalServerError = int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR);


int Find(const QByteArrayList& prefixes, const QByteArray& path)
{
	return imtrest::CHttpBodyPolicyRouterComp::FindPrefixIndex(prefixes, path);
}


class CRequestServletStub: virtual public imtrest::IRequestServlet
{
public:
	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& /*request*/, const QByteArray& /*subCommandId*/) const override
	{
		return {};
	}
};


class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	// reimplemented (imtrest::IProtocolEngine)
	QByteArray GetProtocolTypeId() const override
	{
		return QByteArrayLiteral("http");
	}

	const iser::IVersionInfo* GetProtocolVersion() const override
	{
		return nullptr;
	}

	bool GetProtocolStatusCode(int /*statusCode*/, int& /*protocolStatusCode*/, QByteArray& /*statusCodeLiteral*/) const override
	{
		return false;
	}

	imtrest::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& /*requestHandler*/) const override
	{
		return nullptr;
	}

	imtrest::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& /*requestHandler*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}

	imtrest::IResponseUniquePtr CreateResponse(
				const imtrest::IRequest& /*request*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


/**
	Policy that says who it is: its decision has the mark of the policy as the limit of the body and the path of the request in the headers
	of the refusal, so the test sees which policy has decided and that it got the request of the router.
*/
class CMarkedPolicyComp:
			public icomp::CComponentBase,
			virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMarkedPolicyComp);
		I_REGISTER_INTERFACE(imtrest::IHttpRequestBodyPolicy);
		I_ASSIGN(m_markAttrPtr, "Mark", "Number that identifies the policy", true, 0);
	I_END_COMPONENT;

	// reimplemented (imtrest::IHttpRequestBodyPolicy)
	BodyDecision DecideBody(const imtrest::CHttpRequest& requestHead) const override
	{
		BodyDecision decision;
		decision.maxBodySize = *m_markAttrPtr;
		decision.rejectHeaders.insert("path", requestHead.GetCommandId());

		return decision;
	}

private:
	I_ATTR(int, m_markAttrPtr);
};


typedef icomp::TSimSharedComponentPtr<CMarkedPolicyComp> MarkedPolicy;


MarkedPolicy CreateMarkedPolicy(int mark)
{
	MarkedPolicy policy;

	policy->SetIntAttr("Mark", mark);
	policy->InitComponent();

	return policy;
}


/**
	Create the router as the container of the components does: the attributes and the references are set, then the component is created.
	\param	marks			Marks of the policies of the prefixes.
	\param	defaultMark		Mark of the default policy, less than 0 - the default policy is not set.
*/
std::unique_ptr<Router> CreateRouter(const QByteArrayList& prefixes, const QList<int>& marks, int defaultMark)
{
	std::unique_ptr<Router> routerPtr(new Router);

	for (qsizetype i = 0; i < prefixes.size(); ++i){
		routerPtr->InsertMultiAttr("PathPrefixes", prefixes[i]);
		routerPtr->InsertMultiRef("Policies", CreateMarkedPolicy(marks[i]));
	}

	// The reference that is not set: ACF logs it (the critical message "Component DefaultPolicy is defined, but definition component has no parent" of this test is expected)
	if (defaultMark >= 0){
		routerPtr->SetRef("DefaultPolicy", CreateMarkedPolicy(defaultMark));
	}

	routerPtr->InitComponent();

	return routerPtr;
}


/**
	The decision of the router about the request of the path. The head of the request is parsed by a request without the policy.
*/
IHttpRequestBodyPolicy::BodyDecision Decide(const IHttpRequestBodyPolicy& policy, const QByteArray& path)
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.ExecuteHttpParser("GET " + path + " HTTP/1.1\r\nHost: localhost:7380\r\n\r\n", nullptr);

	return policy.DecideBody(request);
}


/**
	Mark of the policy that has decided.
*/
qint64 GetMark(const IHttpRequestBodyPolicy& policy, const QByteArray& path)
{
	return Decide(policy, path).maxBodySize;
}


} // namespace


// private slots

void CHttpBodyPolicyRouterTest::MatchTest_data()
{
	QTest::addColumn<QByteArray>("prefix");
	QTest::addColumn<QByteArray>("path");
	QTest::addColumn<bool>("isMatch");

	QTest::newRow("same path") << QByteArray("/files") << QByteArray("/files") << true;
	QTest::newRow("nested path") << QByteArray("/files") << QByteArray("/files/a/b") << true;
	QTest::newRow("trailing slash of the path") << QByteArray("/files") << QByteArray("/files/") << true;
	QTest::newRow("no slash in the prefix") << QByteArray("files") << QByteArray("/files/a") << true;
	QTest::newRow("slashes in the prefix") << QByteArray("/files/") << QByteArray("/files/a") << true;
	QTest::newRow("nested prefix") << QByteArray("/api/files") << QByteArray("/api/files/a") << true;

	// Only whole segments match
	QTest::newRow("longer segment") << QByteArray("/files") << QByteArray("/files2") << false;
	QTest::newRow("longer segment with a path") << QByteArray("/files") << QByteArray("/files2/a") << false;
	QTest::newRow("shorter path") << QByteArray("/api/files") << QByteArray("/api") << false;
	QTest::newRow("other path") << QByteArray("/files") << QByteArray("/other") << false;
	QTest::newRow("parent path") << QByteArray("/api/files") << QByteArray("/files") << false;
	QTest::newRow("case of the letters") << QByteArray("/files") << QByteArray("/Files") << false;

	// The empty prefix is the prefix of all paths
	QTest::newRow("empty prefix") << QByteArray() << QByteArray("/files") << true;
	QTest::newRow("root prefix") << QByteArray("/") << QByteArray("/files/a") << true;
	QTest::newRow("empty prefix, root path") << QByteArray() << QByteArray("/") << true;
	QTest::newRow("empty prefix, empty path") << QByteArray() << QByteArray() << true;
	QTest::newRow("prefix, root path") << QByteArray("/files") << QByteArray("/") << false;
}


void CHttpBodyPolicyRouterTest::MatchTest()
{
	QFETCH(QByteArray, prefix);
	QFETCH(QByteArray, path);
	QFETCH(bool, isMatch);

	QCOMPARE(Find(QByteArrayList() << prefix, path), isMatch ? 0 : -1);
}


void CHttpBodyPolicyRouterTest::LongestPrefixWinsTest()
{
	const QByteArrayList prefixes = QByteArrayList() << "/api" << "/api/files" << "/api/files/upload" << "/other";

	QCOMPARE(Find(prefixes, "/api/users"), 0);
	QCOMPARE(Find(prefixes, "/api/files"), 1);
	QCOMPARE(Find(prefixes, "/api/files/a"), 1);
	QCOMPARE(Find(prefixes, "/api/files/upload/1"), 2);
	QCOMPARE(Find(prefixes, "/other/a"), 3);
	QCOMPARE(Find(prefixes, "/unknown"), -1);

	// The order of the prefixes does not matter
	const QByteArrayList reversed = QByteArrayList() << "/api/files/upload" << "/api/files" << "/api";

	QCOMPARE(Find(reversed, "/api/files/upload/1"), 0);
	QCOMPARE(Find(reversed, "/api/files/a"), 1);
	QCOMPARE(Find(reversed, "/api/users"), 2);
}


void CHttpBodyPolicyRouterTest::FirstOfEqualPrefixesWinsTest()
{
	// The same prefix with slashes and without them is the same prefix
	const QByteArrayList prefixes = QByteArrayList() << "/files" << "files/" << "/files";

	QCOMPARE(Find(prefixes, "/files/a"), 0);
}


void CHttpBodyPolicyRouterTest::DefaultPrefixTest()
{
	const QByteArrayList prefixes = QByteArrayList() << "/files" << "";

	// The empty prefix is used if nothing else matches
	QCOMPARE(Find(prefixes, "/files/a"), 0);
	QCOMPARE(Find(prefixes, "/other"), 1);
	QCOMPARE(Find(prefixes, "/files2"), 1);
	QCOMPARE(Find(prefixes, "/"), 1);
}


void CHttpBodyPolicyRouterTest::NoPrefixesTest()
{
	QCOMPARE(Find(QByteArrayList(), "/files"), -1);
}


void CHttpBodyPolicyRouterTest::PolicyOfPrefixDecidesTest()
{
	const std::unique_ptr<Router> routerPtr = CreateRouter(QByteArrayList() << "/files" << "/api" << "/api/files", QList<int>() << 1 << 2 << 3, 100);

	QCOMPARE(GetMark(*routerPtr, "/files"), qint64(1));
	QCOMPARE(GetMark(*routerPtr, "/files/a/b"), qint64(1));
	QCOMPARE(GetMark(*routerPtr, "/api/users"), qint64(2));

	// The longest prefix decides
	QCOMPARE(GetMark(*routerPtr, "/api/files/a"), qint64(3));
}


void CHttpBodyPolicyRouterTest::DefaultPolicyDecidesIfNoPrefixMatchesTest()
{
	const std::unique_ptr<Router> routerPtr = CreateRouter(QByteArrayList() << "/files" << "/api", QList<int>() << 1 << 2, 100);

	QCOMPARE(GetMark(*routerPtr, "/other"), qint64(100));
	QCOMPARE(GetMark(*routerPtr, "/"), qint64(100));

	// Only whole segments match
	QCOMPARE(GetMark(*routerPtr, "/files2"), qint64(100));
	QCOMPARE(GetMark(*routerPtr, "/api2/a"), qint64(100));

	// The default policy decides about the same request that the router has got
	QCOMPARE(Decide(*routerPtr, "/other/a").rejectHeaders.value("path"), QByteArray("/other/a"));
}


void CHttpBodyPolicyRouterTest::DefaultPolicyDecidesWithoutPrefixesTest()
{
	const std::unique_ptr<Router> routerPtr = CreateRouter(QByteArrayList(), QList<int>(), 100);

	QCOMPARE(GetMark(*routerPtr, "/files"), qint64(100));
	QCOMPARE(GetMark(*routerPtr, "/"), qint64(100));
}


void CHttpBodyPolicyRouterTest::PrefixWinsOverDefaultPolicyTest()
{
	// The empty prefix is a prefix like the others: it matches all paths, so the default policy is not needed for them
	const std::unique_ptr<Router> routerPtr = CreateRouter(QByteArrayList() << "/files" << "", QList<int>() << 1 << 7, 100);

	QCOMPARE(GetMark(*routerPtr, "/files/a"), qint64(1));
	QCOMPARE(GetMark(*routerPtr, "/other"), qint64(7));
}


void CHttpBodyPolicyRouterTest::MissingDefaultPolicyRefusesTest()
{
	// The default policy is obligatory, but the component is created anyway: the container does not stop the server
	const std::unique_ptr<Router> routerPtr = CreateRouter(QByteArrayList() << "/files", QList<int>() << 1, -1);

	// The request that nobody has chosen the policy for is not received with a policy that is not set
	const IHttpRequestBodyPolicy::BodyDecision decision = Decide(*routerPtr, "/other");

	QCOMPARE(int(decision.mode), s_reject);
	QCOMPARE(decision.rejectStatusCode, s_internalServerError);
	QVERIFY(!decision.sinkPtr);

	// The requests of the prefixes do not need it
	QCOMPARE(GetMark(*routerPtr, "/files/a"), qint64(1));
}


I_ADD_TEST(CHttpBodyPolicyRouterTest);


