#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <iser/IObject.h>


namespace imtfunc
{


/**
	@brief Type of functionality that can be managed
	
	Defines the different categories of functionality that can be enabled or disabled
	according to IEC 62443-4-2 CR 3.7 (Least Functionality) requirements.
*/
enum class FunctionalityType
{
	/**
		Application feature (e.g., reporting, data export, advanced analytics)
	*/
	FEATURE,
	
	/**
		Network protocol (e.g., TCP, UDP, WebSocket, HTTP, HTTPS)
	*/
	PROTOCOL,
	
	/**
		System service (e.g., remote access, file transfer, logging service)
	*/
	SERVICE,
	
	/**
		API endpoint or interface (e.g., REST API path, GraphQL query/mutation)
	*/
	API_ENDPOINT,
	
	/**
		TLS/SSL version or cipher suite
	*/
	SECURITY_PROTOCOL
};


/**
	@brief Information about a controllable functionality
	
	IFunctionalityInfo describes a single piece of functionality that can be
	enabled or disabled according to the principle of least functionality.
	
	This interface supports IEC 62443-4-2 CR 3.7 by providing:
	- Unique identification of functionality
	- Human-readable description
	- Type categorization
	- Dependency tracking
	- Risk level indication
	
	@par Usage Pattern:
	@code{.cpp}
	// Get functionality info
	IFunctionalityInfo* pInfo = // get from functionality manager
	
	// Check functionality properties
	QString name = pInfo->GetName();
	QString desc = pInfo->GetDescription();
	FunctionalityType type = pInfo->GetType();
	
	// Check dependencies
	QByteArrayList deps = pInfo->GetDependencies();
	for (const QByteArray& depId : deps)
	{
		// Check if dependencies are enabled
	}
	@endcode
	
	@see IFunctionalityManager
	@ingroup imtfunc
*/
class IFunctionalityInfo: virtual public iser::IObject
{
public:
	/**
		@brief Get unique identifier for this functionality
		
		The ID uniquely identifies this functionality within the system.
		Format: "category.subcategory.name" (e.g., "protocol.network.tcp")
		
		@return Functionality unique identifier
	*/
	virtual QByteArray GetId() const = 0;
	
	/**
		@brief Get human-readable name
		
		Short descriptive name for display purposes.
		
		@return Functionality name
	*/
	virtual QString GetName() const = 0;
	
	/**
		@brief Get detailed description
		
		Full description explaining what this functionality does and
		why it might need to be disabled.
		
		@return Functionality description
	*/
	virtual QString GetDescription() const = 0;
	
	/**
		@brief Get functionality type
		
		Categorizes this functionality (feature, protocol, service, etc.)
		
		@return Functionality type
	*/
	virtual FunctionalityType GetType() const = 0;
	
	/**
		@brief Check if functionality is enabled by default
		
		Indicates whether this functionality should be enabled in a
		default installation.
		
		@return true if enabled by default, false otherwise
	*/
	virtual bool IsEnabledByDefault() const = 0;
	
	/**
		@brief Check if functionality is required
		
		Required functionality cannot be disabled as it's essential for
		system operation. Non-required functionality follows the principle
		of least functionality and can be disabled when not needed.
		
		@return true if functionality is required, false if optional
	*/
	virtual bool IsRequired() const = 0;
	
	/**
		@brief Get list of functionality IDs this depends on
		
		Returns IDs of other functionality that must be enabled
		for this functionality to work properly.
		
		@return List of dependency IDs
	*/
	virtual QByteArrayList GetDependencies() const = 0;
	
	/**
		@brief Get security risk level (0=low, 3=high)
		
		Indicates the relative security risk if this functionality is enabled.
		Higher risk functionality should be disabled when not needed.
		
		0 = Low risk (e.g., read-only features)
		1 = Medium-low risk
		2 = Medium-high risk  
		3 = High risk (e.g., remote access, write operations)
		
		@return Risk level (0-3)
	*/
	virtual int GetRiskLevel() const = 0;
};


typedef istd::TUniqueInterfacePtr<IFunctionalityInfo> IFunctionalityInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IFunctionalityInfo> IFunctionalityInfoSharedPtr;


} // namespace imtfunc


