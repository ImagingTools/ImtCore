#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QMap>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtfunc/IFunctionalityInfo.h>


namespace imtfunc
{


/**
	@brief Manager for controlling system functionality according to IEC 62443-4-2 CR 3.7
	
	IFunctionalityManager implements the "Least Functionality" security requirement by
	providing centralized control over which features, protocols, services, and API
	endpoints are enabled in the system.
	
	This interface supports IEC 62443-4-2 CR 3.7 by:
	- Allowing administrators to disable unnecessary functionality
	- Restricting enabled ports, protocols, and services
	- Maintaining configuration of enabled/disabled functionality
	- Tracking dependencies between functionality
	- Providing audit trail of functionality changes
	
	The manager maintains a registry of all controllable functionality and their
	current enabled/disabled state. Changes to functionality state trigger
	notifications to observers.
	
	@par Key Concepts:
	- **Functionality ID**: Unique identifier for each controllable functionality
	- **Enabled State**: Whether functionality is currently active
	- **Dependencies**: Some functionality requires other functionality to be enabled
	- **Required vs Optional**: Required functionality cannot be disabled
	
	@par Usage Pattern:
	@code{.cpp}
	// Get functionality manager (via component system)
	IFunctionalityManager* pMgr = // get from component system
	
	// Register available functionality
	IFunctionalityInfo* pTcpInfo = // create TCP protocol info
	pMgr->RegisterFunctionality(pTcpInfo);
	
	// Check if functionality is enabled
	if (pMgr->IsFunctionalityEnabled("protocol.network.tcp"))
	{
		// Start TCP server
	}
	
	// Disable optional functionality
	if (pMgr->CanDisableFunctionality("protocol.network.udp"))
	{
		pMgr->SetFunctionalityEnabled("protocol.network.udp", false);
	}
	
	// Get list of all enabled functionality
	QByteArrayList enabled = pMgr->GetEnabledFunctionality();
	
	// Register for change notifications
	class FuncObserver : public imod::CSingleModelObserverBase
	{
	protected:
		void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override
		{
			// Functionality state changed
		}
	};
	@endcode
	
	@see IFunctionalityInfo
	@see ISecurityComplianceDocumentationProvider
	@ingroup imtfunc
*/
class IFunctionalityManager: virtual public istd::IChangeable
{
public:
	/**
		@brief Register a functionality that can be managed
		
		Adds a new functionality to the manager's registry. The functionality
		will be tracked and its enabled/disabled state can be controlled.
		
		@param pFunctionalityInfo Information about the functionality to register
		@return true if registration succeeded, false if ID already exists
		@see UnregisterFunctionality()
		@see GetFunctionalityInfo()
	*/
	virtual bool RegisterFunctionality(IFunctionalityInfo* pFunctionalityInfo) = 0;
	
	/**
		@brief Unregister a functionality
		
		Removes a functionality from the manager's registry.
		
		@param functionalityId ID of functionality to unregister
		@return true if unregistration succeeded, false if ID not found
		@see RegisterFunctionality()
	*/
	virtual bool UnregisterFunctionality(const QByteArray& functionalityId) = 0;
	
	/**
		@brief Get information about a registered functionality
		
		Retrieves the IFunctionalityInfo for a given functionality ID.
		
		@param functionalityId ID of functionality
		@return Pointer to functionality info, or nullptr if not found
		@see RegisterFunctionality()
	*/
	virtual IFunctionalityInfo* GetFunctionalityInfo(const QByteArray& functionalityId) const = 0;
	
	/**
		@brief Get list of all registered functionality IDs
		
		@param type Optional filter by functionality type (pass -1 for all types)
		@return List of functionality IDs
	*/
	virtual QByteArrayList GetAllFunctionality(int type = -1) const = 0;
	
	/**
		@brief Check if functionality is currently enabled
		
		@param functionalityId ID of functionality to check
		@return true if enabled, false if disabled or not found
		@see SetFunctionalityEnabled()
	*/
	virtual bool IsFunctionalityEnabled(const QByteArray& functionalityId) const = 0;
	
	/**
		@brief Enable or disable a functionality
		
		Changes the enabled state of a functionality. This will fail if:
		- The functionality is required and cannot be disabled
		- Disabling would break dependencies of other enabled functionality
		- Enabling requires dependencies that are disabled
		
		Change notifications are sent to observers when state changes.
		
		@param functionalityId ID of functionality to modify
		@param enabled true to enable, false to disable
		@param reason Optional reason for change (for audit log)
		@return true if state change succeeded, false otherwise
		@see IsFunctionalityEnabled()
		@see CanDisableFunctionality()
	*/
	virtual bool SetFunctionalityEnabled(const QByteArray& functionalityId, bool enabled, const QString& reason = QString()) = 0;
	
	/**
		@brief Check if functionality can be disabled
		
		Determines whether it's possible to disable a functionality.
		Returns false if functionality is required or other enabled
		functionality depends on it.
		
		@param functionalityId ID of functionality to check
		@return true if can be disabled, false otherwise
	*/
	virtual bool CanDisableFunctionality(const QByteArray& functionalityId) const = 0;
	
	/**
		@brief Get list of enabled functionality IDs
		
		@param type Optional filter by functionality type (pass -1 for all types)
		@return List of enabled functionality IDs
		@see GetDisabledFunctionality()
	*/
	virtual QByteArrayList GetEnabledFunctionality(int type = -1) const = 0;
	
	/**
		@brief Get list of disabled functionality IDs
		
		@param type Optional filter by functionality type (pass -1 for all types)
		@return List of disabled functionality IDs
		@see GetEnabledFunctionality()
	*/
	virtual QByteArrayList GetDisabledFunctionality(int type = -1) const = 0;
	
	/**
		@brief Get functionality IDs that depend on the specified functionality
		
		Returns list of functionality that requires the specified functionality
		to be enabled. Useful for understanding impact of disabling functionality.
		
		@param functionalityId ID of functionality
		@return List of dependent functionality IDs
	*/
	virtual QByteArrayList GetDependentFunctionality(const QByteArray& functionalityId) const = 0;
	
	/**
		@brief Reset all functionality to default enabled/disabled state
		
		Restores all functionality to their IsEnabledByDefault() values.
		This is useful for returning to a known-good configuration.
		
		@return true if reset succeeded
	*/
	virtual bool ResetToDefaults() = 0;
	
	/**
		@brief Validate current configuration
		
		Checks that all enabled functionality has its required dependencies enabled.
		Returns list of functionality IDs with unmet dependencies.
		
		@return List of functionality IDs with dependency problems (empty if valid)
	*/
	virtual QByteArrayList ValidateConfiguration() const = 0;
};


typedef istd::TUniqueInterfacePtr<IFunctionalityManager> IFunctionalityManagerUniquePtr;
typedef istd::TSharedInterfacePtr<IFunctionalityManager> IFunctionalityManagerSharedPtr;


} // namespace imtfunc


