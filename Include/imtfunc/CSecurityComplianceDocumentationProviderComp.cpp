// Qt includes
#include <QtCore/QStringBuilder>
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtfunc/CSecurityComplianceDocumentationProviderComp.h>


namespace imtfunc
{


CSecurityComplianceDocumentationProviderComp::CSecurityComplianceDocumentationProviderComp()
	: ilog::CLoggerComponentBase()
	, m_pFunctionalityManager(nullptr)
{
}


CSecurityComplianceDocumentationProviderComp::~CSecurityComplianceDocumentationProviderComp()
{
}


void CSecurityComplianceDocumentationProviderComp::SetFunctionalityManager(IFunctionalityManager* pManager)
{
	m_pFunctionalityManager = pManager;
}


ComplianceStatus CSecurityComplianceDocumentationProviderComp::GetComplianceStatus(const QString& requirementId) const
{
	if (requirementId == "IEC 62443-4-2 CR 3.7")
	{
		return GenerateCR37ComplianceStatus();
	}
	
	// Unknown requirement
	ComplianceStatus status;
	status.requirementId = requirementId;
	status.requirementName = "Unknown";
	status.status = "UNKNOWN";
	status.description = "Requirement not supported";
	return status;
}


QStringList CSecurityComplianceDocumentationProviderComp::GetSupportedRequirements() const
{
	return QStringList() << "IEC 62443-4-2 CR 3.7";
}


QString CSecurityComplianceDocumentationProviderComp::GenerateComplianceReport(const QString& standardId, const QString& format) const
{
	QString report;
	
	if (format == "MARKDOWN")
	{
		report += "# Security Compliance Report\n\n";
		report += "**Standard:** " + standardId + "\n\n";
		report += "**Generated:** " + QDateTime::currentDateTime().toString(Qt::ISODate) + "\n\n";
		
		if (standardId.contains("IEC 62443-4-2"))
		{
			ComplianceStatus cr37 = GetComplianceStatus("IEC 62443-4-2 CR 3.7");
			
			report += "## " + cr37.requirementName + "\n\n";
			report += "**Status:** " + cr37.status + "\n\n";
			report += "**Description:** " + cr37.description + "\n\n";
			
			if (!cr37.evidence.isEmpty())
			{
				report += "### Evidence\n\n";
				report += cr37.evidence + "\n\n";
			}
			
			if (!cr37.gaps.isEmpty())
			{
				report += "### Gaps\n\n";
				for (const QString& gap : cr37.gaps)
				{
					report += "- " + gap + "\n";
				}
				report += "\n";
			}
			
			if (!cr37.recommendations.isEmpty())
			{
				report += "### Recommendations\n\n";
				for (const QString& rec : cr37.recommendations)
				{
					report += "- " + rec + "\n";
				}
				report += "\n";
			}
		}
	}
	else if (format == "HTML")
	{
		report += "<html><head><title>Security Compliance Report</title></head><body>\n";
		report += "<h1>Security Compliance Report</h1>\n";
		report += "<p><strong>Standard:</strong> " + standardId + "</p>\n";
		report += "<p><strong>Generated:</strong> " + QDateTime::currentDateTime().toString(Qt::ISODate) + "</p>\n";
		
		if (standardId.contains("IEC 62443-4-2"))
		{
			ComplianceStatus cr37 = GetComplianceStatus("IEC 62443-4-2 CR 3.7");
			
			report += "<h2>" + cr37.requirementName + "</h2>\n";
			report += "<p><strong>Status:</strong> " + cr37.status + "</p>\n";
			report += "<p>" + cr37.description + "</p>\n";
			
			if (!cr37.evidence.isEmpty())
			{
				report += "<h3>Evidence</h3>\n";
				report += "<p>" + cr37.evidence + "</p>\n";
			}
			
			if (!cr37.gaps.isEmpty())
			{
				report += "<h3>Gaps</h3><ul>\n";
				for (const QString& gap : cr37.gaps)
				{
					report += "<li>" + gap + "</li>\n";
				}
				report += "</ul>\n";
			}
			
			if (!cr37.recommendations.isEmpty())
			{
				report += "<h3>Recommendations</h3><ul>\n";
				for (const QString& rec : cr37.recommendations)
				{
					report += "<li>" + rec + "</li>\n";
				}
				report += "</ul>\n";
			}
		}
		
		report += "</body></html>\n";
	}
	else // TEXT
	{
		report += "Security Compliance Report\n";
		report += "==========================\n\n";
		report += "Standard: " + standardId + "\n";
		report += "Generated: " + QDateTime::currentDateTime().toString(Qt::ISODate) + "\n\n";
		
		if (standardId.contains("IEC 62443-4-2"))
		{
			ComplianceStatus cr37 = GetComplianceStatus("IEC 62443-4-2 CR 3.7");
			
			report += cr37.requirementName + "\n";
			report += QString("-").repeated(cr37.requirementName.length()) + "\n\n";
			report += "Status: " + cr37.status + "\n\n";
			report += cr37.description + "\n\n";
			
			if (!cr37.evidence.isEmpty())
			{
				report += "Evidence:\n";
				report += cr37.evidence + "\n\n";
			}
			
			if (!cr37.gaps.isEmpty())
			{
				report += "Gaps:\n";
				for (const QString& gap : cr37.gaps)
				{
					report += "  - " + gap + "\n";
				}
				report += "\n";
			}
			
			if (!cr37.recommendations.isEmpty())
			{
				report += "Recommendations:\n";
				for (const QString& rec : cr37.recommendations)
				{
					report += "  - " + rec + "\n";
				}
				report += "\n";
			}
		}
	}
	
	return report;
}


QString CSecurityComplianceDocumentationProviderComp::GetDisabledFunctionalityDocumentation(const QString& format) const
{
	if (!m_pFunctionalityManager)
		return "Error: Functionality manager not configured";
	
	QByteArrayList disabled = m_pFunctionalityManager->GetDisabledFunctionality();
	
	QString doc;
	
	if (format == "MARKDOWN")
	{
		doc += "# Disabled Functionality\n\n";
		doc += "The following functionality has been disabled according to the principle of least functionality:\n\n";
		doc += FormatFunctionalityList(disabled, format);
	}
	else if (format == "HTML")
	{
		doc += "<html><body>\n";
		doc += "<h1>Disabled Functionality</h1>\n";
		doc += "<p>The following functionality has been disabled according to the principle of least functionality:</p>\n";
		doc += FormatFunctionalityList(disabled, format);
		doc += "</body></html>\n";
	}
	else // TEXT
	{
		doc += "Disabled Functionality\n";
		doc += "======================\n\n";
		doc += "The following functionality has been disabled according to the principle of least functionality:\n\n";
		doc += FormatFunctionalityList(disabled, format);
	}
	
	return doc;
}


QString CSecurityComplianceDocumentationProviderComp::GetSecurityControlsDocumentation(const QString& format) const
{
	if (!m_pFunctionalityManager)
		return "Error: Functionality manager not configured";
	
	QByteArrayList enabled = m_pFunctionalityManager->GetEnabledFunctionality();
	
	QString doc;
	
	if (format == "MARKDOWN")
	{
		doc += "# Active Security Controls\n\n";
		doc += "The following security-related functionality is currently enabled:\n\n";
		doc += FormatFunctionalityList(enabled, format);
	}
	else if (format == "HTML")
	{
		doc += "<html><body>\n";
		doc += "<h1>Active Security Controls</h1>\n";
		doc += "<p>The following security-related functionality is currently enabled:</p>\n";
		doc += FormatFunctionalityList(enabled, format);
		doc += "</body></html>\n";
	}
	else // TEXT
	{
		doc += "Active Security Controls\n";
		doc += "========================\n\n";
		doc += "The following security-related functionality is currently enabled:\n\n";
		doc += FormatFunctionalityList(enabled, format);
	}
	
	return doc;
}


QString CSecurityComplianceDocumentationProviderComp::GetConfigurationGuidance(const QString& format) const
{
	QString doc;
	
	if (format == "MARKDOWN")
	{
		doc += "# Security Configuration Guidance\n\n";
		doc += "## IEC 62443-4-2 CR 3.7: Least Functionality\n\n";
		doc += "### Overview\n\n";
		doc += "The principle of least functionality requires that the system provides only ";
		doc += "essential capabilities and prohibits or restricts the use of unnecessary functions, ";
		doc += "ports, protocols, and services.\n\n";
		doc += "### Configuration Steps\n\n";
		doc += "1. **Review Enabled Functionality**: Use `GetEnabledFunctionality()` to list all enabled features\n";
		doc += "2. **Identify Unnecessary Features**: Determine which features are not required for your use case\n";
		doc += "3. **Check Dependencies**: Use `GetDependentFunctionality()` to understand impact of disabling features\n";
		doc += "4. **Disable Optional Features**: Use `SetFunctionalityEnabled()` to disable unnecessary functionality\n";
		doc += "5. **Validate Configuration**: Use `ValidateConfiguration()` to ensure consistency\n\n";
		doc += "### Risk-Based Approach\n\n";
		doc += "Functionality is assigned risk levels (0=low, 3=high). Consider disabling:\n";
		doc += "- Level 3: High-risk functionality (remote access, write operations)\n";
		doc += "- Level 2: Medium-high risk functionality when not needed\n";
		doc += "- Level 1: Medium-low risk functionality in high-security environments\n\n";
		doc += "### Best Practices\n\n";
		doc += "- Start with minimum required functionality\n";
		doc += "- Enable additional features only when needed\n";
		doc += "- Regularly review and audit enabled functionality\n";
		doc += "- Document reasons for enabling high-risk functionality\n";
		doc += "- Use `ResetToDefaults()` to return to known-good state if needed\n\n";
	}
	else if (format == "HTML")
	{
		doc += "<html><body>\n";
		doc += "<h1>Security Configuration Guidance</h1>\n";
		doc += "<h2>IEC 62443-4-2 CR 3.7: Least Functionality</h2>\n";
		doc += "<h3>Overview</h3>\n";
		doc += "<p>The principle of least functionality requires that the system provides only ";
		doc += "essential capabilities and prohibits or restricts the use of unnecessary functions, ";
		doc += "ports, protocols, and services.</p>\n";
		doc += "<h3>Configuration Steps</h3>\n";
		doc += "<ol>\n";
		doc += "<li><strong>Review Enabled Functionality</strong>: Use GetEnabledFunctionality() to list all enabled features</li>\n";
		doc += "<li><strong>Identify Unnecessary Features</strong>: Determine which features are not required for your use case</li>\n";
		doc += "<li><strong>Check Dependencies</strong>: Use GetDependentFunctionality() to understand impact of disabling features</li>\n";
		doc += "<li><strong>Disable Optional Features</strong>: Use SetFunctionalityEnabled() to disable unnecessary functionality</li>\n";
		doc += "<li><strong>Validate Configuration</strong>: Use ValidateConfiguration() to ensure consistency</li>\n";
		doc += "</ol>\n";
		doc += "<h3>Risk-Based Approach</h3>\n";
		doc += "<p>Functionality is assigned risk levels (0=low, 3=high). Consider disabling:</p>\n";
		doc += "<ul>\n";
		doc += "<li>Level 3: High-risk functionality (remote access, write operations)</li>\n";
		doc += "<li>Level 2: Medium-high risk functionality when not needed</li>\n";
		doc += "<li>Level 1: Medium-low risk functionality in high-security environments</li>\n";
		doc += "</ul>\n";
		doc += "</body></html>\n";
	}
	else // TEXT
	{
		doc += "Security Configuration Guidance\n";
		doc += "===============================\n\n";
		doc += "IEC 62443-4-2 CR 3.7: Least Functionality\n\n";
		doc += "Overview\n";
		doc += "--------\n";
		doc += "The principle of least functionality requires that the system provides only\n";
		doc += "essential capabilities and prohibits or restricts the use of unnecessary functions,\n";
		doc += "ports, protocols, and services.\n\n";
		doc += "Configuration Steps\n";
		doc += "-------------------\n";
		doc += "1. Review Enabled Functionality: Use GetEnabledFunctionality() to list all enabled features\n";
		doc += "2. Identify Unnecessary Features: Determine which features are not required for your use case\n";
		doc += "3. Check Dependencies: Use GetDependentFunctionality() to understand impact of disabling features\n";
		doc += "4. Disable Optional Features: Use SetFunctionalityEnabled() to disable unnecessary functionality\n";
		doc += "5. Validate Configuration: Use ValidateConfiguration() to ensure consistency\n\n";
	}
	
	return doc;
}


ComplianceStatus CSecurityComplianceDocumentationProviderComp::GenerateCR37ComplianceStatus() const
{
	ComplianceStatus status;
	status.requirementId = "IEC 62443-4-2 CR 3.7";
	status.requirementName = "Least Functionality";
	status.description = "The control system shall provide the capability to specifically prohibit or "
	                     "restrict the use of unnecessary functions, ports, protocols, and/or services.";
	
	if (!m_pFunctionalityManager)
	{
		status.status = "NON_COMPLIANT";
		status.gaps << "Functionality manager not configured";
		status.recommendations << "Configure and initialize functionality manager component";
		return status;
	}
	
	// Check implementation
	QByteArrayList allFunc = m_pFunctionalityManager->GetAllFunctionality();
	QByteArrayList enabled = m_pFunctionalityManager->GetEnabledFunctionality();
	QByteArrayList disabled = m_pFunctionalityManager->GetDisabledFunctionality();
	QByteArrayList issues = m_pFunctionalityManager->ValidateConfiguration();
	
	// Build evidence
	status.evidence = QString("System manages %1 controllable functionality items:\n").arg(allFunc.size());
	status.evidence += QString("- %1 enabled\n").arg(enabled.size());
	status.evidence += QString("- %1 disabled\n").arg(disabled.size());
	
	if (!issues.isEmpty())
	{
		status.evidence += QString("- %1 configuration issues detected\n").arg(issues.size());
	}
	
	// Determine compliance status
	if (allFunc.isEmpty())
	{
		status.status = "PARTIAL";
		status.gaps << "No functionality registered in manager";
		status.recommendations << "Register controllable functionality (features, protocols, services, API endpoints)";
	}
	else if (!issues.isEmpty())
	{
		status.status = "PARTIAL";
		status.gaps << "Configuration validation issues exist";
		for (const QByteArray& issue : issues)
		{
			status.gaps << QString(issue);
		}
		status.recommendations << "Resolve configuration dependencies";
		status.recommendations << "Use ValidateConfiguration() to identify issues";
	}
	else if (disabled.isEmpty())
	{
		status.status = "PARTIAL";
		status.gaps << "No functionality is disabled (least functionality not demonstrated)";
		status.recommendations << "Review enabled functionality and disable unnecessary items";
		status.recommendations << "Document reasons for all enabled high-risk functionality";
	}
	else
	{
		status.status = "COMPLIANT";
		status.evidence += "\nThe system implements least functionality controls by:\n";
		status.evidence += "- Maintaining registry of controllable functionality\n";
		status.evidence += "- Providing API to enable/disable functionality\n";
		status.evidence += "- Enforcing dependency requirements\n";
		status.evidence += "- Validating configuration consistency\n";
		status.evidence += "- Preventing disabling of required functionality\n";
		
		status.recommendations << "Regularly review enabled functionality";
		status.recommendations << "Document security justification for enabled high-risk items";
		status.recommendations << "Use compliance documentation tools for auditing";
	}
	
	return status;
}


QString CSecurityComplianceDocumentationProviderComp::FormatFunctionalityList(const QByteArrayList& functionalityIds, const QString& format) const
{
	if (!m_pFunctionalityManager)
		return "Error: Functionality manager not configured";
	
	QString result;
	
	if (format == "MARKDOWN")
	{
		result += "| ID | Name | Type | Risk | Description |\n";
		result += "|----|------|------|------|-------------|\n";
		
		for (const QByteArray& id : functionalityIds)
		{
			IFunctionalityInfo* pInfo = m_pFunctionalityManager->GetFunctionalityInfo(id);
			if (pInfo)
			{
				QString type;
				switch (pInfo->GetType())
				{
					case FunctionalityType::FEATURE: type = "Feature"; break;
					case FunctionalityType::PROTOCOL: type = "Protocol"; break;
					case FunctionalityType::SERVICE: type = "Service"; break;
					case FunctionalityType::API_ENDPOINT: type = "API Endpoint"; break;
					case FunctionalityType::SECURITY_PROTOCOL: type = "Security Protocol"; break;
				}
				
				result += "| " + QString(id) + " | ";
				result += pInfo->GetName() + " | ";
				result += type + " | ";
				result += QString::number(pInfo->GetRiskLevel()) + " | ";
				result += pInfo->GetDescription() + " |\n";
			}
		}
	}
	else if (format == "HTML")
	{
		result += "<table border='1'>\n";
		result += "<tr><th>ID</th><th>Name</th><th>Type</th><th>Risk</th><th>Description</th></tr>\n";
		
		for (const QByteArray& id : functionalityIds)
		{
			IFunctionalityInfo* pInfo = m_pFunctionalityManager->GetFunctionalityInfo(id);
			if (pInfo)
			{
				QString type;
				switch (pInfo->GetType())
				{
					case FunctionalityType::FEATURE: type = "Feature"; break;
					case FunctionalityType::PROTOCOL: type = "Protocol"; break;
					case FunctionalityType::SERVICE: type = "Service"; break;
					case FunctionalityType::API_ENDPOINT: type = "API Endpoint"; break;
					case FunctionalityType::SECURITY_PROTOCOL: type = "Security Protocol"; break;
				}
				
				result += "<tr>";
				result += "<td>" + QString(id) + "</td>";
				result += "<td>" + pInfo->GetName() + "</td>";
				result += "<td>" + type + "</td>";
				result += "<td>" + QString::number(pInfo->GetRiskLevel()) + "</td>";
				result += "<td>" + pInfo->GetDescription() + "</td>";
				result += "</tr>\n";
			}
		}
		
		result += "</table>\n";
	}
	else // TEXT
	{
		for (const QByteArray& id : functionalityIds)
		{
			IFunctionalityInfo* pInfo = m_pFunctionalityManager->GetFunctionalityInfo(id);
			if (pInfo)
			{
				QString type;
				switch (pInfo->GetType())
				{
					case FunctionalityType::FEATURE: type = "Feature"; break;
					case FunctionalityType::PROTOCOL: type = "Protocol"; break;
					case FunctionalityType::SERVICE: type = "Service"; break;
					case FunctionalityType::API_ENDPOINT: type = "API Endpoint"; break;
					case FunctionalityType::SECURITY_PROTOCOL: type = "Security Protocol"; break;
				}
				
				result += "- " + QString(id) + " (" + pInfo->GetName() + ")\n";
				result += "  Type: " + type + "\n";
				result += "  Risk Level: " + QString::number(pInfo->GetRiskLevel()) + "\n";
				result += "  Description: " + pInfo->GetDescription() + "\n\n";
			}
		}
	}
	
	return result;
}


} // namespace imtfunc

