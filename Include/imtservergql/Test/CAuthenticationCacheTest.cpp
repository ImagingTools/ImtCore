// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAuthenticationCacheTest.h"


#include <functional>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtservergql/CAuthenticationManagerComp.h>


namespace
{


class CContextComp: public icomp::CComponentBase, public imtgql::CGqlContext
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContext);
	I_END_COMPONENT
};


class CContextFactory: public icomp::CSimComponentContextBase::ComponentsFactory
{
public:
	KeyList GetFactoryKeys() const override { return {QByteArray()}; }
	istd::TUniqueInterfacePtr<icomp::IComponent> CreateInstance(const QByteArray&) const override
	{
		auto* component = new icomp::TSimComponentWrap<CContextComp>;
		component->InitComponent();
		return istd::TUniqueInterfacePtr<icomp::IComponent>(component);
	}
};


class CTokenManagerStub: public imtauth::CPersonalAccessTokenManagerComp
{
public:
	mutable QHash<QByteArray, int> checks;
	std::function<void()> duringValidation;
	bool valid = true;
	QByteArrayList permissions{"read"};

	bool ValidateToken(const QByteArray& raw, QByteArray& user, QByteArray& id, QByteArrayList& scopes,
				QByteArray* product, QByteArray* client, bool* failed) const override
	{
		++checks[raw];
		user = "user";
		id = raw;
		scopes = permissions;
		if (product != nullptr){
			product->clear();
		}
		if (client != nullptr){
			client->clear();
		}
		if (failed != nullptr){
			*failed = false;
		}
		if (duringValidation){
			duringValidation();
		}
		return valid;
	}

	bool UpdateLastUsedAt(const QByteArray&) override { return true; }
};


class CTestAuthenticationManager: public imtservergql::CAuthenticationManagerComp
{
public:
	void Changed(const QByteArray& token)
	{
		istd::IChangeable::ChangeSet changes(imtauth::IPersonalAccessTokenManager::CF_TOKEN_CHANGED);
		changes.SetChangeInfo(imtauth::IPersonalAccessTokenManager::CN_TOKEN_ID, token);
		OnUpdate(nullptr, changes);
	}

	// Users, roles or memberships changed: no token did
	void AccessChanged()
	{
		OnUpdate(nullptr, istd::IChangeable::ChangeSet(istd::IChangeable::CF_ANY));
	}
};


struct CAuthenticationFixture
{
	CContextFactory factory;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> tokens;
	icomp::TSimSharedComponentPtr<CTestAuthenticationManager> authentication;

	CAuthenticationFixture()
	{
		tokens->InitComponent();
		authentication->SetRef("PersonalAccessTokenManager", tokens);
		authentication->SetFactory("GqlContextFactory", &factory);
		authentication->InitComponent();
	}

	bool Resolve(const QByteArray& token)
	{
		imtgql::IGqlContextCreator::ContextCreationError error;
		return Resolve(token, error);
	}

	bool Resolve(const QByteArray& token, imtgql::IGqlContextCreator::ContextCreationError& error)
	{
		const auto context = authentication->CreateGqlContext({{imtbase::s_authenticationTokenHeaderId, token}}, error);
		imtgql::CGqlRequestContextManager::Clear();
		return context.IsValid() && error.status == imtgql::IGqlContextCreator::CCS_OK;
	}

	// The check of an established subscription: reads the token afresh
	bool Renew(const QByteArray& token)
	{
		imtgql::IGqlContextCreator::AuthorizationState state = imtgql::IGqlContextCreator::AS_UNAVAILABLE;
		imtgql::IGqlContextCreator::ContextCreationError error;
		return authentication->RenewGqlContext({{imtbase::s_authenticationTokenHeaderId, token}}, true, state, error).IsValid();
	}
};


} // namespace


void CAuthenticationCacheTest::TargetedInvalidation()
{
	CAuthenticationFixture fixture;
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QVERIFY(fixture.Resolve("imt_pat_second"));
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 1);
	fixture.authentication->Changed("imt_pat_first");
	QVERIFY(fixture.Resolve("imt_pat_second"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_second"), 1);
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);
	fixture.authentication->Changed({});
	QVERIFY(fixture.Resolve("imt_pat_second"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_second"), 2);
}


void CAuthenticationCacheTest::InFlightInvalidation()
{
	CAuthenticationFixture fixture;
	// Changed during every check: no answer is current, the token could not be checked
	fixture.tokens->duringValidation = [&](){ fixture.authentication->Changed("imt_pat_first"); };
	imtgql::IGqlContextCreator::ContextCreationError error;
	QVERIFY(!fixture.Resolve("imt_pat_first", error));
	QCOMPARE(error.status, imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR);
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);
	fixture.tokens->duringValidation = {};
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 3);
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 3);

	// Changed during the first check only: checked again, the second answer holds
	fixture.authentication->Changed("imt_pat_first");
	fixture.tokens->duringValidation = [&](){
		fixture.tokens->duringValidation = {};
		fixture.authentication->Changed("imt_pat_first");
	};
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 5);
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 5);

	fixture.tokens->duringValidation = [&](){ fixture.authentication->Changed("imt_pat_unrelated"); };
	QVERIFY(fixture.Resolve("imt_pat_second"));
	fixture.tokens->duringValidation = {};
	QVERIFY(fixture.Resolve("imt_pat_second"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_second"), 1);
}


// The answer from before the revocation does not let the request through
void CAuthenticationCacheTest::RevokedDuringItsCheck()
{
	CAuthenticationFixture fixture;
	// Revoked while the first check runs: the store answers with it from the second one
	fixture.tokens->duringValidation = [&](){
		fixture.tokens->duringValidation = [&](){ fixture.tokens->valid = false; };
		fixture.authentication->Changed("imt_pat_first");
	};

	imtgql::IGqlContextCreator::ContextCreationError error;
	QVERIFY(!fixture.Resolve("imt_pat_first", error));
	QCOMPARE(error.status, imtgql::IGqlContextCreator::CCS_FORBIDDEN);
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);
}


// A server reconnects: its subscription reads the token afresh while its request checks it. Not a change: the request goes on
void CAuthenticationCacheTest::FreshReadDuringTheCheck()
{
	CAuthenticationFixture fixture;
	bool isRenewed = false;
	fixture.tokens->duringValidation = [&](){
		fixture.tokens->duringValidation = {};
		isRenewed = fixture.Renew("imt_pat_first");
	};

	imtgql::IGqlContextCreator::ContextCreationError error;
	QVERIFY2(fixture.Resolve("imt_pat_first", error), qPrintable(error.message));
	QVERIFY(isRenewed);
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);

	// Its answer is not cached, it may be older than the fresh one; the next one is
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 3);
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 3);
}


// Busy changes of the access while a token is checked: the token is valid all the same, its answer is only not kept
void CAuthenticationCacheTest::AccessChangeDuringTheCheckIsNoTokenChange()
{
	CAuthenticationFixture fixture;
	fixture.tokens->duringValidation = [&](){ fixture.authentication->AccessChanged(); };

	imtgql::IGqlContextCreator::ContextCreationError error;
	QVERIFY2(fixture.Resolve("imt_pat_first", error), qPrintable(error.message));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 1);

	fixture.tokens->duringValidation = {};
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);
}


void CAuthenticationCacheTest::SubscriptionCheckBypassesCache()
{
	CAuthenticationFixture fixture;
	QVERIFY(fixture.Resolve("imt_pat_first"));
	fixture.tokens->permissions = {"narrowed"};
	const imtgql::IGqlContext::Headers headers{{imtbase::s_authenticationTokenHeaderId, "imt_pat_first"}};
	imtgql::IGqlContextCreator::AuthorizationState state = imtgql::IGqlContextCreator::AS_UNAVAILABLE;
	imtgql::IGqlContextCreator::ContextCreationError error;
	QVERIFY(fixture.authentication->RenewGqlContext(headers, true, state, error).IsValid());
	QCOMPARE(state, imtgql::IGqlContextCreator::AS_CONFIRMED);
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 2);

	// The narrowed token replaces the cached one for the requests too
	QVERIFY(fixture.Resolve("imt_pat_first"));
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 3);
	fixture.tokens->valid = false;
	QVERIFY(!fixture.authentication->RenewGqlContext(headers, true, state, error).IsValid());
	QCOMPARE(state, imtgql::IGqlContextCreator::AS_DENIED);
	QCOMPARE(fixture.tokens->checks.value("imt_pat_first"), 4);
	QVERIFY(!error.message.isEmpty());
	QVERIFY(fixture.authentication->GetUserFromJwt("imt_pat_first").isEmpty());
}


I_ADD_TEST(CAuthenticationCacheTest);
