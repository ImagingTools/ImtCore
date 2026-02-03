#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtfunc/CFunctionalityManagerComp.h>
#include <imtfunc/CFunctionalityInfo.h>
#include <imtfunc/CSecurityComplianceDocumentationProviderComp.h>


namespace imtfunctest
{


/**
	Test class for IEC 62443-4-2 CR 3.7 functionality management system.
	Tests functionality registration, enable/disable operations, dependency validation,
	and compliance documentation generation.
*/
class CFunctionalityManagerTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	
	// Functionality registration tests
	void testRegisterFunctionality();
	void testUnregisterFunctionality();
	void testGetFunctionalityInfo();
	void testGetAllFunctionality();
	
	// State management tests
	void testIsFunctionalityEnabled();
	void testSetFunctionalityEnabled();
	void testCanDisableFunctionality();
	void testGetEnabledFunctionality();
	void testGetDisabledFunctionality();
	
	// Dependency tests
	void testDependencyEnforcement();
	void testDependentFunctionality();
	void testCannotDisableWithDependents();
	
	// Configuration tests
	void testResetToDefaults();
	void testValidateConfiguration();
	void testRequiredFunctionality();
	
	// Compliance documentation tests
	void testComplianceStatus();
	void testGenerateComplianceReport();
	void testDisabledFunctionalityDocumentation();

private:
	imtfunc::CFunctionalityManagerComp* m_pManager;
	imtfunc::CSecurityComplianceDocumentationProviderComp* m_pDocProvider;
};


} // namespace imtfunctest


