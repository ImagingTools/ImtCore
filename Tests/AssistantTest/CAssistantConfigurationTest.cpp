// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAssistantConfigurationTest.h"


// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <istd/TSingleFactory.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtassist/CAssistantActionRegistry.h>
#include <imtassist/CAssistantConfigurationComp.h>
#include <imtassist/CAssistantConfigurationStoreComp.h>
#include <imtassist/CAssistantService.h>
#include <imtassist/CFakeAssistantProvider.h>
#include <imtassist/CTenantAssistantProviderComp.h>
#include <imtcrypt/ISecretProtector.h>


using namespace imtassist;


namespace
{


const QByteArray s_tenantA = "tenant-a";
const QByteArray s_tenantB = "tenant-b";
const QByteArray s_apiKey = "sk-test-secret-value";


class CTestProtectorComp: public icomp::CComponentBase, virtual public imtcrypt::ISecretProtector
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestProtectorComp);
		I_REGISTER_INTERFACE(imtcrypt::ISecretProtector);
	I_END_COMPONENT;

	// reimplemented (imtcrypt::ISecretProtector)
	virtual bool Protect(const QByteArray& secret, QByteArray& protectedSecret) const override
	{
		protectedSecret = "P:" + secret.toHex();

		return true;
	}

	virtual bool Unprotect(const QByteArray& protectedSecret, QByteArray& secret) const override
	{
		if (!protectedSecret.startsWith("P:")){
			return false;
		}

		secret = QByteArray::fromHex(protectedSecret.mid(2));

		return true;
	}
};


class CTestCollectionComp: public icomp::CComponentBase, public imtbase::CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT;

	CTestCollectionComp()
	{
		RegisterFactory<istd::TSingleFactory<istd::IChangeable, CIdentifiableAssistantConfiguration>>("AssistantConfiguration");
	}
};


typedef icomp::TSimSharedComponentPtr<CTestProtectorComp> TestProtector;
typedef icomp::TSimSharedComponentPtr<CTestCollectionComp> TestCollection;
typedef icomp::TSimSharedComponentPtr<CAssistantConfigurationStoreComp> Store;
typedef icomp::TSimComponentWrap<CTenantAssistantProviderComp> TenantProvider;


struct StoreFixture
{
	explicit StoreFixture(bool withProtector = true, bool allowCustomEndpoints = false)
	{
		collection->InitComponent();
		protector->InitComponent();

		store->SetRef("ConfigurationCollection", collection);
		store->SetFactory("ConfigurationFactory", &configurationFactory);
		if (withProtector){
			store->SetRef("SecretProtector", protector);
		}

		store->SetBoolAttr("AllowCustomEndpoints", allowCustomEndpoints);
		store->SetIntAttr("CacheTimeToLive", 60);
		store->InitComponent();
	}

	TestCollection collection;
	TestProtector protector;
	icomp::TSimComponentsFactory<CAssistantConfigurationComp> configurationFactory;
	Store store;
};


CAssistantConfiguration CreateConfiguration(const QByteArray& tenantId, const QByteArray& presetId = "openai", bool isEnabled = true)
{
	CAssistantConfiguration retVal;
	retVal.SetTenantId(tenantId);
	retVal.SetEnabled(isEnabled);
	retVal.SetPresetId(presetId);
	retVal.SetModel(QStringLiteral("test-model"));

	return retVal;
}


bool SetConfiguration(
			StoreFixture& fixture,
			const IAssistantConfiguration& configuration,
			IAssistantConfigurationStore::ApiKeyChange change = IAssistantConfigurationStore::AKC_KEEP,
			const QByteArray& apiKey = QByteArray())
{
	QString error;
	const bool retVal = fixture.store->SetConfiguration(configuration, change, apiKey, error);
	if (!retVal){
		qDebug() << "SetConfiguration refused:" << error;
	}

	return retVal;
}


} // anonymous namespace


void CAssistantConfigurationTest::testConfiguration_SerializeRoundTrip()
{
	CAssistantConfiguration configuration = CreateConfiguration(s_tenantA, "vertex-gemini");
	configuration.SetRegion(QStringLiteral("europe-west4"));
	configuration.SetProject(QStringLiteral("project-1"));
	configuration.SetProtectedApiKey("P:00ff");
	configuration.SetMaxOutputTokens(2048);
	configuration.SetAdditionalInstructions(QStringLiteral("Antworte auf Deutsch."));
	configuration.SetUpdatedAt(QDateTime(QDate(2026, 11, 27), QTime(12, 0), Qt::UTC));

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(configuration.Serialize(writeArchive));

	CAssistantConfiguration loaded;
	iser::CMemoryReadArchive readArchive(writeArchive);
	QVERIFY(loaded.Serialize(readArchive));
	QVERIFY(loaded.IsEqual(configuration));

	QVERIFY(loaded.ResetData());
	QVERIFY(!loaded.IsEnabled());
	QCOMPARE(loaded.GetMaxOutputTokens(), int(IAssistantConfiguration::DEFAULT_MAX_OUTPUT_TOKENS));
}


void CAssistantConfigurationTest::testStore_DefaultIsDisabled()
{
	StoreFixture fixture;

	IAssistantConfigurationUniquePtr configurationPtr = fixture.store->GetConfiguration(s_tenantA);
	QVERIFY(configurationPtr.IsValid());
	QCOMPARE(configurationPtr->GetTenantId(), s_tenantA);
	QVERIFY(!configurationPtr->IsEnabled());
	QVERIFY(configurationPtr->GetPresetId().isEmpty());
	QVERIFY(!fixture.store->IsAssistantEnabledForTenant(s_tenantA));

	QVERIFY(!fixture.store->GetConfiguration(QByteArray()).IsValid());
	QVERIFY(!fixture.store->IsAssistantEnabledForTenant(QByteArray()));
}


void CAssistantConfigurationTest::testStore_StoresApiKeyProtected()
{
	StoreFixture fixture;

	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, s_apiKey));

	// The collection holds only the protected key.
	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(fixture.collection->GetObjectData(s_tenantA, dataPtr));
	const IAssistantConfiguration* storedPtr = dynamic_cast<const IAssistantConfiguration*>(dataPtr.GetPtr());
	QVERIFY(storedPtr != nullptr);
	QVERIFY(!storedPtr->GetProtectedApiKey().isEmpty());
	QVERIFY(!storedPtr->GetProtectedApiKey().contains(s_apiKey));

	iser::CMemoryWriteArchive archive;
	QVERIFY(const_cast<IAssistantConfiguration*>(storedPtr)->Serialize(archive));
	QVERIFY(!QByteArray(static_cast<const char*>(archive.GetBuffer()), archive.GetBufferSize()).contains(s_apiKey));

	IAssistantConfigurationUniquePtr configurationPtr = fixture.store->GetConfiguration(s_tenantA);
	QVERIFY(configurationPtr.IsValid());
	QVERIFY(configurationPtr->IsEnabled());
	QVERIFY(configurationPtr->GetUpdatedAt().isValid());

	QByteArray apiKey;
	QVERIFY(fixture.store->ReadApiKey(*configurationPtr, apiKey));
	QCOMPARE(apiKey, s_apiKey);
	QVERIFY(fixture.store->IsAssistantEnabledForTenant(s_tenantA));
}


void CAssistantConfigurationTest::testStore_KeepsAndClearsApiKey()
{
	StoreFixture fixture;

	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, s_apiKey));

	// A changed configuration without a key keeps the stored key, whatever the client sent as protected key.
	CAssistantConfiguration changed = CreateConfiguration(s_tenantA);
	changed.SetModel(QStringLiteral("other-model"));
	changed.SetProtectedApiKey("P:6576696c");
	QVERIFY(SetConfiguration(fixture, changed));

	IAssistantConfigurationUniquePtr configurationPtr = fixture.store->GetConfiguration(s_tenantA);
	QVERIFY(configurationPtr.IsValid());
	QCOMPARE(configurationPtr->GetModel(), QStringLiteral("other-model"));

	QByteArray apiKey;
	QVERIFY(fixture.store->ReadApiKey(*configurationPtr, apiKey));
	QCOMPARE(apiKey, s_apiKey);

	QVERIFY(SetConfiguration(fixture, changed, IAssistantConfigurationStore::AKC_CLEAR));
	configurationPtr = fixture.store->GetConfiguration(s_tenantA);
	QVERIFY(configurationPtr.IsValid());
	QVERIFY(configurationPtr->GetProtectedApiKey().isEmpty());
	QVERIFY(fixture.store->ReadApiKey(*configurationPtr, apiKey));
	QVERIFY(apiKey.isEmpty());

	// An empty new key is refused, the configuration stays unchanged.
	QVERIFY(!SetConfiguration(fixture, CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, QByteArray()));
	configurationPtr = fixture.store->GetConfiguration(s_tenantA);
	QCOMPARE(configurationPtr->GetModel(), QStringLiteral("other-model"));
}


void CAssistantConfigurationTest::testStore_RefusesApiKeyWithoutProtector()
{
	StoreFixture fixture(false);

	QString error;
	QVERIFY(!fixture.store->SetConfiguration(CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, s_apiKey, error));
	QVERIFY(!error.isEmpty());
	QVERIFY(!error.contains(QString::fromUtf8(s_apiKey)));
	QVERIFY(!fixture.collection->GetElementIds().contains(s_tenantA));

	// Without key a configuration can be stored, e.g. for a local model server.
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA)));
}


void CAssistantConfigurationTest::testStore_PolicyRefusesPresetsAndEndpoints()
{
	StoreFixture fixture;

	const QByteArrayList allowed = fixture.store->GetAllowedPresetIds();
	QVERIFY(allowed.contains("openai"));
	QVERIFY(allowed.contains("anthropic"));
	QVERIFY(allowed.contains("bedrock"));
	QVERIFY(!allowed.contains("openai-compatible"));
	QVERIFY(!allowed.contains("ollama"));
	QVERIFY(!fixture.store->AreCustomEndpointsAllowed());

	QVERIFY(!SetConfiguration(fixture, CreateConfiguration(s_tenantA, "ollama")));
	QVERIFY(!SetConfiguration(fixture, CreateConfiguration(s_tenantA, "unknown-provider")));

	CAssistantConfiguration withEndpoint = CreateConfiguration(s_tenantA);
	withEndpoint.SetEndpoint(QStringLiteral("https://models.example.com/v1/chat/completions"));
	QString error;
	QVERIFY(!fixture.store->SetConfiguration(withEndpoint, IAssistantConfigurationStore::AKC_KEEP, QByteArray(), error));
	QVERIFY(!error.contains(QStringLiteral("example.com")));

	QVERIFY(!fixture.store->IsAssistantEnabledForTenant(s_tenantA));
}


void CAssistantConfigurationTest::testStore_CustomEndpointsWhenAllowed()
{
	StoreFixture fixture(true, true);

	CAssistantConfiguration withEndpoint = CreateConfiguration(s_tenantA);
	withEndpoint.SetEndpoint(QStringLiteral("https://models.example.com/v1/chat/completions"));
	QVERIFY(SetConfiguration(fixture, withEndpoint));

	// Only https, or http of the local machine.
	withEndpoint.SetEndpoint(QStringLiteral("http://models.example.com/v1/chat/completions"));
	QVERIFY(!SetConfiguration(fixture, withEndpoint));
}


void CAssistantConfigurationTest::testStore_RefusesInvalidValues()
{
	StoreFixture fixture;

	// Enabled without provider
	QVERIFY(!SetConfiguration(fixture, CreateConfiguration(s_tenantA, QByteArray())));

	// Disabled without provider is a valid configuration
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA, QByteArray(), false)));

	CAssistantConfiguration configuration = CreateConfiguration(s_tenantA, "bedrock");
	configuration.SetRegion(QStringLiteral("eu-central-1"));
	QVERIFY(SetConfiguration(fixture, configuration));

	// A dot would move the request to another host of the domain of the provider.
	configuration.SetRegion(QStringLiteral("bucket.s3-website-eu-central-1"));
	QVERIFY(!SetConfiguration(fixture, configuration));

	configuration.SetRegion(QStringLiteral("eu-central-1"));
	configuration.SetMaxOutputTokens(0);
	QVERIFY(!SetConfiguration(fixture, configuration));

	configuration.SetMaxOutputTokens(1024);
	configuration.SetAdditionalInstructions(QString(CAssistantConfigurationStoreComp::MAX_INSTRUCTIONS_LENGTH + 1, QLatin1Char('x')));
	QVERIFY(!SetConfiguration(fixture, configuration));

	// Missing region of a preset that needs one
	CAssistantConfiguration withoutRegion = CreateConfiguration(s_tenantA, "bedrock");
	QVERIFY(!SetConfiguration(fixture, withoutRegion));

	QVERIFY(!SetConfiguration(fixture, CreateConfiguration(QByteArray())));
}


void CAssistantConfigurationTest::testStore_TenantsAreIsolated()
{
	StoreFixture fixture;

	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, s_apiKey));

	QVERIFY(fixture.store->IsAssistantEnabledForTenant(s_tenantA));
	QVERIFY(!fixture.store->IsAssistantEnabledForTenant(s_tenantB));

	IAssistantConfigurationUniquePtr configurationPtr = fixture.store->GetConfiguration(s_tenantB);
	QVERIFY(configurationPtr.IsValid());
	QVERIFY(configurationPtr->GetProtectedApiKey().isEmpty());

	// Disabling is seen at once, despite the cache.
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA, "openai", false)));
	QVERIFY(!fixture.store->IsAssistantEnabledForTenant(s_tenantA));
}


void CAssistantConfigurationTest::testService_EnabledPerTenant()
{
	StoreFixture fixture;
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA), IAssistantConfigurationStore::AKC_SET, s_apiKey));

	CFakeAssistantProvider provider;
	provider.SetDefaultReply(CFakeAssistantProvider::CreateTextReply(QStringLiteral("Hello")));
	CAssistantActionRegistry registry;

	CAssistantService service;
	service.SetEnabled(true);
	service.SetProvider(&provider);
	service.SetActionRegistry(&registry);
	service.SetTenantPolicy(&fixture.store.GetImpl());

	const CAssistantRequestContext contextA("alice", s_tenantA);
	const CAssistantRequestContext contextB("bob", s_tenantB);

	QVERIFY(service.IsAvailable(contextA));
	QVERIFY(!service.IsAvailable(contextB));
	QVERIFY(!service.IsAvailable(CAssistantRequestContext()));

	const QByteArray conversationA = service.StartConversation(contextA);
	QVERIFY(!conversationA.isEmpty());
	QVERIFY(service.StartConversation(contextB).isEmpty());

	// Disabled by the administration of the tenant: a running conversation is refused, too.
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA, "openai", false)));
	QVERIFY(!service.IsAvailable(contextA));

	const AssistantMessages messages = service.SendUserMessage(contextA, conversationA, QStringLiteral("Hi"));
	bool isRejected = false;
	for (const CAssistantMessage& message: messages){
		isRejected = isRejected || ((message.GetKind() == CAssistantMessage::MK_ERROR) && (message.GetCode() == "AssistantDisabled"));
	}

	QVERIFY(isRejected);

	// The server-wide switch still applies.
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA)));
	QVERIFY(service.IsAvailable(contextA));
	service.SetEnabled(false);
	QVERIFY(!service.IsAvailable(contextA));

	service.SetTenantPolicy(nullptr);
}


void CAssistantConfigurationTest::testTenantProvider_NotConfiguredTenant()
{
	StoreFixture fixture;
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA, "openai", false)));

	TenantProvider provider;
	provider.SetRef("ConfigurationStore", fixture.store);
	provider.SetBoolAttr("AllowServerCredentials", false);
	provider.InitComponent();

	QVERIFY(provider.IsConfigured());

	IAssistantProvider::Request request;
	request.tenantId = s_tenantA;
	CCancellationToken token;
	QCOMPARE(provider.Complete(request, token).status, IAssistantProvider::RS_NOT_CONFIGURED);

	request.tenantId = s_tenantB;
	QCOMPARE(provider.Complete(request, token).status, IAssistantProvider::RS_NOT_CONFIGURED);

	request.tenantId.clear();
	QCOMPARE(provider.Complete(request, token).status, IAssistantProvider::RS_NOT_CONFIGURED);

	// Enabled, but without API key and without server credentials: no request is sent.
	qputenv("IMT_ASSISTANT_API_KEY", "server-key");
	QVERIFY(SetConfiguration(fixture, CreateConfiguration(s_tenantA)));
	request.tenantId = s_tenantA;
	QCOMPARE(provider.Complete(request, token).status, IAssistantProvider::RS_NOT_CONFIGURED);
	qunsetenv("IMT_ASSISTANT_API_KEY");

	// In the service the tenant gets a clear error.
	CAssistantActionRegistry registry;
	CAssistantService service;
	service.SetEnabled(true);
	service.SetProvider(&provider);
	service.SetActionRegistry(&registry);
	service.SetTenantPolicy(&fixture.store.GetImpl());

	const CAssistantRequestContext context("alice", s_tenantA);
	const QByteArray conversationId = service.StartConversation(context);
	QVERIFY(!conversationId.isEmpty());

	const AssistantMessages messages = service.SendUserMessage(context, conversationId, QStringLiteral("Hi"));
	bool isNotConfigured = false;
	for (const CAssistantMessage& message: messages){
		isNotConfigured = isNotConfigured || ((message.GetKind() == CAssistantMessage::MK_ERROR) && (message.GetCode() == "ProviderNotConfigured"));
	}

	QVERIFY(isNotConfigured);

	service.SetTenantPolicy(nullptr);
	service.SetProvider(nullptr);
}


I_ADD_TEST(CAssistantConfigurationTest);


