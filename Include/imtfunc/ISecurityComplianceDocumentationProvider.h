#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QMap>

// ACF includes
#include <iser/IObject.h>


namespace imtfunc
{


/**
	@brief Information about security compliance status
	
	Provides compliance status for a specific security requirement.
*/
struct ComplianceStatus
{
	/** Requirement identifier (e.g., "IEC 62443-4-2 CR 3.7") */
	QString requirementId;
	
	/** Short name of the requirement */
	QString requirementName;
	
	/** Detailed description of the requirement */
	QString description;
	
	/** Compliance status: "COMPLIANT", "PARTIAL", "NON_COMPLIANT" */
	QString status;
	
	/** Evidence supporting compliance status */
	QString evidence;
	
	/** List of gaps or issues if not fully compliant */
	QStringList gaps;
	
	/** Recommendations for achieving/maintaining compliance */
	QStringList recommendations;
};


/**
	@brief Provider for security compliance documentation
	
	ISecurityComplianceDocumentationProvider generates documentation that
	demonstrates compliance with security standards such as IEC 62443-4-2.
	
	This interface supports auditing and certification by:
	- Documenting enabled/disabled functionality
	- Listing active security controls
	- Providing evidence of least functionality implementation
	- Generating compliance reports
	
	@par Usage Pattern:
	@code{.cpp}
	// Get documentation provider (via component system)
	ISecurityComplianceDocumentationProvider* pProvider = // get from component system
	
	// Get compliance status for specific requirement
	ComplianceStatus status = pProvider->GetComplianceStatus("IEC 62443-4-2 CR 3.7");
	
	// Generate full compliance report
	QString report = pProvider->GenerateComplianceReport("IEC 62443-4-2");
	
	// Get list of all disabled functionality (for documentation)
	QString disabledDoc = pProvider->GetDisabledFunctionalityDocumentation();
	@endcode
	
	@see IFunctionalityManager
	@ingroup imtfunc
*/
class ISecurityComplianceDocumentationProvider: virtual public iser::IObject
{
public:
	/**
		@brief Get compliance status for a specific requirement
		
		Returns detailed compliance information for the specified
		security requirement.
		
		@param requirementId Identifier of security requirement (e.g., "IEC 62443-4-2 CR 3.7")
		@return Compliance status information
	*/
	virtual ComplianceStatus GetComplianceStatus(const QString& requirementId) const = 0;
	
	/**
		@brief Get list of supported compliance requirements
		
		Returns list of requirement identifiers that this provider can
		generate documentation for.
		
		@return List of supported requirement IDs
	*/
	virtual QStringList GetSupportedRequirements() const = 0;
	
	/**
		@brief Generate compliance report for a standard
		
		Creates a formatted compliance report document covering all
		requirements in the specified standard.
		
		@param standardId Standard identifier (e.g., "IEC 62443-4-2")
		@param format Output format ("HTML", "MARKDOWN", "TEXT")
		@return Formatted compliance report
	*/
	virtual QString GenerateComplianceReport(const QString& standardId, const QString& format = "MARKDOWN") const = 0;
	
	/**
		@brief Get documentation of disabled functionality
		
		Returns formatted documentation listing all functionality that
		has been disabled, supporting the principle of least functionality.
		
		@param format Output format ("HTML", "MARKDOWN", "TEXT")
		@return Documentation of disabled functionality
	*/
	virtual QString GetDisabledFunctionalityDocumentation(const QString& format = "MARKDOWN") const = 0;
	
	/**
		@brief Get documentation of enabled security controls
		
		Returns formatted documentation of active security measures.
		
		@param format Output format ("HTML", "MARKDOWN", "TEXT")
		@return Documentation of security controls
	*/
	virtual QString GetSecurityControlsDocumentation(const QString& format = "MARKDOWN") const = 0;
	
	/**
		@brief Get configuration guidance documentation
		
		Returns documentation with recommendations for secure configuration
		according to security standards.
		
		@param format Output format ("HTML", "MARKDOWN", "TEXT")
		@return Configuration guidance documentation
	*/
	virtual QString GetConfigurationGuidance(const QString& format = "MARKDOWN") const = 0;
};


typedef istd::TUniqueInterfacePtr<ISecurityComplianceDocumentationProvider> ISecurityComplianceDocumentationProviderUniquePtr;
typedef istd::TSharedInterfacePtr<ISecurityComplianceDocumentationProvider> ISecurityComplianceDocumentationProviderSharedPtr;


} // namespace imtfunc


