// ImtCore includes
#include <imtfunc/CFunctionalityManagerComp.h>


namespace imtfunc
{


CFunctionalityManagerComp::CFunctionalityManagerComp()
	: ilog::CLoggerComponentBase()
	, imod::CChangeBroadcasterComp()
{
}


CFunctionalityManagerComp::~CFunctionalityManagerComp()
{
	// Clean up registered functionality
	for (auto it = m_functionalityRegistry.begin(); it != m_functionalityRegistry.end(); ++it)
	{
		delete it.value();
	}
	m_functionalityRegistry.clear();
}


bool CFunctionalityManagerComp::RegisterFunctionality(IFunctionalityInfo* pFunctionalityInfo)
{
	if (!pFunctionalityInfo)
		return false;
	
	QByteArray id = pFunctionalityInfo->GetId();
	
	// Check if already registered
	if (m_functionalityRegistry.contains(id))
	{
		LogWarning("Functionality already registered: " + QString(id));
		return false;
	}
	
	// Register the functionality
	m_functionalityRegistry.insert(id, pFunctionalityInfo);
	
	// Set initial enabled state based on default or required status
	bool initiallyEnabled = pFunctionalityInfo->IsRequired() || pFunctionalityInfo->IsEnabledByDefault();
	m_enabledState.insert(id, initiallyEnabled);
	
	// Update dependency map
	UpdateDependencyMap();
	
	LogInfo("Registered functionality: " + pFunctionalityInfo->GetName() + " (" + QString(id) + ")");
	
	// Notify observers
	istd::CChangeNotifier notifier(this);
	
	return true;
}


bool CFunctionalityManagerComp::UnregisterFunctionality(const QByteArray& functionalityId)
{
	if (!m_functionalityRegistry.contains(functionalityId))
		return false;
	
	// Check if any enabled functionality depends on this
	if (HasEnabledDependents(functionalityId))
	{
		LogWarning("Cannot unregister functionality with enabled dependents: " + QString(functionalityId));
		return false;
	}
	
	// Remove from registry
	IFunctionalityInfo* pInfo = m_functionalityRegistry.take(functionalityId);
	delete pInfo;
	
	// Remove enabled state
	m_enabledState.remove(functionalityId);
	
	// Update dependency map
	UpdateDependencyMap();
	
	LogInfo("Unregistered functionality: " + QString(functionalityId));
	
	// Notify observers
	istd::CChangeNotifier notifier(this);
	
	return true;
}


IFunctionalityInfo* CFunctionalityManagerComp::GetFunctionalityInfo(const QByteArray& functionalityId) const
{
	return m_functionalityRegistry.value(functionalityId, nullptr);
}


QByteArrayList CFunctionalityManagerComp::GetAllFunctionality(int type) const
{
	QByteArrayList result;
	
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		if (type < 0 || static_cast<int>(it.value()->GetType()) == type)
		{
			result.append(it.key());
		}
	}
	
	return result;
}


bool CFunctionalityManagerComp::IsFunctionalityEnabled(const QByteArray& functionalityId) const
{
	return m_enabledState.value(functionalityId, false);
}


bool CFunctionalityManagerComp::SetFunctionalityEnabled(const QByteArray& functionalityId, bool enabled, const QString& reason)
{
	// Check if functionality exists
	IFunctionalityInfo* pInfo = GetFunctionalityInfo(functionalityId);
	if (!pInfo)
	{
		LogWarning("Cannot set state for unknown functionality: " + QString(functionalityId));
		return false;
	}
	
	// Check if already in requested state
	bool currentState = IsFunctionalityEnabled(functionalityId);
	if (currentState == enabled)
		return true; // Already in desired state
	
	if (enabled)
	{
		// Enabling functionality - check dependencies
		if (!AreDependenciesSatisfied(functionalityId))
		{
			QByteArrayList deps = pInfo->GetDependencies();
			QString depList;
			for (const QByteArray& dep : deps)
			{
				if (!IsFunctionalityEnabled(dep))
				{
					if (!depList.isEmpty())
						depList += ", ";
					depList += QString(dep);
				}
			}
			LogWarning("Cannot enable functionality - missing dependencies: " + depList);
			return false;
		}
		
		// Enable the functionality
		m_enabledState[functionalityId] = true;
		
		QString msg = "Enabled functionality: " + pInfo->GetName() + " (" + QString(functionalityId) + ")";
		if (!reason.isEmpty())
			msg += " - Reason: " + reason;
		LogInfo(msg);
	}
	else
	{
		// Disabling functionality
		if (pInfo->IsRequired())
		{
			LogWarning("Cannot disable required functionality: " + QString(functionalityId));
			return false;
		}
		
		// Check if any enabled functionality depends on this
		if (HasEnabledDependents(functionalityId))
		{
			QByteArrayList dependents = GetDependentFunctionality(functionalityId);
			QString depList;
			for (const QByteArray& dep : dependents)
			{
				if (IsFunctionalityEnabled(dep))
				{
					if (!depList.isEmpty())
						depList += ", ";
					depList += QString(dep);
				}
			}
			LogWarning("Cannot disable functionality - enabled dependents exist: " + depList);
			return false;
		}
		
		// Disable the functionality
		m_enabledState[functionalityId] = false;
		
		QString msg = "Disabled functionality: " + pInfo->GetName() + " (" + QString(functionalityId) + ")";
		if (!reason.isEmpty())
			msg += " - Reason: " + reason;
		LogInfo(msg);
	}
	
	// Notify observers
	istd::CChangeNotifier notifier(this);
	
	return true;
}


bool CFunctionalityManagerComp::CanDisableFunctionality(const QByteArray& functionalityId) const
{
	IFunctionalityInfo* pInfo = GetFunctionalityInfo(functionalityId);
	if (!pInfo)
		return false;
	
	// Cannot disable required functionality
	if (pInfo->IsRequired())
		return false;
	
	// Cannot disable if enabled functionality depends on it
	if (HasEnabledDependents(functionalityId))
		return false;
	
	return true;
}


QByteArrayList CFunctionalityManagerComp::GetEnabledFunctionality(int type) const
{
	QByteArrayList result;
	
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		const QByteArray& id = it.key();
		IFunctionalityInfo* pInfo = it.value();
		
		if (IsFunctionalityEnabled(id))
		{
			if (type < 0 || static_cast<int>(pInfo->GetType()) == type)
			{
				result.append(id);
			}
		}
	}
	
	return result;
}


QByteArrayList CFunctionalityManagerComp::GetDisabledFunctionality(int type) const
{
	QByteArrayList result;
	
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		const QByteArray& id = it.key();
		IFunctionalityInfo* pInfo = it.value();
		
		if (!IsFunctionalityEnabled(id))
		{
			if (type < 0 || static_cast<int>(pInfo->GetType()) == type)
			{
				result.append(id);
			}
		}
	}
	
	return result;
}


QByteArrayList CFunctionalityManagerComp::GetDependentFunctionality(const QByteArray& functionalityId) const
{
	QSet<QByteArray> dependents = m_reverseDependencies.value(functionalityId);
	// Convert QSet to QList for Qt 6 compatibility (QSet::values() is deprecated)
	return QByteArrayList(dependents.begin(), dependents.end());
}


bool CFunctionalityManagerComp::ResetToDefaults()
{
	LogInfo("Resetting all functionality to default state");
	
	// Reset all functionality to default state
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		const QByteArray& id = it.key();
		IFunctionalityInfo* pInfo = it.value();
		
		bool defaultState = pInfo->IsRequired() || pInfo->IsEnabledByDefault();
		m_enabledState[id] = defaultState;
	}
	
	// Notify observers
	istd::CChangeNotifier notifier(this);
	
	return true;
}


QByteArrayList CFunctionalityManagerComp::ValidateConfiguration() const
{
	QByteArrayList issues;
	
	// Check that all enabled functionality has dependencies satisfied
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		const QByteArray& id = it.key();
		IFunctionalityInfo* pInfo = it.value();
		
		if (IsFunctionalityEnabled(id))
		{
			QByteArrayList deps = pInfo->GetDependencies();
			for (const QByteArray& dep : deps)
			{
				if (!IsFunctionalityEnabled(dep))
				{
					issues.append(id + " requires " + dep);
				}
			}
		}
	}
	
	return issues;
}


void CFunctionalityManagerComp::UpdateDependencyMap()
{
	m_reverseDependencies.clear();
	
	// Build reverse dependency map
	for (auto it = m_functionalityRegistry.constBegin(); it != m_functionalityRegistry.constEnd(); ++it)
	{
		const QByteArray& id = it.key();
		IFunctionalityInfo* pInfo = it.value();
		
		QByteArrayList deps = pInfo->GetDependencies();
		for (const QByteArray& dep : deps)
		{
			m_reverseDependencies[dep].insert(id);
		}
	}
}


bool CFunctionalityManagerComp::AreDependenciesSatisfied(const QByteArray& functionalityId) const
{
	IFunctionalityInfo* pInfo = GetFunctionalityInfo(functionalityId);
	if (!pInfo)
		return false;
	
	QByteArrayList deps = pInfo->GetDependencies();
	for (const QByteArray& dep : deps)
	{
		if (!IsFunctionalityEnabled(dep))
			return false;
	}
	
	return true;
}


bool CFunctionalityManagerComp::HasEnabledDependents(const QByteArray& functionalityId) const
{
	QSet<QByteArray> dependents = m_reverseDependencies.value(functionalityId);
	for (const QByteArray& dep : dependents)
	{
		if (IsFunctionalityEnabled(dep))
			return true;
	}
	
	return false;
}


} // namespace imtfunc

