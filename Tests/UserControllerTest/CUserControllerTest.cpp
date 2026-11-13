// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CUserControllerTest.h"


// STL includes
#include <atomic>
#include <functional>
#include <thread>

// Qt includes
#include <QtCore/QThread>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <iprm/TParamsPtr.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtauth/CCredentialControllerComp.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserInfoComp.h>
#include <imtauth/CTenantInfo.h>
#include <imtauth/CTenantInvitation.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/CUserVerification.h>
#include <imtauth/CUserVerificationControllerComp.h>
#include <imtauth/IUserVerificationCodeSender.h>
#include <imtauthgql/CUserCollectionControllerComp.h>
#include <imtauthgql/CUserControllerComp.h>
#include <imtcrypt/CMD5HashCalculator.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace
{


/**
	Users selected like the SQL user table does: by the login ("Id"), the e-mail ("Mail") and the password hash.
*/
class CTestUserCollectionComp:
			public icomp::CComponentBase,
			public imtbase::CObjectCollectionBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestUserCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override
	{
		const Ids allIds = imtbase::CObjectCollectionBase::GetElementIds(0, -1, nullptr, logPtr);

		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(selectionParamsPtr, "ComplexFilter", false);
		if (!complexFilterPtr.IsValid()){
			return allIds.mid(offset, count);
		}

		Ids retVal;
		for (const QByteArray& objectId : allIds){
			DataPtr dataPtr;
			if (!GetObjectData(objectId, dataPtr)){
				continue;
			}

			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			bool isMatch = userInfoPtr != nullptr;
			for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : complexFilterPtr->GetFilterExpression().fieldFilters){
				const QString filterValue = fieldFilter.filterValue.toString();
				if (fieldFilter.fieldId == "Id"){
					isMatch = isMatch && QString::fromUtf8(userInfoPtr->GetId()) == filterValue;
				}
				else if (fieldFilter.fieldId == "Mail"){
					isMatch = isMatch && userInfoPtr->GetMail() == filterValue;
				}
				else if (fieldFilter.fieldId == "PasswordHash"){
					isMatch = isMatch && QString::fromUtf8(userInfoPtr->GetPasswordHash()) == filterValue;
				}
				else{
					isMatch = false;
				}
			}

			if (isMatch){
				retVal.append(objectId);
			}
		}

		return retVal.mid(offset, count);
	}

	// Runs once, right after the next object is read, while the reader is still inside the read
	mutable std::function<void()> m_afterLoadHook;

	// reimplemented (imtbase::IObjectCollection)
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override
	{
		const bool retVal = imtbase::CObjectCollectionBase::GetObjectData(objectId, dataPtr, dataConfigurationPtr);
		if (m_afterLoadHook){
			const std::function<void()> hook = m_afterLoadHook;
			m_afterLoadHook = nullptr;
			hook();
		}

		return retVal;
	}

protected:
	// reimplemented (imtbase::CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override
	{
		if (typeId == "UserVerification"){
			return new imtauth::CIdentifiableUserVerificationInfo;
		}

		return new imtauth::CIdentifiableUserInfo;
	}
};


/**
	Keeps the sent codes instead of mailing them.
*/
class CTestCodeSenderComp:
			public icomp::CComponentBase,
			virtual public imtauth::IUserVerificationCodeSender
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestCodeSenderComp);
		I_REGISTER_INTERFACE(imtauth::IUserVerificationCodeSender);
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserVerificationCodeSender)
	virtual bool SendCode(const imtauth::IUserInfo& userInfo, const QByteArray& verificationCode) const override
	{
		if (m_isFailing){
			return false;
		}

		m_codes.insert(userInfo.GetMail(), verificationCode);
		++m_sentCount;

		return true;
	}

	mutable QHash<QString, QByteArray> m_codes;
	mutable int m_sentCount = 0;
	bool m_isFailing = false;
};


/**
	The controller with its request handlers callable from the test.
*/
class CTestUserControllerComp: public imtauthgql::CUserControllerComp
{
public:
	typedef imtauthgql::CUserControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTestUserControllerComp);
	I_END_COMPONENT;

	using BaseClass::OnChangePassword;
	using BaseClass::OnSendEmailCode;
	using BaseClass::OnCheckEmailCode;
	using BaseClass::OnCheckEmail;
	using BaseClass::OnGetUserObjectId;
};


class CAdminUserInfo: public imtauth::CUserInfo
{
public:
	// reimplemented (imtauth::IUserInfo)
	virtual bool IsAdmin() const override
	{
		return true;
	}

	// reimplemented (istd::IChangeable)
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		istd::IChangeableUniquePtr retVal(new CAdminUserInfo);
		if (!retVal->CopyFrom(*this, mode)){
			return nullptr;
		}

		return retVal;
	}
};


typedef icomp::TMakeComponentWrap<imtcrypt::CMD5HashCalculator, imtcrypt::IHashGenerator> CHashCalculatorComp;


QByteArray NewId()
{
	return QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}


struct ChangeResult
{
	bool success = false;
	QString message;
	QStringList violatedRules;
};


/**
	Invitations selected like the SQL invitation table does, by the inviter; loads and removals can be made to fail.
*/
class CTestInvitationCollectionComp:
			public icomp::CComponentBase,
			public imtbase::CObjectCollectionBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestInvitationCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	QSet<QByteArray> m_loadFailingIds;
	bool m_isRemovalFailing = false;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override
	{
		const Ids allIds = imtbase::CObjectCollectionBase::GetElementIds(0, -1, nullptr, logPtr);

		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(selectionParamsPtr, "ComplexFilter", false);
		if (!complexFilterPtr.IsValid()){
			return allIds.mid(offset, count);
		}

		Ids retVal;
		for (const QByteArray& objectId : allIds){
			DataPtr dataPtr;
			if (!imtbase::CObjectCollectionBase::GetObjectData(objectId, dataPtr)){
				continue;
			}

			const imtauth::ITenantInvitation* invitationPtr = dynamic_cast<const imtauth::ITenantInvitation*>(dataPtr.GetPtr());
			bool isMatch = invitationPtr != nullptr;
			for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : complexFilterPtr->GetFilterExpression().fieldFilters){
				isMatch = isMatch && fieldFilter.fieldId == "InvitedByUserId"
							&& QString::fromUtf8(invitationPtr->GetInvitedByUserId()) == fieldFilter.filterValue.toString();
			}

			if (isMatch){
				retVal.append(objectId);
			}
		}

		return retVal.mid(offset, count);
	}

	// reimplemented (imtbase::IObjectCollection)
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override
	{
		if (m_loadFailingIds.contains(objectId)){
			return false;
		}

		return imtbase::CObjectCollectionBase::GetObjectData(objectId, dataPtr, dataConfigurationPtr);
	}

	virtual bool RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr = nullptr) override
	{
		if (m_isRemovalFailing){
			return false;
		}

		return imtbase::CObjectCollectionBase::RemoveElements(elementIds, operationContextPtr);
	}

protected:
	// reimplemented (imtbase::CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& /*typeId*/) const override
	{
		return new imtauth::CIdentifiableTenantInvitation;
	}
};


/**
	One organization created by a user, whose reads can be made to fail.
*/
class CTestTenantManagerComp:
			public icomp::CComponentBase,
			virtual public imtauth::ITenantManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestTenantManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantManager);
	I_END_COMPONENT;

	QByteArray m_creatorId;
	QByteArray m_tenantId = NewId();
	bool m_isSelectionFailing = false;
	bool m_isLoadFailing = false;

	// reimplemented (imtauth::ITenantManager)
	virtual QByteArrayList GetTenantIds() const override { return {m_tenantId}; }
	virtual imtauth::ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const override
	{
		if (m_isLoadFailing || tenantId != m_tenantId){
			return imtauth::ITenantInfoUniquePtr();
		}

		imtauth::ITenantInfoUniquePtr retVal(new imtauth::CIdentifiableTenantInfo);
		retVal->SetCreatorId(m_creatorId);
		retVal->SetOwnerId(NewId());

		return retVal;
	}
	virtual QByteArrayList GetOwnedOrCreatedTenantIds(const QByteArray& userId, bool* isReadPtr = nullptr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = !m_isSelectionFailing;
		}

		return !m_isSelectionFailing && userId == m_creatorId ? QByteArrayList({m_tenantId}) : QByteArrayList();
	}
	virtual QByteArrayList GetChildTenantIds(const QByteArray& /*tenantId*/, bool* isReadPtr = nullptr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = true;
		}

		return QByteArrayList();
	}
	virtual QByteArrayList FindTenantIdsByName(const QString& /*tenantName*/) const override { return QByteArrayList(); }
	virtual QByteArrayList GetTenantPermissions(const QByteArray& /*tenantId*/) const override { return QByteArrayList(); }
	virtual QByteArray CreateTenant(const QString& /*tenantName*/, const QString& /*description*/, const QByteArray& /*ownerId*/, const QByteArray& /*creatorId*/, const QByteArray& /*parentTenantId*/) override { return QByteArray(); }
	virtual bool RemoveTenant(const QByteArray& /*tenantId*/) override { return false; }
	virtual bool UpdateTenant(const QByteArray& /*tenantId*/, const QString& /*tenantName*/, const QString& /*description*/, const QByteArray& /*ownerId*/, bool /*updateOwner*/) override { return false; }
	virtual bool SetTenantActive(const QByteArray& /*tenantId*/, bool /*isActive*/) override { return false; }
	virtual bool SetTenantPermissions(const QByteArray& /*tenantId*/, const QByteArrayList& /*permissions*/) override { return false; }
	virtual bool SetTenantHierarchy(const QByteArray& /*tenantId*/, const QByteArray& /*parentTenantId*/) override { return false; }
	virtual QByteArray GetSystemTenantId() const override { return QByteArray(); }
	virtual bool EnsureSystemTenant() override { return false; }
};


/**
	The user collection controller with its removal check callable from the test.
*/
class CTestUserCollectionControllerComp: public imtauthgql::CUserCollectionControllerComp
{
public:
	typedef imtauthgql::CUserCollectionControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTestUserCollectionControllerComp);
	I_END_COMPONENT;

	using BaseClass::OnBeforeRemoveElements;
};


class CRemovalFixture
{
public:
	CRemovalFixture()
	{
		invitations->InitComponent();
		tenants->InitComponent();
		controller->SetRef("InvitationCollection", invitations);
		controller->SetRef("TenantManager", tenants);
		controller->InitComponent();
	}

	QByteArray AddInvitation(const QByteArray& invitedByUserId)
	{
		const QByteArray invitationId = NewId();

		imtauth::CIdentifiableTenantInvitation invitation;
		invitation.SetObjectUuid(invitationId);
		invitation.SetUserId(NewId());
		invitation.SetTenantId(NewId());
		invitation.SetInvitedByUserId(invitedByUserId);
		invitations->InsertNewObject("TenantInvitation", QString(), QString(), &invitation, invitationId);

		return invitationId;
	}

	bool RemoveUsers(const QByteArrayList& userIds, QString& errorMessage)
	{
		return controller->OnBeforeRemoveElements(userIds, imtgql::CGqlRequest(), errorMessage);
	}

	icomp::TSimSharedComponentPtr<CTestInvitationCollectionComp> invitations;
	icomp::TSimSharedComponentPtr<CTestTenantManagerComp> tenants;
	icomp::TSimSharedComponentPtr<CTestUserCollectionControllerComp> controller;
};


class CFixture
{
public:
	CFixture()
	{
		users->InitComponent();
		hashCalculator->InitComponent();
		codeSender->InitComponent();

		credentials->SetRef("HashCalculator", hashCalculator);
		credentials->SetRef("UserCollection", users);
		credentials->InitComponent();

		verifications->InitComponent();
		verification->SetRef("UserVerificationCollection", verifications);
		verification->SetIntAttr("CodeValidityPeriod", 180);
		verification->InitComponent();

		controller->SetRef("UserVerificationController", verification);
		controller->SetRef("UserCollection", users);
		controller->SetRef("HashCalculator", hashCalculator);
		controller->SetRef("CredentialController", credentials);
		controller->SetFactory("UserFactory", &userFactory);
		controller->SetRef("UserVerificationCodeSender", codeSender);
		controller->InitComponent();
	}

	QByteArray AddUser(const QByteArray& login, const QString& password, const QByteArray& externalSystemId = QByteArray())
	{
		const QByteArray objectId = NewId();

		imtauth::CIdentifiableUserInfo user;
		user.SetObjectUuid(objectId);
		user.SetId(login);
		user.SetName(QString::fromUtf8(login));
		user.SetMail(Mail(login));
		user.SetPasswordHash(Hash(login, password));
		if (!externalSystemId.isEmpty()){
			imtauth::IUserInfo::SystemInfo systemInfo;
			systemInfo.systemId = externalSystemId;
			systemInfo.systemName = QString::fromUtf8(externalSystemId);
			user.AddToSystem(systemInfo);
		}

		users->InsertNewObject("User", QString(), QString(), &user, objectId);

		return objectId;
	}

	static QString Mail(const QByteArray& login)
	{
		return QString::fromUtf8(login) + QStringLiteral("@test.local");
	}

	QByteArray Hash(const QByteArray& login, const QString& password)
	{
		return hashCalculator->GenerateHash(login + password.toUtf8());
	}

	bool HasPassword(const QByteArray& login, const QString& password)
	{
		return static_cast<imtauth::ICredentialController&>(credentials.GetImpl()).CheckCredential(login, password.toUtf8());
	}

	ChangeResult ChangePassword(
				const QByteArray& login,
				const QString& oldPassword,
				const QString& newPassword,
				const QString& verificationCode = QString(),
				const imtauth::IUserInfo* requestUserPtr = nullptr)
	{
		imtgql::CGqlParamObject input;
		input.InsertParam("login", QVariant(login));
		if (!oldPassword.isEmpty()){
			input.InsertParam("oldPassword", QVariant(oldPassword));
		}
		input.InsertParam("newPassword", QVariant(newPassword));
		if (!verificationCode.isEmpty()){
			input.InsertParam("verificationCode", QVariant(verificationCode));
		}

		imtgql::CGqlRequest request;
		SetupRequest(request, "ChangePassword", input, requestUserPtr);

		QString errorMessage;
		const sdl::V1_0::imtauth::CChangePasswordGqlRequest changeRequest(request, false);
		const sdl::V1_0::imtauth::CChangePasswordPayload payload = controller->OnChangePassword(changeRequest, request, errorMessage);

		ChangeResult retVal;
		retVal.success = payload.success.has_value() && *payload.success;
		retVal.message = payload.message.has_value() ? *payload.message : QString();
		if (payload.violatedRules.has_value()){
			retVal.violatedRules = payload.violatedRules->ToList();
		}

		return retVal;
	}

	sdl::V1_0::imtauth::CSendEmailCodePayload SendEmailCode(const QByteArray& loginOrEmail)
	{
		imtgql::CGqlParamObject input;
		input.InsertParam("login", QVariant(loginOrEmail));

		imtgql::CGqlRequest request;
		SetupRequest(request, "SendEmailCode", input);

		QString errorMessage;
		const sdl::V1_0::imtauth::CSendEmailCodeGqlRequest sendRequest(request, false);

		return controller->OnSendEmailCode(sendRequest, request, errorMessage);
	}

	bool CheckEmailCode(const QByteArray& loginOrEmail, const QString& code)
	{
		imtgql::CGqlParamObject input;
		input.InsertParam("login", QVariant(loginOrEmail));
		input.InsertParam("code", QVariant(code));

		imtgql::CGqlRequest request;
		SetupRequest(request, "CheckEmailCode", input);

		QString errorMessage;
		const sdl::V1_0::imtauth::CCheckEmailCodeGqlRequest checkRequest(request, false);
		const sdl::V1_0::imtauth::CCheckEmailCodePayload payload = controller->OnCheckEmailCode(checkRequest, request, errorMessage);

		return payload.correctCode.has_value() && *payload.correctCode;
	}

	QByteArray GetUserObjectId(const QByteArray& login, const imtauth::IUserInfo* requestUserPtr = nullptr, const QByteArray& requestUserId = QByteArray())
	{
		imtgql::CGqlParamObject input;
		input.InsertParam("login", QVariant(login));

		imtgql::CGqlRequest request;
		SetupRequest(request, "GetUserObjectId", input, requestUserPtr, requestUserId);

		QString errorMessage;
		const sdl::V1_0::imtauth::CGetUserObjectIdGqlRequest objectIdRequest(request, false);
		const sdl::V1_0::imtauth::CUserObjectId payload = controller->OnGetUserObjectId(objectIdRequest, request, errorMessage);

		return payload.objectId.has_value() ? *payload.objectId : QByteArray();
	}

	sdl::V1_0::imtauth::CCheckEmailPayload CheckEmail(const QString& email)
	{
		imtgql::CGqlParamObject input;
		input.InsertParam("email", QVariant(email));

		imtgql::CGqlRequest request;
		SetupRequest(request, "CheckEmail", input);

		QString errorMessage;
		const sdl::V1_0::imtauth::CCheckEmailGqlRequest checkRequest(request, false);

		return controller->OnCheckEmail(checkRequest, request, errorMessage);
	}

	QString SentCode(const QByteArray& login)
	{
		return QString::fromUtf8(codeSender->m_codes.value(Mail(login)));
	}

	int SentCount()
	{
		return codeSender->m_sentCount;
	}

	static QString WrongCode(const QString& code)
	{
		return code == QStringLiteral("000000") ? QStringLiteral("111111") : QStringLiteral("000000");
	}

	icomp::TSimSharedComponentPtr<CTestUserCollectionComp> users;
	icomp::TSimSharedComponentPtr<CHashCalculatorComp> hashCalculator;
	icomp::TSimSharedComponentPtr<CTestCodeSenderComp> codeSender;
	icomp::TSimSharedComponentPtr<imtauth::CCredentialControllerComp> credentials;
	icomp::TSimSharedComponentPtr<CTestUserCollectionComp> verifications;
	icomp::TSimSharedComponentPtr<imtauth::CUserVerificationControllerComp> verification;
	icomp::TSimComponentsFactory<imtauth::CUserInfoComp> userFactory;
	icomp::TSimSharedComponentPtr<CTestUserControllerComp> controller;

private:
	void SetupRequest(
				imtgql::CGqlRequest& request,
				const QByteArray& commandId,
				const imtgql::CGqlParamObject& input,
				const imtauth::IUserInfo* requestUserPtr = nullptr,
				const QByteArray& requestUserId = QByteArray())
	{
		request.SetCommandId(commandId);
		request.AddParam("input", input);

		if (requestUserPtr != nullptr){
			imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
			contextPtr->SetProductId("Puma");
			contextPtr->SetUserInfo(requestUserPtr);
			contextPtr->SetUserId(requestUserId);
			request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));
		}
	}
};


} // anonymous namespace


// Change with the old password

void CUserControllerTest::testChangeWithOldPassword_Succeeds()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const ChangeResult result = fixture.ChangePassword("alice", "Old_Pass_1", "New_Pass_2");
	QVERIFY2(result.success, qPrintable(result.message));
	QVERIFY(fixture.HasPassword("alice", "New_Pass_2"));
	QVERIFY(!fixture.HasPassword("alice", "Old_Pass_1"));
	QCOMPARE(fixture.SentCount(), 0);
}


void CUserControllerTest::testChangeWithWrongOldPassword_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const ChangeResult result = fixture.ChangePassword("alice", "Wrong_Pass", "New_Pass_2");
	QVERIFY(!result.success);
	QCOMPARE(result.violatedRules, QStringList({"InvalidCredentials"}));
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
}


void CUserControllerTest::testChangeWithoutOldPasswordAndCode_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	// A pending verification alone proves nothing
	QVERIFY(fixture.SendEmailCode("alice").success.has_value());

	const ChangeResult result = fixture.ChangePassword("alice", QString(), "Hijacked_1");
	QVERIFY(!result.success);
	QCOMPARE(result.violatedRules, QStringList({"InvalidCredentials"}));
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
}


void CUserControllerTest::testSignedInUserWithoutOldPassword_CannotChangeAnotherPassword()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	fixture.AddUser("mallory", "Mallory_1");

	imtauth::CUserInfo mallory;
	mallory.SetId("mallory");

	const ChangeResult result = fixture.ChangePassword("alice", QString(), "Hijacked_1", QString(), &mallory);
	QVERIFY(!result.success);
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
}


void CUserControllerTest::testAdmin_ChangesAnotherPasswordWithoutOldPassword()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	CAdminUserInfo admin;
	admin.SetId("su");

	const ChangeResult result = fixture.ChangePassword("alice", QString(), "Reset_Pass_1", QString(), &admin);
	QVERIFY2(result.success, qPrintable(result.message));
	QVERIFY(fixture.HasPassword("alice", "Reset_Pass_1"));
}


// Recovery by the e-mailed code

void CUserControllerTest::testRecoveryWithoutSentCode_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const ChangeResult result = fixture.ChangePassword("alice", QString(), "Hijacked_1", "123456");
	QVERIFY(!result.success);
	QCOMPARE(result.violatedRules, QStringList({"InvalidCredentials"}));
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
}


void CUserControllerTest::testRecoveryWithWrongCode_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	QVERIFY(*fixture.SendEmailCode("alice").success);
	const QString code = fixture.SentCode("alice");
	QVERIFY(!code.isEmpty());

	QVERIFY(!fixture.ChangePassword("alice", QString(), "Hijacked_1", CFixture::WrongCode(code)).success);
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
}


void CUserControllerTest::testRecoveryWithRightCode_SucceedsOnce()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	QVERIFY(*fixture.SendEmailCode("alice").success);
	const QString code = fixture.SentCode("alice");

	const ChangeResult result = fixture.ChangePassword("alice", QString(), "Recovered_1", code);
	QVERIFY2(result.success, qPrintable(result.message));
	QVERIFY(fixture.HasPassword("alice", "Recovered_1"));

	// The code is used up
	QVERIFY(!fixture.ChangePassword("alice", QString(), "Hijacked_1", code).success);
	QVERIFY(fixture.HasPassword("alice", "Recovered_1"));
	QVERIFY(!fixture.CheckEmailCode("alice", code));
}


void CUserControllerTest::testRecoveryByEmail_HashesWithTheLogin()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	const QByteArray mail = CFixture::Mail("alice").toUtf8();

	QVERIFY(*fixture.SendEmailCode(mail).success);
	const QString code = fixture.SentCode("alice");
	QVERIFY(fixture.CheckEmailCode(mail, code));

	const ChangeResult result = fixture.ChangePassword(mail, QString(), "Recovered_1", code);
	QVERIFY2(result.success, qPrintable(result.message));
	QVERIFY(fixture.HasPassword("alice", "Recovered_1"));
}


void CUserControllerTest::testRecoveryAfterTooManyWrongCodes_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	QVERIFY(*fixture.SendEmailCode("alice").success);
	const QString code = fixture.SentCode("alice");
	for (int i = 0; i < 3; ++i){
		QVERIFY(!fixture.CheckEmailCode("alice", CFixture::WrongCode(code)));
	}
	for (int i = 0; i < 2; ++i){
		QVERIFY(!fixture.ChangePassword("alice", QString(), "Hijacked_1", CFixture::WrongCode(code)).success);
	}

	QVERIFY(!fixture.ChangePassword("alice", QString(), "Recovered_1", code).success);
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));

	// A new code starts over
	QVERIFY(*fixture.SendEmailCode("alice").success);
	const QString newCode = fixture.SentCode("alice");
	QVERIFY(fixture.ChangePassword("alice", QString(), "Recovered_1", newCode).success);
	QVERIFY(fixture.HasPassword("alice", "Recovered_1"));
}


void CUserControllerTest::testRecoveryCodeOfAnotherUser_IsRefused()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	fixture.AddUser("mallory", "Mallory_1");

	QVERIFY(*fixture.SendEmailCode("mallory").success);
	const QString malloryCode = fixture.SentCode("mallory");

	QVERIFY(!fixture.ChangePassword("alice", QString(), "Hijacked_1", malloryCode).success);
	QVERIFY(fixture.HasPassword("alice", "Old_Pass_1"));
	QVERIFY(fixture.ChangePassword("mallory", QString(), "Recovered_1", malloryCode).success);
}


void CUserControllerTest::testRecoveryCode_GeneratedDuringAConsumptionStays()
{
	CFixture fixture;
	const QByteArray userId = NewId();
	imtauth::CIdentifiableUserVerificationInfo oldCode;
	QVERIFY(fixture.verification->GenerateVerificationData(userId, &oldCode));

	// A new code is generated after an old-code request read the stored code: without one lock the generation completes
	// inside that request, which then removes the new code
	imtauth::CIdentifiableUserVerificationInfo newCode;
	std::atomic<bool> isGenerated{false};
	std::thread generation;
	fixture.verifications->m_afterLoadHook = [&fixture, &userId, &newCode, &isGenerated, &generation](){
		generation = std::thread([&fixture, &userId, &newCode, &isGenerated](){
			isGenerated = fixture.verification->GenerateVerificationData(userId, &newCode);
		});
		QThread::msleep(200);
	};
	QVERIFY(fixture.verification->ConsumeVerification(userId, oldCode.GetVerificationCode()));
	generation.join();

	QVERIFY(isGenerated.load());
	QVERIFY2(fixture.verification->VerifyUser(userId, newCode.GetVerificationCode()), "The newly sent code was removed");
}


void CUserControllerTest::testCheckEmailCode_DoesNotUseTheCode()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	QVERIFY(*fixture.SendEmailCode("alice").success);
	const QString code = fixture.SentCode("alice");
	QVERIFY(fixture.CheckEmailCode("alice", code));
	QVERIFY(fixture.CheckEmailCode("alice", code));

	QVERIFY(fixture.ChangePassword("alice", QString(), "Recovered_1", code).success);
}


// Answers to anonymous requests

void CUserControllerTest::testUnknownAccount_AnswersLikeAWrongPassword()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const ChangeResult unknown = fixture.ChangePassword("nobody", "Any_Pass_1", "New_Pass_2");
	ChangeResult wrong = fixture.ChangePassword("alice", "Any_Pass_1", "New_Pass_2");
	QVERIFY(!unknown.success);
	QCOMPARE(unknown.violatedRules, wrong.violatedRules);
	QCOMPARE(unknown.message, wrong.message.replace("alice", "nobody"));
}


// The answers tell nothing about the account that is not proven: neither that it exists nor its login or kind
void CUserControllerTest::testEmailOfAnAccount_AnswersLikeAnUnknownEmail()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	const QByteArray knownMail = CFixture::Mail("alice").toUtf8();
	const QByteArray unknownMail = CFixture::Mail("nobody").toUtf8();

	ChangeResult known = fixture.ChangePassword(knownMail, "Any_Pass_1", "New_Pass_2");
	const ChangeResult unknown = fixture.ChangePassword(unknownMail, "Any_Pass_1", "New_Pass_2");
	QVERIFY(!known.success);
	QVERIFY2(!known.message.contains("'alice'"), qPrintable(known.message));
	QCOMPARE(known.violatedRules, unknown.violatedRules);
	QCOMPARE(known.message.replace(QString::fromUtf8(knownMail), QString::fromUtf8(unknownMail)), unknown.message);
}


void CUserControllerTest::testExternalAccount_AnswersLikeAWrongPasswordUntilProven()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	fixture.AddUser("ext", "Ext_Pass_1", "Google");

	ChangeResult wrong = fixture.ChangePassword("alice", "Any_Pass_1", "New_Pass_2");
	const ChangeResult external = fixture.ChangePassword("ext", "Any_Pass_1", "New_Pass_2");
	QVERIFY(!external.success);
	QCOMPARE(external.violatedRules, wrong.violatedRules);
	QCOMPARE(wrong.message.replace("alice", "ext"), external.message);

	// Who proves the account learns why the password cannot be changed
	const ChangeResult proven = fixture.ChangePassword("ext", "Ext_Pass_1", "New_Pass_2");
	QVERIFY(!proven.success);
	QCOMPARE(proven.violatedRules, QStringList({QStringLiteral("ExternalUser")}));
}


void CUserControllerTest::testSendEmailCode_UnknownAccountAnswersLikeAKnownOne()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const sdl::V1_0::imtauth::CSendEmailCodePayload known = fixture.SendEmailCode("alice");
	const sdl::V1_0::imtauth::CSendEmailCodePayload unknown = fixture.SendEmailCode("nobody");
	const sdl::V1_0::imtauth::CSendEmailCodePayload unknownMail = fixture.SendEmailCode("nobody@test.local");

	QCOMPARE(fixture.SentCount(), 1);
	for (const sdl::V1_0::imtauth::CSendEmailCodePayload* payloadPtr : {&known, &unknown, &unknownMail}){
		QVERIFY(payloadPtr->success.has_value() && *payloadPtr->success);
		QVERIFY(!payloadPtr->message.has_value() || payloadPtr->message->isEmpty());
	}
	QVERIFY(!fixture.CheckEmailCode("nobody", "000000"));
}


void CUserControllerTest::testSendEmailCode_FailedSendingAnswersLikeAnUnknownAccount()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	fixture.codeSender->m_isFailing = true;

	const sdl::V1_0::imtauth::CSendEmailCodePayload failed = fixture.SendEmailCode("alice");
	const sdl::V1_0::imtauth::CSendEmailCodePayload unknown = fixture.SendEmailCode("nobody");
	for (const sdl::V1_0::imtauth::CSendEmailCodePayload* payloadPtr : {&failed, &unknown}){
		QVERIFY(payloadPtr->success.has_value() && *payloadPtr->success);
		QVERIFY(!payloadPtr->message.has_value() || payloadPtr->message->isEmpty());
	}
}


void CUserControllerTest::testCheckEmail_NamesNoAccount()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	const sdl::V1_0::imtauth::CCheckEmailPayload known = fixture.CheckEmail(CFixture::Mail("alice"));
	const sdl::V1_0::imtauth::CCheckEmailPayload unknown = fixture.CheckEmail("nobody@test.local");

	for (const sdl::V1_0::imtauth::CCheckEmailPayload* payloadPtr : {&known, &unknown}){
		QVERIFY(payloadPtr->success.has_value() && *payloadPtr->success);
		QVERIFY(!payloadPtr->login.has_value() || payloadPtr->login->isEmpty());
		QVERIFY(!payloadPtr->userName.has_value() || payloadPtr->userName->isEmpty());
	}
	QCOMPARE(fixture.SentCount(), 0);
}


// Account ids

void CUserControllerTest::testGetUserObjectId_OwnAccount()
{
	CFixture fixture;
	const QByteArray aliceId = fixture.AddUser("alice", "Old_Pass_1");

	imtauth::CUserInfo alice;
	alice.SetId("alice");

	QCOMPARE(fixture.GetUserObjectId("alice", &alice, aliceId), aliceId);
}


void CUserControllerTest::testGetUserObjectId_UserViewerGetsAnotherAccount()
{
	CFixture fixture;
	const QByteArray aliceId = fixture.AddUser("alice", "Old_Pass_1");
	const QByteArray managerId = fixture.AddUser("manager", "Manager_1");

	// The permission UserItem requires, held by an account that is no superuser
	imtauth::CUserInfo manager;
	manager.SetId("manager");
	manager.SetLocalPermissions("Puma", {"/Administration/Users/View"});

	QCOMPARE(fixture.GetUserObjectId("alice", &manager, managerId), aliceId);
}


void CUserControllerTest::testGetUserObjectId_OtherUserGetsNothing()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");
	const QByteArray malloryId = fixture.AddUser("mallory", "Mallory_1");

	imtauth::CUserInfo mallory;
	mallory.SetId("mallory");
	mallory.SetLocalPermissions("Puma", {"/Administration/Users/ViewPage"});

	QVERIFY(fixture.GetUserObjectId("alice", &mallory, malloryId).isEmpty());
}


void CUserControllerTest::testGetUserObjectId_AnonymousGetsNothing()
{
	CFixture fixture;
	fixture.AddUser("alice", "Old_Pass_1");

	QVERIFY(fixture.GetUserObjectId("alice").isEmpty());
	QVERIFY(fixture.GetUserObjectId("nobody").isEmpty());
}


void CUserControllerTest::testRemoveUsers_RemovesTheInvitationsTheySent()
{
	CRemovalFixture fixture;
	const QByteArray firstUserId = NewId();
	const QByteArray secondUserId = NewId();
	fixture.AddInvitation(firstUserId);
	fixture.AddInvitation(secondUserId);
	const QByteArray othersInvitationId = fixture.AddInvitation(NewId());

	// An invitation is accepted without a look at its inviter, so a removed user's invitations go with the user
	QString errorMessage;
	QVERIFY(fixture.RemoveUsers({firstUserId, secondUserId}, errorMessage));
	QCOMPARE(fixture.invitations->GetElementIds(), QByteArrayList({othersInvitationId}));
}


void CUserControllerTest::testRemoveUsers_FailedReadRemovesNoInvitation()
{
	CRemovalFixture fixture;
	const QByteArray firstUserId = NewId();
	const QByteArray secondUserId = NewId();
	const QByteArray firstInvitationId = fixture.AddInvitation(firstUserId);
	const QByteArray secondInvitationId = fixture.AddInvitation(secondUserId);

	// The second user's invitations cannot be read: the removal is refused, and the first user keeps the invitations too
	fixture.invitations->m_loadFailingIds.insert(secondInvitationId);
	QString errorMessage;
	QVERIFY(!fixture.RemoveUsers({firstUserId, secondUserId}, errorMessage));
	QVERIFY(!errorMessage.isEmpty());
	fixture.invitations->m_loadFailingIds.clear();

	QByteArrayList invitationIds = fixture.invitations->GetElementIds();
	std::sort(invitationIds.begin(), invitationIds.end());
	QByteArrayList expectedIds = {firstInvitationId, secondInvitationId};
	std::sort(expectedIds.begin(), expectedIds.end());
	QCOMPARE(invitationIds, expectedIds);
}


void CUserControllerTest::testRemoveUsers_FailedInvitationRemovalIsRefused()
{
	CRemovalFixture fixture;
	const QByteArray userId = NewId();
	fixture.AddInvitation(userId);

	fixture.invitations->m_isRemovalFailing = true;
	QString errorMessage;
	QVERIFY(!fixture.RemoveUsers({userId}, errorMessage));
	QVERIFY(!errorMessage.isEmpty());

	fixture.invitations->m_isRemovalFailing = false;
	QVERIFY(fixture.RemoveUsers({userId}, errorMessage));
	QVERIFY(fixture.invitations->GetElementIds().isEmpty());
}


void CUserControllerTest::testRemoveUsers_RefusedWhenTheirOrganizationsCannotBeRead()
{
	CRemovalFixture fixture;
	const QByteArray userId = NewId();
	const QByteArray invitationId = fixture.AddInvitation(userId);
	fixture.tenants->m_creatorId = userId;

	// An organization that cannot be read may be owned by the user and would be left with a removed owner
	QString errorMessage;
	fixture.tenants->m_isSelectionFailing = true;
	QVERIFY(!fixture.RemoveUsers({userId}, errorMessage));
	QVERIFY(!errorMessage.isEmpty());
	fixture.tenants->m_isSelectionFailing = false;

	fixture.tenants->m_isLoadFailing = true;
	errorMessage.clear();
	QVERIFY(!fixture.RemoveUsers({userId}, errorMessage));
	QVERIFY(!errorMessage.isEmpty());
	QCOMPARE(fixture.invitations->GetElementIds(), QByteArrayList({invitationId}));
	fixture.tenants->m_isLoadFailing = false;

	// The creator who does not own the organization is removed
	QVERIFY2(fixture.RemoveUsers({userId}, errorMessage), qPrintable(errorMessage));
}


I_ADD_TEST(CUserControllerTest);


