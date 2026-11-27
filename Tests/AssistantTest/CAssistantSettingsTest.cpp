// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAssistantSettingsTest.h"


// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <iprm/CEnableableParamComp.h>
#include <iprm/CSelectionParamComp.h>
#include <iprm/CTextParamComp.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtassist/CAssistantActionRegistry.h>
#include <imtassist/CAssistantProviderPresetListComp.h>
#include <imtassist/CAssistantServiceComp.h>
#include <imtassist/CHttpAssistantProviderComp.h>
#include <imtcrypt/CProtectedTextParamComp.h>


using namespace imtassist;


namespace
{


/**
	Reversible test protection; a failing protector refuses everything.
*/
class CTestSecretProtectorComp: public icomp::CComponentBase, virtual public imtcrypt::ISecretProtector
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestSecretProtectorComp);
		I_REGISTER_INTERFACE(imtcrypt::ISecretProtector);
		I_ASSIGN(m_isFailingAttrPtr, "Failing", "Refuse every operation", true, false);
	I_END_COMPONENT;

	// reimplemented (imtcrypt::ISecretProtector)
	virtual bool Protect(const QByteArray& secret, QByteArray& protectedSecret) const override
	{
		if (*m_isFailingAttrPtr){
			return false;
		}

		protectedSecret = "P:" + secret.toHex();

		return true;
	}

	virtual bool Unprotect(const QByteArray& protectedSecret, QByteArray& secret) const override
	{
		if (*m_isFailingAttrPtr || !protectedSecret.startsWith("P:")){
			return false;
		}

		secret = QByteArray::fromHex(protectedSecret.mid(2));

		return true;
	}

private:
	I_ATTR(bool, m_isFailingAttrPtr);
};


typedef icomp::TSimSharedComponentPtr<iprm::CTextParamComp> TextParam;
typedef icomp::TSimSharedComponentPtr<iprm::CSelectionParamComp> SelectionParam;
typedef icomp::TSimSharedComponentPtr<iprm::CEnableableParamComp> EnableableParam;
typedef icomp::TSimSharedComponentPtr<CAssistantProviderPresetListComp> ProviderList;
typedef icomp::TSimSharedComponentPtr<CTestSecretProtectorComp> TestProtector;
typedef icomp::TSimComponentWrap<CHttpAssistantProviderComp> HttpProvider;
typedef icomp::TSimComponentWrap<CAssistantServiceComp> AssistantService;
typedef icomp::TSimComponentWrap<imtcrypt::CProtectedTextParamComp> ProtectedTextParam;


int FindPresetIndex(const iprm::IOptionsList& options, const QByteArray& presetId)
{
	for (int i = 0; i < options.GetOptionsCount(); ++i){
		if (options.GetOptionId(i) == presetId){
			return i;
		}
	}

	return -1;
}


/**
	Local server that records one request and answers with a text reply in the OpenAI format.
*/
struct CLocalModelServer
{
	QTcpServer server;
	QByteArray request;

	CLocalModelServer()
	{
		server.listen(QHostAddress::LocalHost);
		QObject::connect(&server, &QTcpServer::newConnection, [this](){
			QTcpSocket* socketPtr = server.nextPendingConnection();
			QObject::connect(socketPtr, &QTcpSocket::readyRead, [this, socketPtr](){
				request += socketPtr->readAll();
				int headerEnd = request.indexOf("\r\n\r\n");
				int lengthPos = request.toLower().indexOf("content-length:");
				if ((headerEnd < 0) || (lengthPos < 0)){
					return;
				}

				int length = request.mid(lengthPos + 15, request.indexOf("\r\n", lengthPos) - lengthPos - 15).trimmed().toInt();
				if (request.size() < headerEnd + 4 + length){
					return;
				}

				QByteArray body = "{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Hello\"}}]}";
				socketPtr->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
				socketPtr->disconnectFromHost();
			});
		});
	}

	QString Endpoint() const
	{
		return QStringLiteral("http://127.0.0.1:%1/v1/chat/completions").arg(server.serverPort());
	}
};


IAssistantProvider::Request CreateRequest()
{
	IAssistantProvider::Message user;
	user.role = IAssistantProvider::Message::R_USER;
	user.text = "Hello";

	IAssistantProvider::Request retVal;
	retVal.messages << user;
	retVal.timeoutMs = 5000;

	return retVal;
}


} // namespace


void CAssistantSettingsTest::testProviderList_ContainsAllPresets()
{
	ProviderList list;
	list->InitComponent();

	CAssistantProviderPresets::Presets presets = CAssistantProviderPresets::GetPresets();
	QCOMPARE(list->GetOptionsCount(), presets.size());
	QVERIFY(list->GetOptionsFlags() & iprm::IOptionsList::SCF_SUPPORT_UNIQUE_ID);

	for (int i = 0; i < presets.size(); ++i){
		QCOMPARE(list->GetOptionId(i), presets.at(i).id);
		QCOMPARE(list->GetOptionName(i), presets.at(i).name);
		QVERIFY(list->IsOptionEnabled(i));
	}

	QVERIFY(list->GetOptionId(-1).isEmpty());
	QVERIFY(list->GetOptionId(presets.size()).isEmpty());
}


void CAssistantSettingsTest::testProvider_SettingsOverrideAttributes()
{
	CLocalModelServer modelServer;

	ProviderList list;
	list->InitComponent();

	SelectionParam providerParam;
	providerParam->SetRef("Constraints", list);
	providerParam->InitComponent();
	QVERIFY(providerParam->SetSelectedOptionIndex(FindPresetIndex(*list, "openai-compatible")));

	TextParam endpointParam;
	endpointParam->InitComponent();
	endpointParam->SetText(modelServer.Endpoint());

	TextParam modelParam;
	modelParam->InitComponent();
	modelParam->SetText("model-from-settings");

	TextParam apiKeyParam;
	apiKeyParam->InitComponent();

	HttpProvider provider;
	provider.SetIdAttr("Preset", "openai");
	provider.SetIdAttr("Model", "model-from-attribute");
	provider.SetIdAttr("ApiKeyVariable", "IMT_ASSISTANT_SETTINGS_TEST_KEY_UNSET");
	provider.SetRef("ProviderParam", providerParam);
	provider.SetRef("EndpointParam", endpointParam);
	provider.SetRef("ModelParam", modelParam);
	provider.SetRef("ApiKeyParam", apiKeyParam);
	provider.InitComponent();

	// No key in the settings and no environment variable: not configured, no request.
	qunsetenv("IMT_ASSISTANT_SETTINGS_TEST_KEY_UNSET");
	QVERIFY(!provider.IsConfigured());

	apiKeyParam->SetText("key-from-settings");
	QVERIFY(provider.IsConfigured());

	IAssistantProvider::Reply reply = provider.Complete(CreateRequest(), CCancellationToken());
	QCOMPARE(reply.status, IAssistantProvider::RS_OK);
	QCOMPARE(reply.text, QString("Hello"));
	QVERIFY(modelServer.request.contains("Authorization: Bearer key-from-settings"));
	QVERIFY(modelServer.request.contains("\"model\":\"model-from-settings\""));
	QVERIFY(!modelServer.request.contains("model-from-attribute"));
}


void CAssistantSettingsTest::testProvider_SettingsChangeWithoutRestart()
{
	ProviderList list;
	list->InitComponent();

	SelectionParam providerParam;
	providerParam->SetRef("Constraints", list);
	providerParam->InitComponent();

	TextParam regionParam;
	regionParam->InitComponent();

	TextParam apiKeyParam;
	apiKeyParam->InitComponent();
	apiKeyParam->SetText("key");

	HttpProvider provider;
	provider.SetIdAttr("Preset", "openai");
	provider.SetIdAttr("Model", "model-1");
	provider.SetRef("ProviderParam", providerParam);
	provider.SetRef("RegionParam", regionParam);
	provider.SetRef("ApiKeyParam", apiKeyParam);
	provider.InitComponent();

	// Without a selection the attribute is used.
	QVERIFY(provider.IsConfigured());

	// Bedrock needs a region: not configured until the region is set.
	QVERIFY(providerParam->SetSelectedOptionIndex(FindPresetIndex(*list, "bedrock")));
	QVERIFY(!provider.IsConfigured());

	regionParam->SetText("eu-central-1");
	QVERIFY(provider.IsConfigured());

	// An invalid region is rejected.
	regionParam->SetText("eu-central-1.evil.example/x");
	QVERIFY(!provider.IsConfigured());

	// Removing the key from the settings disables the provider again.
	regionParam->SetText("eu-central-1");
	apiKeyParam->SetText("");
	QVERIFY(!provider.IsConfigured());
}


void CAssistantSettingsTest::testService_EnabledBySetting()
{
	CAssistantActionRegistry registry;

	EnableableParam enabledParam;
	enabledParam->InitComponent();
	QVERIFY(!enabledParam->IsEnabled());

	AssistantService service;
	service.SetBoolAttr("Enabled", true);
	service.SetRef("EnabledParam", enabledParam);
	service.InitComponent();
	service.SetActionRegistry(&registry);

	// The setting wins over the attribute.
	QVERIFY(!service.IsEnabled());
	CAssistantRequestContext context("alice", "tenant-a");
	QVERIFY(service.StartConversation(context).isEmpty());

	enabledParam->SetEnabled(true);
	QVERIFY(service.IsEnabled());
	QVERIFY(!service.StartConversation(context).isEmpty());

	// Without the setting the attribute decides; it is off by default.
	AssistantService defaultService;
	defaultService.InitComponent();
	QVERIFY(!defaultService.IsEnabled());
}


void CAssistantSettingsTest::testProtectedTextParam_StoresProtected()
{
	TestProtector protector;
	protector->InitComponent();

	ProtectedTextParam param;
	param.SetRef("SecretProtector", protector);
	param.InitComponent();
	param.SetText("sk-secret-123");

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(param.Serialize(writeArchive));

	QByteArray stored(static_cast<const char*>(writeArchive.GetBuffer()), writeArchive.GetBufferSize());
	QVERIFY(!stored.contains("sk-secret-123"));

	ProtectedTextParam loaded;
	loaded.SetRef("SecretProtector", protector);
	loaded.InitComponent();

	iser::CMemoryReadArchive readArchive(writeArchive);
	QVERIFY(loaded.Serialize(readArchive));
	QCOMPARE(loaded.GetText(), QString("sk-secret-123"));

	// An empty secret stays empty.
	ProtectedTextParam empty;
	empty.SetRef("SecretProtector", protector);
	empty.InitComponent();

	iser::CMemoryWriteArchive emptyArchive;
	QVERIFY(empty.Serialize(emptyArchive));

	iser::CMemoryReadArchive emptyReadArchive(emptyArchive);
	QVERIFY(loaded.Serialize(emptyReadArchive));
	QVERIFY(loaded.GetText().isEmpty());
}


void CAssistantSettingsTest::testProtectedTextParam_RefusesWithoutProtection()
{
	TestProtector failingProtector;
	failingProtector->SetBoolAttr("Failing", true);
	failingProtector->InitComponent();

	ProtectedTextParam param;
	param.SetRef("SecretProtector", failingProtector);
	param.InitComponent();
	param.SetText("sk-secret-123");

	// The secret is not written in clear text when it cannot be protected.
	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(param.Serialize(writeArchive));

	QByteArray stored(static_cast<const char*>(writeArchive.GetBuffer()), writeArchive.GetBufferSize());
	QVERIFY(!stored.contains("sk-secret-123"));

	TestProtector protector;
	protector->InitComponent();

	ProtectedTextParam loaded;
	loaded.SetRef("SecretProtector", protector);
	loaded.InitComponent();

	iser::CMemoryReadArchive readArchive(writeArchive);
	QVERIFY(loaded.Serialize(readArchive));
	QVERIFY(loaded.GetText().isEmpty());
}


I_ADD_TEST(CAssistantSettingsTest);


