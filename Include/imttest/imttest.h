// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imttest.h
	\brief Main header file for the imttest module - Testing Utilities and Test Management
	
	This file provides namespace-level documentation for the imttest module.
	Include this file to access the complete imttest module documentation in Doxygen.
*/

/**
	\namespace imttest
	\brief Testing Utilities and Test Management Module
	
	The \b imttest module provides testing infrastructure, test data management,
	and utilities for unit testing, integration testing, and test automation
	in ImtCore applications.
	
	\section imttest_overview Overview
	
	This module provides testing support:
	- Test information and metadata
	- Test instance management
	- Test database delegates
	- Test data providers
	- Test collection management
	- Test metadata creation
	
	\section imttest_architecture Architecture
	
	\subsection imttest_patterns Design Patterns
	
	<b>Data Transfer Object (DTO) Pattern:</b>
	- ITestInfo as pure test data container
	- Separate interface from implementation
	- Serializable test information
	- Facilitates test data persistence
	
	<b>Provider Pattern:</b>
	- ITestInfoProvider supplies test data
	- Abstraction over test data sources
	- Enables mock test providers
	- Facilitates test isolation
	
	<b>Delegate Pattern:</b>
	- CTestDatabaseDelegateComp for database persistence
	- SQL query generation for test data
	- Database-agnostic test storage
	
	<b>Meta-Info Pattern:</b>
	- CTestMetaInfo provides test metadata
	- CTestMetaInfoCreatorComp creates metadata
	- Consistent metadata structure
	
	\subsection imttest_core_interfaces Core Interfaces
	
	The module provides test management interfaces:
	
	<b>Test Information:</b>
	\code
	ITestInfo (iser::IObject)
	├─ GetTestId() / SetTestId() - Test identifier
	├─ GetTestName() / SetTestName() - Test name
	├─ GetTestDescription() / SetTestDescription() - Test description
	└─ Extends iser::IObject for serialization
	    │
	    └─ CTestInfo (concrete implementation)
	        ├─ Data object for test information
	        └─ Serializable test data
	\endcode
	
	<b>Test Data Provider:</b>
	\code
	ITestInfoProvider (istd::IPolymorphic)
	├─ GetTestInfo() - Retrieve test information
	├─ GetTestsByFilter() - Query tests
	└─ Test data access abstraction
	\endcode
	
	<b>Test Metadata:</b>
	\code
	CTestMetaInfo (idoc::IDocumentMetaInfo)
	├─ Test metadata structure
	├─ MIT_TEST_ID - Test identifier metadata
	├─ MIT_TEST_NAME - Test name metadata
	└─ MIT_TEST_DESCRIPTION - Test description metadata
	    │
	    └─ CTestMetaInfoCreatorComp - Creates test metadata
	\endcode
	
	<b>Test Database Persistence:</b>
	\code
	CTestDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ CreateNewObjectQuery() - SQL INSERT for tests
	├─ CreateUpdateObjectQuery() - SQL UPDATE for tests
	├─ CreateDeleteObjectsQuery() - SQL DELETE for tests
	└─ Database persistence for test data
	\endcode
	
	\section imttest_usage Usage Examples
	
	\subsection imttest_create_test Creating Test Information
	
	**Create Test Data:**
	\code{.cpp}
	// Create test info
	auto testInfo = CTestInfo::CreateInstance();
	
	// Set test properties
	testInfo->SetTestId("TEST-001");
	testInfo->SetTestName("User Authentication Test");
	testInfo->SetTestDescription(
	    "Validates user authentication with valid and invalid credentials");
	
	// Access test properties
	QByteArray testId = testInfo->GetTestId();
	QString testName = testInfo->GetTestName();
	QString description = testInfo->GetTestDescription();
	
	qDebug() << "Test:" << testName << "ID:" << testId;
	\endcode
	
	\subsection imttest_database_storage Database Storage
	
	**Store Tests in Database:**
	\code{.cpp}
	// Create test collection with database persistence
	auto testCollection = CSqlDatabaseObjectCollectionComp::CreateInstance();
	
	// Configure test database delegate
	auto testDelegate = CTestDatabaseDelegateComp::CreateInstance();
	testCollection->SetAttribute("Delegate", testDelegate.get());
	
	// Configure database connection
	auto databaseEngine = GetDatabaseEngine();
	testCollection->SetAttribute("DatabaseEngine", databaseEngine.get());
	testCollection->SetAttribute("TableName", "tests");
	
	// Add test to collection (automatically persisted)
	testCollection->AddObject(testInfo.get());
	
	// Query tests
	auto filter = CreateFilterParams("test_name", "Authentication%");
	auto authTests = testCollection->GetObjectsByFilter(filter.get());
	
	qDebug() << "Found" << authTests.size() << "authentication tests";
	\endcode
	
	\subsection imttest_metadata Test Metadata
	
	**Create Test Metadata:**
	\code{.cpp}
	// Create metadata creator
	auto metaInfoCreator = CTestMetaInfoCreatorComp::CreateInstance();
	
	// Create metadata for test
	auto testMetaInfo = metaInfoCreator->CreateMetaInfo(testInfo.get());
	
	// Access metadata
	QVariant testIdMeta = testMetaInfo->GetValue(
	    ITestInfo::MIT_TEST_ID);
	QVariant testNameMeta = testMetaInfo->GetValue(
	    ITestInfo::MIT_TEST_NAME);
	QVariant testDescMeta = testMetaInfo->GetValue(
	    ITestInfo::MIT_TEST_DESCRIPTION);
	
	qDebug() << "Test Metadata:"
	         << "ID:" << testIdMeta.toByteArray()
	         << "Name:" << testNameMeta.toString()
	         << "Description:" << testDescMeta.toString();
	\endcode
	
	\subsection imttest_provider Test Provider
	
	**Implement Test Provider:**
	\code{.cpp}
	class CTestSuiteProviderComp : public ACF_COMPONENT(ITestInfoProvider)
	{
	    I_REFERENCE(IObjectCollection, m_testCollection)
	    
	public:
	    ITestInfo* GetTestInfo(const QByteArray& testId) const override
	    {
	        return dynamic_cast<ITestInfo*>(
	            m_testCollection->GetObject(testId));
	    }
	    
	    QList<ITestInfo*> GetTestsByCategory(const QString& category) const
	    {
	        // Filter tests by category
	        auto filter = CreateFilterParams("category", category);
	        auto tests = m_testCollection->GetObjectsByFilter(filter.get());
	        
	        QList<ITestInfo*> result;
	        for (auto test : tests) {
	            result.append(dynamic_cast<ITestInfo*>(test));
	        }
	        return result;
	    }
	    
	    int GetTestCount() const
	    {
	        return m_testCollection->GetIds().size();
	    }
	};
	\endcode
	
	\section imttest_integration Integration Patterns
	
	\subsection imttest_unit_testing Unit Testing Integration
	
	**Pattern: Test Runner with Test Collection:**
	\code{.cpp}
	class CTestRunnerComp : public ACF_COMPONENT(ITestRunner)
	{
	    I_REFERENCE(ITestInfoProvider, m_testProvider)
	    
	public:
	    void RunAllTests()
	    {
	        QList<ITestInfo*> tests = LoadAllTests();
	        
	        int passed = 0;
	        int failed = 0;
	        
	        for (ITestInfo* test : tests) {
	            qDebug() << "Running test:" << test->GetTestName();
	            
	            bool result = ExecuteTest(test);
	            
	            if (result) {
	                passed++;
	                qDebug() << "  PASSED";
	            } else {
	                failed++;
	                qCritical() << "  FAILED";
	            }
	        }
	        
	        qDebug() << "Test Results:"
	                 << "Passed:" << passed
	                 << "Failed:" << failed
	                 << "Total:" << (passed + failed);
	    }
	    
	private:
	    QList<ITestInfo*> LoadAllTests()
	    {
	        // Load all tests from provider
	        return m_testProvider->GetAllTests();
	    }
	    
	    bool ExecuteTest(ITestInfo* test)
	    {
	        // Execute test based on test ID
	        // This would call actual test implementation
	        QString testId = test->GetTestId();
	        return InvokeTestMethod(testId);
	    }
	};
	\endcode
	
	\subsection imttest_test_reporting Test Reporting
	
	**Pattern: Test Results Collection:**
	\code{.cpp}
	class CTestReportGeneratorComp : public ACF_COMPONENT(ITestReportGenerator)
	{
	    I_REFERENCE(ITestInfoProvider, m_testProvider)
	    
	public:
	    void GenerateTestReport(const QString& outputPath)
	    {
	        QList<ITestInfo*> tests = m_testProvider->GetAllTests();
	        
	        // Create HTML report
	        QString html = "<html><head><title>Test Report</title></head><body>";
	        html += "<h1>ImtCore Test Report</h1>";
	        html += "<table border='1'>";
	        html += "<tr><th>Test ID</th><th>Test Name</th>"
	                "<th>Description</th></tr>";
	        
	        for (ITestInfo* test : tests) {
	            html += QString("<tr>"
	                          "<td>%1</td>"
	                          "<td>%2</td>"
	                          "<td>%3</td>"
	                          "</tr>")
	                .arg(QString::fromUtf8(test->GetTestId()))
	                .arg(test->GetTestName())
	                .arg(test->GetTestDescription());
	        }
	        
	        html += "</table></body></html>";
	        
	        // Save report
	        QFile file(outputPath);
	        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
	            file.write(html.toUtf8());
	            file.close();
	            qDebug() << "Test report saved to:" << outputPath;
	        }
	    }
	};
	\endcode
	
	\section imttest_best_practices Best Practices
	
	\subsection imttest_organization Test Organization
	
	- Use consistent test ID naming conventions (e.g., TEST-MODULE-001)
	- Group related tests by category or module
	- Maintain test descriptions with clear objectives
	- Store test data in version control
	- Use database persistence for large test suites
	
	\subsection imttest_maintenance Test Maintenance
	
	- Update test descriptions when functionality changes
	- Archive obsolete tests instead of deleting
	- Version test data alongside application code
	- Document test dependencies and prerequisites
	- Regular review and cleanup of test database
	
	\section imttest_related_modules Integration with Other Modules
	
	<b>With imtdb (Database Layer):</b>
	- Test data persistence in SQL databases
	- Test collection management
	- Query and filter test data
	
	<b>With imtbase (Core Infrastructure):</b>
	- Test collections and filters
	- Serialization of test data
	- ACF component integration
	
	<b>Testing Frameworks:</b>
	- Qt Test framework integration
	- Google Test compatibility
	- Custom test runners
	
	\section imttest_references References
	
	<b>Related Modules:</b>
	- imtdb - Database persistence
	- imtbase - Core collections and serialization
	
	<b>Testing Frameworks:</b>
	- Qt Test: https://doc.qt.io/qt-6/qttest-index.html
	- Google Test: https://github.com/google/googletest
	
	<b>External Documentation:</b>
	- Test-Driven Development (TDD)
	- Unit Testing Best Practices
*/

namespace imttest
{
	// Namespace for testing utilities and test management
}
