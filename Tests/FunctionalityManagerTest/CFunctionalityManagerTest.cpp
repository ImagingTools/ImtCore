#include "CFunctionalityManagerTest.h"


// Qt includes
#include <QtCore/QDebug>


namespace imtfunctest
{


void CFunctionalityManagerTest::initTestCase()
{
	// Create functionality manager
	m_pManager = new imtfunc::CFunctionalityManagerComp();
	QVERIFY2(m_pManager != nullptr, "Failed to create functionality manager");
	
	// Create documentation provider
	m_pDocProvider = new imtfunc::CSecurityComplianceDocumentationProviderComp();
	QVERIFY2(m_pDocProvider != nullptr, "Failed to create documentation provider");
	m_pDocProvider->SetFunctionalityManager(m_pManager);
}


void CFunctionalityManagerTest::cleanupTestCase()
{
	delete m_pDocProvider;
	m_pDocProvider = nullptr;
	
	delete m_pManager;
	m_pManager = nullptr;
}


void CFunctionalityManagerTest::testRegisterFunctionality()
{
	// Create and register a functionality
	auto* pInfo = new imtfunc::CFunctionalityInfo(
		"protocol.network.tcp",
		"TCP Protocol",
		"Transmission Control Protocol",
		imtfunc::FunctionalityType::PROTOCOL,
		true,   // enabled by default
		true,   // required
		2);     // medium-high risk
	
	bool registered = m_pManager->RegisterFunctionality(pInfo);
	QVERIFY2(registered, "Failed to register functionality");
	
	// Try to register same ID again - should fail
	auto* pDuplicate = new imtfunc::CFunctionalityInfo(
		"protocol.network.tcp",
		"TCP Duplicate",
		"Duplicate",
		imtfunc::FunctionalityType::PROTOCOL,
		true, true, 2);
	
	bool shouldFail = m_pManager->RegisterFunctionality(pDuplicate);
	QVERIFY2(!shouldFail, "Duplicate registration should fail");
	delete pDuplicate;
}


void CFunctionalityManagerTest::testUnregisterFunctionality()
{
	// Register a functionality
	auto* pInfo = new imtfunc::CFunctionalityInfo(
		"test.unregister",
		"Test Unregister",
		"Test functionality for unregistration",
		imtfunc::FunctionalityType::FEATURE,
		false, false, 1);
	
	m_pManager->RegisterFunctionality(pInfo);
	
	// Verify it exists
	imtfunc::IFunctionalityInfo* pRetrieved = m_pManager->GetFunctionalityInfo("test.unregister");
	QVERIFY2(pRetrieved != nullptr, "Functionality not found after registration");
	
	// Unregister it
	bool unregistered = m_pManager->UnregisterFunctionality("test.unregister");
	QVERIFY2(unregistered, "Failed to unregister functionality");
	
	// Verify it's gone
	pRetrieved = m_pManager->GetFunctionalityInfo("test.unregister");
	QVERIFY2(pRetrieved == nullptr, "Functionality still exists after unregistration");
	
	// Try to unregister non-existent - should fail
	bool shouldFail = m_pManager->UnregisterFunctionality("does.not.exist");
	QVERIFY2(!shouldFail, "Unregistering non-existent should fail");
}


void CFunctionalityManagerTest::testGetFunctionalityInfo()
{
	// Register a functionality
	auto* pInfo = new imtfunc::CFunctionalityInfo(
		"test.getinfo",
		"Test GetInfo",
		"Test description",
		imtfunc::FunctionalityType::SERVICE,
		true, false, 1);
	
	m_pManager->RegisterFunctionality(pInfo);
	
	// Retrieve it
	imtfunc::IFunctionalityInfo* pRetrieved = m_pManager->GetFunctionalityInfo("test.getinfo");
	QVERIFY2(pRetrieved != nullptr, "Failed to retrieve functionality");
	QCOMPARE(pRetrieved->GetName(), QString("Test GetInfo"));
	QCOMPARE(pRetrieved->GetType(), imtfunc::FunctionalityType::SERVICE);
}


void CFunctionalityManagerTest::testGetAllFunctionality()
{
	// Clear by resetting
	m_pManager->ResetToDefaults();
	
	// Register multiple functionalities
	auto* pFeature = new imtfunc::CFunctionalityInfo(
		"test.feature1", "Feature 1", "Desc", imtfunc::FunctionalityType::FEATURE, true, false, 1);
	auto* pProtocol = new imtfunc::CFunctionalityInfo(
		"test.protocol1", "Protocol 1", "Desc", imtfunc::FunctionalityType::PROTOCOL, true, false, 2);
	auto* pService = new imtfunc::CFunctionalityInfo(
		"test.service1", "Service 1", "Desc", imtfunc::FunctionalityType::SERVICE, true, false, 2);
	
	m_pManager->RegisterFunctionality(pFeature);
	m_pManager->RegisterFunctionality(pProtocol);
	m_pManager->RegisterFunctionality(pService);
	
	// Get all functionality
	QByteArrayList all = m_pManager->GetAllFunctionality();
	QVERIFY2(all.size() >= 3, "Should have at least 3 registered functionality");
	
	// Get only protocols
	QByteArrayList protocols = m_pManager->GetAllFunctionality(
		static_cast<int>(imtfunc::FunctionalityType::PROTOCOL));
	QVERIFY2(protocols.contains("test.protocol1"), "Protocol list should contain test.protocol1");
}


void CFunctionalityManagerTest::testIsFunctionalityEnabled()
{
	// Register enabled by default
	auto* pEnabled = new imtfunc::CFunctionalityInfo(
		"test.enabled", "Enabled", "Desc", imtfunc::FunctionalityType::FEATURE, true, false, 1);
	m_pManager->RegisterFunctionality(pEnabled);
	
	// Register disabled by default
	auto* pDisabled = new imtfunc::CFunctionalityInfo(
		"test.disabled", "Disabled", "Desc", imtfunc::FunctionalityType::FEATURE, false, false, 1);
	m_pManager->RegisterFunctionality(pDisabled);
	
	// Check states
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.enabled"), "Should be enabled by default");
	QVERIFY2(!m_pManager->IsFunctionalityEnabled("test.disabled"), "Should be disabled by default");
	
	// Non-existent should return false
	QVERIFY2(!m_pManager->IsFunctionalityEnabled("does.not.exist"), "Non-existent should be disabled");
}


void CFunctionalityManagerTest::testSetFunctionalityEnabled()
{
	// Register optional functionality
	auto* pOptional = new imtfunc::CFunctionalityInfo(
		"test.optional", "Optional", "Desc", imtfunc::FunctionalityType::FEATURE, true, false, 1);
	m_pManager->RegisterFunctionality(pOptional);
	
	// Initially enabled
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.optional"), "Should be enabled initially");
	
	// Disable it
	bool disabled = m_pManager->SetFunctionalityEnabled("test.optional", false, "Test disable");
	QVERIFY2(disabled, "Failed to disable functionality");
	QVERIFY2(!m_pManager->IsFunctionalityEnabled("test.optional"), "Should be disabled after SetFunctionalityEnabled");
	
	// Enable it again
	bool enabled = m_pManager->SetFunctionalityEnabled("test.optional", true, "Test enable");
	QVERIFY2(enabled, "Failed to enable functionality");
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.optional"), "Should be enabled after SetFunctionalityEnabled");
}


void CFunctionalityManagerTest::testCanDisableFunctionality()
{
	// Register required functionality
	auto* pRequired = new imtfunc::CFunctionalityInfo(
		"test.required", "Required", "Desc", imtfunc::FunctionalityType::FEATURE, true, true, 1);
	m_pManager->RegisterFunctionality(pRequired);
	
	// Register optional functionality
	auto* pOptional = new imtfunc::CFunctionalityInfo(
		"test.optional2", "Optional", "Desc", imtfunc::FunctionalityType::FEATURE, true, false, 1);
	m_pManager->RegisterFunctionality(pOptional);
	
	// Required cannot be disabled
	QVERIFY2(!m_pManager->CanDisableFunctionality("test.required"), 
		"Required functionality should not be disableable");
	
	// Optional can be disabled
	QVERIFY2(m_pManager->CanDisableFunctionality("test.optional2"), 
		"Optional functionality should be disableable");
}


void CFunctionalityManagerTest::testGetEnabledFunctionality()
{
	// Register mix of enabled and disabled
	auto* pEnabled1 = new imtfunc::CFunctionalityInfo(
		"test.enabled1", "Enabled 1", "Desc", imtfunc::FunctionalityType::FEATURE, true, false, 1);
	auto* pEnabled2 = new imtfunc::CFunctionalityInfo(
		"test.enabled2", "Enabled 2", "Desc", imtfunc::FunctionalityType::PROTOCOL, true, false, 2);
	auto* pDisabled1 = new imtfunc::CFunctionalityInfo(
		"test.disabled1", "Disabled 1", "Desc", imtfunc::FunctionalityType::FEATURE, false, false, 1);
	
	m_pManager->RegisterFunctionality(pEnabled1);
	m_pManager->RegisterFunctionality(pEnabled2);
	m_pManager->RegisterFunctionality(pDisabled1);
	
	// Get all enabled
	QByteArrayList enabled = m_pManager->GetEnabledFunctionality();
	QVERIFY2(enabled.contains("test.enabled1"), "Should contain test.enabled1");
	QVERIFY2(enabled.contains("test.enabled2"), "Should contain test.enabled2");
	QVERIFY2(!enabled.contains("test.disabled1"), "Should not contain test.disabled1");
	
	// Get only enabled features
	QByteArrayList enabledFeatures = m_pManager->GetEnabledFunctionality(
		static_cast<int>(imtfunc::FunctionalityType::FEATURE));
	QVERIFY2(enabledFeatures.contains("test.enabled1"), "Should contain test.enabled1");
	QVERIFY2(!enabledFeatures.contains("test.enabled2"), "Should not contain protocol in features list");
}


void CFunctionalityManagerTest::testGetDisabledFunctionality()
{
	// Get disabled functionality
	QByteArrayList disabled = m_pManager->GetDisabledFunctionality();
	QVERIFY2(disabled.contains("test.disabled1"), "Should contain test.disabled1");
	QVERIFY2(!disabled.contains("test.enabled1"), "Should not contain test.enabled1");
}


void CFunctionalityManagerTest::testDependencyEnforcement()
{
	// Register base functionality
	auto* pBase = new imtfunc::CFunctionalityInfo(
		"test.base", "Base", "Base functionality", 
		imtfunc::FunctionalityType::PROTOCOL, false, false, 2);
	m_pManager->RegisterFunctionality(pBase);
	
	// Register dependent functionality
	auto* pDependent = new imtfunc::CFunctionalityInfo(
		"test.dependent", "Dependent", "Depends on base",
		imtfunc::FunctionalityType::FEATURE, false, false, 2);
	pDependent->AddDependency("test.base");
	m_pManager->RegisterFunctionality(pDependent);
	
	// Try to enable dependent without enabling base - should fail
	bool shouldFail = m_pManager->SetFunctionalityEnabled("test.dependent", true);
	QVERIFY2(!shouldFail, "Should not enable dependent without dependencies");
	
	// Enable base first
	bool baseEnabled = m_pManager->SetFunctionalityEnabled("test.base", true);
	QVERIFY2(baseEnabled, "Should enable base");
	
	// Now enable dependent - should succeed
	bool dependentEnabled = m_pManager->SetFunctionalityEnabled("test.dependent", true);
	QVERIFY2(dependentEnabled, "Should enable dependent when dependencies are met");
}


void CFunctionalityManagerTest::testDependentFunctionality()
{
	// Get dependents of base
	QByteArrayList dependents = m_pManager->GetDependentFunctionality("test.base");
	QVERIFY2(dependents.contains("test.dependent"), "Should list test.dependent as dependent");
}


void CFunctionalityManagerTest::testCannotDisableWithDependents()
{
	// With both enabled, should not be able to disable base
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.base"), "Base should be enabled");
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.dependent"), "Dependent should be enabled");
	
	bool canDisable = m_pManager->CanDisableFunctionality("test.base");
	QVERIFY2(!canDisable, "Should not be able to disable base with enabled dependents");
	
	// Disable dependent first
	m_pManager->SetFunctionalityEnabled("test.dependent", false);
	
	// Now should be able to disable base
	canDisable = m_pManager->CanDisableFunctionality("test.base");
	QVERIFY2(canDisable, "Should be able to disable base after disabling dependents");
}


void CFunctionalityManagerTest::testResetToDefaults()
{
	// Modify some states
	m_pManager->SetFunctionalityEnabled("test.optional", false);
	QVERIFY2(!m_pManager->IsFunctionalityEnabled("test.optional"), "Should be disabled");
	
	// Reset to defaults
	bool reset = m_pManager->ResetToDefaults();
	QVERIFY2(reset, "Reset should succeed");
	
	// Check that state is restored
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.optional"), 
		"Should be enabled after reset (was enabled by default)");
}


void CFunctionalityManagerTest::testValidateConfiguration()
{
	// Create invalid configuration
	auto* pBroken = new imtfunc::CFunctionalityInfo(
		"test.broken", "Broken", "Has unmet dependency",
		imtfunc::FunctionalityType::FEATURE, true, false, 1);
	pBroken->AddDependency("does.not.exist");
	m_pManager->RegisterFunctionality(pBroken);
	
	// Validation should find issues
	QByteArrayList issues = m_pManager->ValidateConfiguration();
	QVERIFY2(!issues.isEmpty(), "Should detect configuration issues");
	
	// Check that the issue mentions the broken functionality
	bool foundIssue = false;
	for (const QByteArray& issue : issues)
	{
		if (issue.contains("test.broken"))
		{
			foundIssue = true;
			break;
		}
	}
	QVERIFY2(foundIssue, "Should report issue with test.broken");
}


void CFunctionalityManagerTest::testRequiredFunctionality()
{
	// Try to disable required functionality
	auto* pRequired2 = new imtfunc::CFunctionalityInfo(
		"test.required2", "Required 2", "Required functionality",
		imtfunc::FunctionalityType::PROTOCOL, true, true, 2);
	m_pManager->RegisterFunctionality(pRequired2);
	
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.required2"), "Required should be enabled");
	
	// Try to disable - should fail
	bool shouldFail = m_pManager->SetFunctionalityEnabled("test.required2", false);
	QVERIFY2(!shouldFail, "Should not be able to disable required functionality");
	
	// Should still be enabled
	QVERIFY2(m_pManager->IsFunctionalityEnabled("test.required2"), 
		"Required should still be enabled after failed disable");
}


void CFunctionalityManagerTest::testComplianceStatus()
{
	imtfunc::ComplianceStatus status = m_pDocProvider->GetComplianceStatus("IEC 62443-4-2 CR 3.7");
	
	QCOMPARE(status.requirementId, QString("IEC 62443-4-2 CR 3.7"));
	QCOMPARE(status.requirementName, QString("Least Functionality"));
	QVERIFY2(!status.description.isEmpty(), "Should have description");
	QVERIFY2(!status.status.isEmpty(), "Should have status");
	QVERIFY2(!status.evidence.isEmpty(), "Should have evidence");
}


void CFunctionalityManagerTest::testGenerateComplianceReport()
{
	QString report = m_pDocProvider->GenerateComplianceReport("IEC 62443-4-2", "MARKDOWN");
	
	QVERIFY2(!report.isEmpty(), "Report should not be empty");
	QVERIFY2(report.contains("IEC 62443-4-2"), "Report should contain standard name");
	QVERIFY2(report.contains("CR 3.7") || report.contains("Least Functionality"), 
		"Report should mention CR 3.7 or Least Functionality");
	
	// Test HTML format
	QString htmlReport = m_pDocProvider->GenerateComplianceReport("IEC 62443-4-2", "HTML");
	QVERIFY2(htmlReport.contains("<html>"), "HTML report should contain HTML tags");
	
	// Test TEXT format
	QString textReport = m_pDocProvider->GenerateComplianceReport("IEC 62443-4-2", "TEXT");
	QVERIFY2(!textReport.isEmpty(), "Text report should not be empty");
}


void CFunctionalityManagerTest::testDisabledFunctionalityDocumentation()
{
	QString doc = m_pDocProvider->GetDisabledFunctionalityDocumentation("MARKDOWN");
	
	QVERIFY2(!doc.isEmpty(), "Documentation should not be empty");
	QVERIFY2(doc.contains("Disabled Functionality") || doc.contains("disabled"), 
		"Should mention disabled functionality");
}


} // namespace imtfunctest

