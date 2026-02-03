// ImtCore includes
#include <imtfunc/CFunctionalityInfo.h>


namespace imtfunc
{


CFunctionalityInfo::CFunctionalityInfo(
	const QByteArray& id,
	const QString& name,
	const QString& description,
	FunctionalityType type,
	bool enabledByDefault,
	bool required,
	int riskLevel)
	: m_id(id)
	, m_name(name)
	, m_description(description)
	, m_type(type)
	, m_enabledByDefault(enabledByDefault)
	, m_required(required)
	, m_riskLevel(riskLevel)
{
	// Clamp risk level to valid range
	if (m_riskLevel < 0)
		m_riskLevel = 0;
	else if (m_riskLevel > 3)
		m_riskLevel = 3;
}


QByteArray CFunctionalityInfo::GetId() const
{
	return m_id;
}


QString CFunctionalityInfo::GetName() const
{
	return m_name;
}


QString CFunctionalityInfo::GetDescription() const
{
	return m_description;
}


FunctionalityType CFunctionalityInfo::GetType() const
{
	return m_type;
}


bool CFunctionalityInfo::IsEnabledByDefault() const
{
	return m_enabledByDefault;
}


bool CFunctionalityInfo::IsRequired() const
{
	return m_required;
}


QByteArrayList CFunctionalityInfo::GetDependencies() const
{
	return m_dependencies;
}


int CFunctionalityInfo::GetRiskLevel() const
{
	return m_riskLevel;
}


void CFunctionalityInfo::AddDependency(const QByteArray& dependencyId)
{
	if (!m_dependencies.contains(dependencyId))
	{
		m_dependencies.append(dependencyId);
	}
}


void CFunctionalityInfo::SetDependencies(const QByteArrayList& dependencies)
{
	m_dependencies = dependencies;
}


} // namespace imtfunc

