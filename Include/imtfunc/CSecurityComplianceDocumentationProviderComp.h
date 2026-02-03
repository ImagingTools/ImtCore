#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfunc/ISecurityComplianceDocumentationProvider.h>
#include <imtfunc/IFunctionalityManager.h>


namespace imtfunc
{


/**
	@brief Implementation of ISecurityComplianceDocumentationProvider
	
	Generates compliance documentation for security standards such as IEC 62443-4-2.
	
	@par Component Configuration:
	- **FunctionalityManager**: Reference to IFunctionalityManager for querying
	  functionality state and generating compliance evidence
	
	@see ISecurityComplianceDocumentationProvider
	@ingroup imtfunc
*/
class CSecurityComplianceDocumentationProviderComp:
			public ilog::CLoggerComponentBase,
			virtual public ISecurityComplianceDocumentationProvider
{
public:
	CSecurityComplianceDocumentationProviderComp();
	virtual ~CSecurityComplianceDocumentationProviderComp();
	
	// ISecurityComplianceDocumentationProvider interface
	ComplianceStatus GetComplianceStatus(const QString& requirementId) const override;
	QStringList GetSupportedRequirements() const override;
	QString GenerateComplianceReport(const QString& standardId, const QString& format = "MARKDOWN") const override;
	QString GetDisabledFunctionalityDocumentation(const QString& format = "MARKDOWN") const override;
	QString GetSecurityControlsDocumentation(const QString& format = "MARKDOWN") const override;
	QString GetConfigurationGuidance(const QString& format = "MARKDOWN") const override;
	
	/**
		@brief Set functionality manager reference
		
		@param pManager Pointer to functionality manager
	*/
	void SetFunctionalityManager(IFunctionalityManager* pManager);

protected:
	/**
		@brief Generate IEC 62443-4-2 CR 3.7 compliance status
		
		@return Compliance status for CR 3.7
	*/
	ComplianceStatus GenerateCR37ComplianceStatus() const;
	
	/**
		@brief Format functionality list
		
		@param functionalityIds List of functionality IDs
		@param format Output format
		@return Formatted functionality list
	*/
	QString FormatFunctionalityList(const QByteArrayList& functionalityIds, const QString& format) const;

private:
	IFunctionalityManager* m_pFunctionalityManager;
};


} // namespace imtfunc


