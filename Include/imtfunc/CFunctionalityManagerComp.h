#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QSet>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CChangeBroadcasterComp.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtfunc/IFunctionalityManager.h>
#include <imtfunc/CFunctionalityInfo.h>


namespace imtfunc
{


/**
	@brief Functionality manager component implementing IEC 62443-4-2 CR 3.7
	
	CFunctionalityManagerComp implements IFunctionalityManager to provide centralized
	control over system functionality according to the "least functionality" principle.
	
	This component supports IEC 62443-4-2 CR 3.7 by:
	- Maintaining registry of controllable functionality
	- Tracking enabled/disabled state of each functionality
	- Enforcing dependency requirements
	- Preventing disabling of required functionality
	- Notifying observers of functionality state changes
	- Validating configuration consistency
	
	@par Key Features:
	- **Functionality Registry**: QMap-based storage of functionality information
	- **State Management**: Tracks enabled/disabled state with dependency validation
	- **Change Notifications**: Notifies observers when functionality state changes
	- **Dependency Tracking**: Maintains reverse dependency map for efficient queries
	- **Validation**: Ensures configuration remains consistent
	- **Audit Support**: Provides reason tracking for state changes
	
	@par Usage Pattern:
	@code{.cpp}
	// Create and configure component
	CFunctionalityManagerComp* pMgr = // create via component system
	
	// Register functionality
	CFunctionalityInfo* pTcpInfo = new CFunctionalityInfo(
		"protocol.network.tcp",
		"TCP Protocol",
		"Transmission Control Protocol for reliable network communication",
		FunctionalityType::PROTOCOL,
		true,  // enabled by default
		true,  // required
		2);    // medium-high risk
	pMgr->RegisterFunctionality(pTcpInfo);
	
	// Register optional functionality with dependencies
	CFunctionalityInfo* pWebSocketInfo = new CFunctionalityInfo(
		"protocol.network.websocket",
		"WebSocket Protocol",
		"WebSocket protocol for real-time bidirectional communication",
		FunctionalityType::PROTOCOL,
		false, // disabled by default
		false, // optional
		2);    // medium-high risk
	pWebSocketInfo->AddDependency("protocol.network.tcp");
	pMgr->RegisterFunctionality(pWebSocketInfo);
	
	// Check and modify functionality state
	if (pMgr->IsFunctionalityEnabled("protocol.network.websocket"))
	{
		// WebSocket is enabled
	}
	
	if (pMgr->CanDisableFunctionality("protocol.network.udp"))
	{
		pMgr->SetFunctionalityEnabled("protocol.network.udp", false, 
			"Disabled per security policy");
	}
	@endcode
	
	@see IFunctionalityManager
	@see CFunctionalityInfo
	@ingroup imtfunc
*/
class CFunctionalityManagerComp:
			public ilog::CLoggerComponentBase,
			public imod::CChangeBroadcasterComp,
			virtual public IFunctionalityManager
{
public:
	CFunctionalityManagerComp();
	virtual ~CFunctionalityManagerComp();
	
	// IFunctionalityManager interface
	bool RegisterFunctionality(IFunctionalityInfo* pFunctionalityInfo) override;
	bool UnregisterFunctionality(const QByteArray& functionalityId) override;
	IFunctionalityInfo* GetFunctionalityInfo(const QByteArray& functionalityId) const override;
	QByteArrayList GetAllFunctionality(int type = -1) const override;
	bool IsFunctionalityEnabled(const QByteArray& functionalityId) const override;
	bool SetFunctionalityEnabled(const QByteArray& functionalityId, bool enabled, const QString& reason = QString()) override;
	bool CanDisableFunctionality(const QByteArray& functionalityId) const override;
	QByteArrayList GetEnabledFunctionality(int type = -1) const override;
	QByteArrayList GetDisabledFunctionality(int type = -1) const override;
	QByteArrayList GetDependentFunctionality(const QByteArray& functionalityId) const override;
	bool ResetToDefaults() override;
	QByteArrayList ValidateConfiguration() const override;

protected:
	/**
		@brief Update reverse dependency map
		
		Rebuilds the map of functionality to dependent functionality.
		Called when functionality is registered/unregistered.
	*/
	void UpdateDependencyMap();
	
	/**
		@brief Check if dependencies are satisfied
		
		Verifies all dependencies of a functionality are enabled.
		
		@param functionalityId ID of functionality to check
		@return true if all dependencies are enabled
	*/
	bool AreDependenciesSatisfied(const QByteArray& functionalityId) const;
	
	/**
		@brief Check if any enabled functionality depends on this
		
		@param functionalityId ID to check
		@return true if any enabled functionality depends on this
	*/
	bool HasEnabledDependents(const QByteArray& functionalityId) const;

private:
	/** Map of functionality ID to info */
	QMap<QByteArray, IFunctionalityInfo*> m_functionalityRegistry;
	
	/** Map of functionality ID to enabled state */
	QMap<QByteArray, bool> m_enabledState;
	
	/** Map of functionality ID to list of IDs that depend on it */
	QMap<QByteArray, QSet<QByteArray>> m_reverseDependencies;
};


} // namespace imtfunc


