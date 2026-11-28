// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantConfiguration.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CChangeNotifier.h>


namespace imtassist
{


namespace
{


template <typename Value>
void AssignValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


} // anonymous namespace


// public methods

CAssistantConfiguration::CAssistantConfiguration():
	m_isEnabled(false),
	m_maxOutputTokens(DEFAULT_MAX_OUTPUT_TOKENS)
{
}


// reimplemented (imtassist::IAssistantConfiguration)

QByteArray CAssistantConfiguration::GetTenantId() const
{
	return m_tenantId;
}


void CAssistantConfiguration::SetTenantId(const QByteArray& value)
{
	AssignValue(*this, m_tenantId, value);
}


bool CAssistantConfiguration::IsEnabled() const
{
	return m_isEnabled;
}


void CAssistantConfiguration::SetEnabled(bool value)
{
	AssignValue(*this, m_isEnabled, value);
}


QByteArray CAssistantConfiguration::GetPresetId() const
{
	return m_presetId;
}


void CAssistantConfiguration::SetPresetId(const QByteArray& value)
{
	AssignValue(*this, m_presetId, value);
}


QString CAssistantConfiguration::GetModel() const
{
	return m_model;
}


void CAssistantConfiguration::SetModel(const QString& value)
{
	AssignValue(*this, m_model, value);
}


QString CAssistantConfiguration::GetEndpoint() const
{
	return m_endpoint;
}


void CAssistantConfiguration::SetEndpoint(const QString& value)
{
	AssignValue(*this, m_endpoint, value);
}


QString CAssistantConfiguration::GetRegion() const
{
	return m_region;
}


void CAssistantConfiguration::SetRegion(const QString& value)
{
	AssignValue(*this, m_region, value);
}


QString CAssistantConfiguration::GetProject() const
{
	return m_project;
}


void CAssistantConfiguration::SetProject(const QString& value)
{
	AssignValue(*this, m_project, value);
}


QString CAssistantConfiguration::GetResource() const
{
	return m_resource;
}


void CAssistantConfiguration::SetResource(const QString& value)
{
	AssignValue(*this, m_resource, value);
}


QByteArray CAssistantConfiguration::GetProtectedApiKey() const
{
	return m_protectedApiKey;
}


void CAssistantConfiguration::SetProtectedApiKey(const QByteArray& value)
{
	AssignValue(*this, m_protectedApiKey, value);
}


int CAssistantConfiguration::GetMaxOutputTokens() const
{
	return m_maxOutputTokens;
}


void CAssistantConfiguration::SetMaxOutputTokens(int value)
{
	AssignValue(*this, m_maxOutputTokens, value);
}


QString CAssistantConfiguration::GetAdditionalInstructions() const
{
	return m_additionalInstructions;
}


void CAssistantConfiguration::SetAdditionalInstructions(const QString& value)
{
	AssignValue(*this, m_additionalInstructions, value);
}


QDateTime CAssistantConfiguration::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CAssistantConfiguration::SetUpdatedAt(const QDateTime& value)
{
	AssignValue(*this, m_updatedAt, value);
}


// reimplemented (iser::ISerializable)

bool CAssistantConfiguration::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag enabledTag("Enabled", "Is enabled", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(enabledTag);
	retVal = retVal && archive.Process(m_isEnabled);
	retVal = retVal && archive.EndTag(enabledTag);

	iser::CArchiveTag presetIdTag("PresetId", "Provider preset", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(presetIdTag);
	retVal = retVal && archive.Process(m_presetId);
	retVal = retVal && archive.EndTag(presetIdTag);

	iser::CArchiveTag modelTag("Model", "Model", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(modelTag);
	retVal = retVal && archive.Process(m_model);
	retVal = retVal && archive.EndTag(modelTag);

	iser::CArchiveTag endpointTag("Endpoint", "Endpoint", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(endpointTag);
	retVal = retVal && archive.Process(m_endpoint);
	retVal = retVal && archive.EndTag(endpointTag);

	iser::CArchiveTag regionTag("Region", "Region", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(regionTag);
	retVal = retVal && archive.Process(m_region);
	retVal = retVal && archive.EndTag(regionTag);

	iser::CArchiveTag projectTag("Project", "Project", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(projectTag);
	retVal = retVal && archive.Process(m_project);
	retVal = retVal && archive.EndTag(projectTag);

	iser::CArchiveTag resourceTag("Resource", "Resource", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(resourceTag);
	retVal = retVal && archive.Process(m_resource);
	retVal = retVal && archive.EndTag(resourceTag);

	iser::CArchiveTag protectedApiKeyTag("ProtectedApiKey", "Protected API key", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(protectedApiKeyTag);
	retVal = retVal && archive.Process(m_protectedApiKey);
	retVal = retVal && archive.EndTag(protectedApiKeyTag);

	iser::CArchiveTag maxOutputTokensTag("MaxOutputTokens", "Maximum output tokens", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(maxOutputTokensTag);
	retVal = retVal && archive.Process(m_maxOutputTokens);
	retVal = retVal && archive.EndTag(maxOutputTokensTag);

	iser::CArchiveTag additionalInstructionsTag("AdditionalInstructions", "Additional instructions", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(additionalInstructionsTag);
	retVal = retVal && archive.Process(m_additionalInstructions);
	retVal = retVal && archive.EndTag(additionalInstructionsTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CAssistantConfiguration::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IAssistantConfiguration* sourcePtr = dynamic_cast<const IAssistantConfiguration*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_tenantId = sourcePtr->GetTenantId();
	m_isEnabled = sourcePtr->IsEnabled();
	m_presetId = sourcePtr->GetPresetId();
	m_model = sourcePtr->GetModel();
	m_endpoint = sourcePtr->GetEndpoint();
	m_region = sourcePtr->GetRegion();
	m_project = sourcePtr->GetProject();
	m_resource = sourcePtr->GetResource();
	m_protectedApiKey = sourcePtr->GetProtectedApiKey();
	m_maxOutputTokens = sourcePtr->GetMaxOutputTokens();
	m_additionalInstructions = sourcePtr->GetAdditionalInstructions();
	m_updatedAt = sourcePtr->GetUpdatedAt();

	return true;
}


bool CAssistantConfiguration::IsEqual(const IChangeable& object) const
{
	const IAssistantConfiguration* otherPtr = dynamic_cast<const IAssistantConfiguration*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return
				(m_tenantId == otherPtr->GetTenantId()) &&
				(m_isEnabled == otherPtr->IsEnabled()) &&
				(m_presetId == otherPtr->GetPresetId()) &&
				(m_model == otherPtr->GetModel()) &&
				(m_endpoint == otherPtr->GetEndpoint()) &&
				(m_region == otherPtr->GetRegion()) &&
				(m_project == otherPtr->GetProject()) &&
				(m_resource == otherPtr->GetResource()) &&
				(m_protectedApiKey == otherPtr->GetProtectedApiKey()) &&
				(m_maxOutputTokens == otherPtr->GetMaxOutputTokens()) &&
				(m_additionalInstructions == otherPtr->GetAdditionalInstructions()) &&
				(m_updatedAt == otherPtr->GetUpdatedAt());
}


istd::IChangeableUniquePtr CAssistantConfiguration::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CAssistantConfiguration);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CAssistantConfiguration::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId = QByteArray();
	m_isEnabled = false;
	m_presetId = QByteArray();
	m_model = QString();
	m_endpoint = QString();
	m_region = QString();
	m_project = QString();
	m_resource = QString();
	m_protectedApiKey = QByteArray();
	m_maxOutputTokens = DEFAULT_MAX_OUTPUT_TOKENS;
	m_additionalInstructions = QString();
	m_updatedAt = QDateTime();

	return true;
}


} // namespace imtassist


