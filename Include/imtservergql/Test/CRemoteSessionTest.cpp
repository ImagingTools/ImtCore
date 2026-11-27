// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRemoteSessionTest.h"


#include <atomic>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <QtCore/QThread>
#include <icomp/TModelCompWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <imod/CSingleModelObserverBase.h>
#include <iprm/CTextParamComp.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauthgql/CRemoteSessionComp.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/CGqlResponse.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace
{


// Answers the sign-in requests in the order of the script, issuing "token-1", "token-2"..., and keeps what they carried
class CScriptedSignInClientComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CScriptedSignInClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	enum class Answer
	{
		Token,
		// A GraphQL error, like the answer to a wrong password
		Refusal,
		// An answer without a token
		ExpiredPassword,
		NoConnection
	};

	struct SentSignIn
	{
		QString login;
		QString password;
		QByteArray productId;
		QByteArray contextToken;
		QByteArray contextProductId;
	};

	// The answers to the first sign-ins; a token after them
	QList<Answer> answers;
	// When set, the first sign-in waits for it
	QSemaphore* firstSignInGatePtr = nullptr;
	mutable QSemaphore firstSignInStarted;
	// When set, the first sign-in calls it from a nested event loop, as a synchronous request runs the queued calls of its thread
	std::function<void()> duringFirstSignIn;

	QList<SentSignIn> GetSentSignIns() const
	{
		QMutexLocker locker(&m_mutex);

		return m_sentSignIns;
	}

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const override
	{
		SentSignIn sentSignIn;
		const auto* gqlRequestPtr = dynamic_cast<const imtgql::CGqlRequest*>(requestPtr.GetPtr());
		if (gqlRequestPtr != nullptr){
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest authorizationRequest(*gqlRequestPtr, false);
			const auto& input = authorizationRequest.GetRequestedArguments().input;
			if (input.has_value()){
				sentSignIn.login = input->login.has_value() ? *input->login : QString();
				sentSignIn.password = input->password.has_value() ? *input->password : QString();
				sentSignIn.productId = input->productId.has_value() ? *input->productId : QByteArray();
			}
		}
		const imtgql::IGqlContext* contextPtr = requestPtr->GetRequestContext();
		if (contextPtr != nullptr){
			sentSignIn.contextToken = contextPtr->GetToken();
			sentSignIn.contextProductId = contextPtr->GetProductId();
		}

		qsizetype signInIndex = 0;
		{
			QMutexLocker locker(&m_mutex);
			signInIndex = m_sentSignIns.size();
			m_sentSignIns.append(sentSignIn);
		}

		if (signInIndex == 0 && firstSignInGatePtr != nullptr){
			firstSignInStarted.release();
			firstSignInGatePtr->acquire();
		}

		if (signInIndex == 0 && duringFirstSignIn){
			QEventLoop nestedLoop;
			QMetaObject::invokeMethod(&nestedLoop, [this, &nestedLoop](){
				duringFirstSignIn();
				nestedLoop.quit();
			}, Qt::QueuedConnection);
			nestedLoop.exec();
		}

		QJsonObject root;
		switch (signInIndex < answers.size() ? answers[signInIndex] : Answer::Token){
		case Answer::Token:
			root = QJsonObject{{QStringLiteral("data"), QJsonObject{{QStringLiteral("Authorization"), QJsonObject{{QStringLiteral("token"), QStringLiteral("token-%1").arg(++m_issuedTokenCount)}}}}}};
			break;
		case Answer::Refusal:
			root = QJsonObject{{QStringLiteral("errors"), QJsonArray{QJsonObject{{QStringLiteral("message"), QStringLiteral("Invalid login or password")}}}}};
			break;
		case Answer::ExpiredPassword:
			root = QJsonObject{{QStringLiteral("data"), QJsonObject{{QStringLiteral("Authorization"), QJsonObject{{QStringLiteral("passwordExpired"), true}}}}}};
			break;
		case Answer::NoConnection:
			return GqlResult{{}, EC_NETWORK, QStringLiteral("Connection refused")};
		}

		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(QJsonDocument(root).toJson(QJsonDocument::Compact));
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);

		return GqlResult{retVal, EC_NONE, {}};
	}

	bool SendRequestNoWait(GqlRequestPtr /*requestPtr*/, imtbase::IUrlParam* /*urlParamPtr*/) const override
	{
		return false;
	}

private:
	mutable QMutex m_mutex;
	mutable QList<SentSignIn> m_sentSignIns;
	mutable std::atomic<int> m_issuedTokenCount{0};
};


typedef CScriptedSignInClientComp::Answer Answer;


class CTokenObserver: public imod::CSingleModelObserverBase
{
public:
	int updateCount = 0;
	QThread* updateThreadPtr = nullptr;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/) override
	{
		++updateCount;
		updateThreadPtr = QThread::currentThread();
	}
};


struct CSessionFixture
{
	icomp::TSimSharedComponentPtr<CScriptedSignInClientComp> client;
	icomp::TSimSharedComponentPtr<iprm::CTextParamComp> login;
	icomp::TSimSharedComponentPtr<iprm::CTextParamComp> password;
	icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<imtauthgql::CRemoteSessionComp>> session;

	explicit CSessionFixture(const QList<Answer>& answers = QList<Answer>())
	{
		client->answers = answers;
		client->InitComponent();
		login->InitComponent();
		login->SetText(QStringLiteral("service"));
		password->InitComponent();
		password->SetText(QStringLiteral("secret"));
		session->SetRef("ApiClient", client);
		session->SetRef("LoginParam", login);
		session->SetRef("PasswordParam", password);
		session->SetIdAttr("ProductId", "RemoteProduct");
		session->InitComponent();
	}

	QByteArray RequestToken(QString* errorPtr = nullptr)
	{
		QString error;
		const imtgql::IGqlContextSharedPtr contextPtr = session->CreateRequestContext(error);
		if (errorPtr != nullptr){
			*errorPtr = error;
		}

		return contextPtr.IsValid() ? contextPtr->GetToken() : QByteArray();
	}
};


} // namespace


// The first request signs in with the login and the password, for the product of the remote server
void CRemoteSessionTest::FirstRequestSignsInTest()
{
	CSessionFixture fixture;

	// The request being handled here lends nothing to the sign-in
	imtgql::CGqlContext callerContext;
	callerContext.SetToken(QByteArrayLiteral("caller-token"));
	callerContext.SetProductId(QByteArrayLiteral("CallerProduct"));
	imtgql::CGqlRequestContextManager::SetContext(&callerContext);

	QString error;
	const imtgql::IGqlContextSharedPtr contextPtr = fixture.session->CreateRequestContext(error);
	imtgql::CGqlRequestContextManager::Clear();

	QVERIFY2(contextPtr.IsValid(), qPrintable(error));
	QCOMPARE(contextPtr->GetToken(), QByteArrayLiteral("token-1"));
	QCOMPARE(contextPtr->GetProductId(), QByteArrayLiteral("RemoteProduct"));

	const QList<CScriptedSignInClientComp::SentSignIn> signIns = fixture.client->GetSentSignIns();
	QCOMPARE(signIns.size(), 1);
	QCOMPARE(signIns[0].login, QStringLiteral("service"));
	QCOMPARE(signIns[0].password, QStringLiteral("secret"));
	QCOMPARE(signIns[0].productId, QByteArrayLiteral("RemoteProduct"));
	QVERIFY(signIns[0].contextToken.isEmpty());
	QCOMPARE(signIns[0].contextProductId, QByteArrayLiteral("RemoteProduct"));
}


// The token serves the next requests; GetToken does not sign in
void CRemoteSessionTest::TokenServesTheNextRequestsTest()
{
	CSessionFixture fixture;
	QVERIFY(fixture.session->GetToken(QByteArray()).isEmpty());
	QVERIFY(fixture.client->GetSentSignIns().isEmpty());

	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.session->GetToken(QByteArray()), QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);
}


// After a refused token the next request signs in again; a late report of the old token keeps the new one
void CRemoteSessionTest::RejectedTokenIsReplacedTest()
{
	CSessionFixture fixture;
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));

	fixture.session->OnTokenRejected(QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-2"));

	fixture.session->OnTokenRejected(QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-2"));
	QCOMPARE(fixture.client->GetSentSignIns().size(), 2);
}


// A refused sign-in is not sent again with the same credentials: a server may lock the account
void CRemoteSessionTest::RefusedSignInIsNotRepeatedTest()
{
	CSessionFixture fixture({Answer::Refusal});

	QString error;
	QVERIFY(fixture.RequestToken(&error).isEmpty());
	QVERIFY2(error.contains(QStringLiteral("Invalid login or password")), qPrintable(error));

	error.clear();
	QVERIFY(fixture.RequestToken(&error).isEmpty());
	QVERIFY(!error.isEmpty());
	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);
}


// New credentials are tried again
void CRemoteSessionTest::ChangedPasswordIsTriedTest()
{
	CSessionFixture fixture({Answer::Refusal});
	QVERIFY(fixture.RequestToken().isEmpty());

	fixture.password->SetText(QStringLiteral("new-secret"));
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));

	const QList<CScriptedSignInClientComp::SentSignIn> signIns = fixture.client->GetSentSignIns();
	QCOMPARE(signIns.size(), 2);
	QCOMPARE(signIns[1].password, QStringLiteral("new-secret"));
}


// An answer without a token, like the one to an expired password, is a refusal as well
void CRemoteSessionTest::AnswerWithoutTokenIsARefusalTest()
{
	CSessionFixture fixture({Answer::ExpiredPassword});

	QString error;
	QVERIFY(fixture.RequestToken(&error).isEmpty());
	QVERIFY2(error.contains(QStringLiteral("expired")), qPrintable(error));
	QVERIFY(fixture.RequestToken().isEmpty());
	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);
}


// No connection is no refusal: the next request signs in again
void CRemoteSessionTest::ConnectionFailureIsNoRefusalTest()
{
	CSessionFixture fixture({Answer::NoConnection});

	QString error;
	QVERIFY(fixture.RequestToken(&error).isEmpty());
	QVERIFY(!error.isEmpty());
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));
	QCOMPARE(fixture.client->GetSentSignIns().size(), 2);
}


// The requests that need a token at the same time share one sign-in
void CRemoteSessionTest::ParallelRequestsShareOneSignInTest()
{
	CSessionFixture fixture;
	QSemaphore firstSignInGate;
	fixture.client->firstSignInGatePtr = &firstSignInGate;

	const int requestCount = 4;
	QMutex tokensMutex;
	QByteArrayList tokens;
	std::vector<std::unique_ptr<QThread>> threads;
	threads.reserve(requestCount);
	for (int index = 0; index < requestCount; ++index){
		threads.emplace_back(QThread::create([&fixture, &tokensMutex, &tokens](){
			const QByteArray token = fixture.RequestToken();
			QMutexLocker locker(&tokensMutex);
			tokens.append(token);
		}));
	}

	threads[0]->start();
	QVERIFY(fixture.client->firstSignInStarted.tryAcquire(1, 5000));
	for (int index = 1; index < requestCount; ++index){
		threads[index]->start();
	}

	// The others come while the first sign-in waits for its answer
	QTest::qWait(200);
	firstSignInGate.release();

	for (const std::unique_ptr<QThread>& threadPtr : threads){
		QVERIFY(threadPtr->wait(5000));
	}

	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);
	QCOMPARE(tokens, QByteArrayList(requestCount, QByteArrayLiteral("token-1")));
}


// The requests of other threads take the outcome of the sign-in they waited for, a failure as well: they send no sign-in of their own
void CRemoteSessionTest::ParallelRequestsShareAFailedSignInTest()
{
	CSessionFixture fixture({Answer::NoConnection});
	QSemaphore firstSignInGate;
	fixture.client->firstSignInGatePtr = &firstSignInGate;

	const int requestCount = 4;
	QMutex errorsMutex;
	QStringList errors;
	std::vector<std::unique_ptr<QThread>> threads;
	threads.reserve(requestCount);
	for (int index = 0; index < requestCount; ++index){
		threads.emplace_back(QThread::create([&fixture, &errorsMutex, &errors](){
			QString error;
			const QByteArray token = fixture.RequestToken(&error);
			QMutexLocker locker(&errorsMutex);
			errors.append(token.isEmpty() ? error : QString());
		}));
	}

	threads[0]->start();
	QVERIFY(fixture.client->firstSignInStarted.tryAcquire(1, 5000));
	for (int index = 1; index < requestCount; ++index){
		threads[index]->start();
	}

	QTest::qWait(200);
	firstSignInGate.release();

	for (const std::unique_ptr<QThread>& threadPtr : threads){
		QVERIFY(threadPtr->wait(5000));
	}

	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);
	QCOMPARE(errors.size(), requestCount);
	for (const QString& error : std::as_const(errors)){
		QVERIFY2(error.contains(QStringLiteral("Connection refused")), qPrintable(error));
	}
}


// A synchronous sign-in waits in a nested event loop of its thread; a request that comes from that loop fails at once instead of blocking the thread for ever
void CRemoteSessionTest::RequestFromInsideTheSignInFailsTest()
{
	CSessionFixture fixture;
	QString nestedError;
	QByteArray nestedToken;
	fixture.client->duringFirstSignIn = [&fixture, &nestedError, &nestedToken](){
		nestedToken = fixture.RequestToken(&nestedError);
	};

	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));
	QVERIFY(nestedToken.isEmpty());
	QVERIFY2(nestedError.contains(QStringLiteral("in progress")), qPrintable(nestedError));
	QCOMPARE(fixture.client->GetSentSignIns().size(), 1);

	// The next request takes the token of the finished sign-in
	QCOMPARE(fixture.RequestToken(), QByteArrayLiteral("token-1"));
}


// The observers learn about a new token in the thread of the session, also after a sign-in in another thread
void CRemoteSessionTest::NewTokenIsAnnouncedInTheThreadOfTheSessionTest()
{
	CSessionFixture fixture;
	auto* modelPtr = dynamic_cast<imod::IModel*>(&fixture.session.GetImpl());
	QVERIFY(modelPtr != nullptr);

	CTokenObserver observer;
	QVERIFY(modelPtr->AttachObserver(&observer));

	std::unique_ptr<QThread> threadPtr(QThread::create([&fixture](){
		fixture.RequestToken();
	}));
	threadPtr->start();
	QVERIFY(threadPtr->wait(5000));

	QTRY_COMPARE(observer.updateCount, 1);
	QCOMPARE(observer.updateThreadPtr, QThread::currentThread());
}


I_ADD_TEST(CRemoteSessionTest);
