// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtassist/IAssistantConfiguration.h>


namespace imtassist
{


/**
	Configuration of the assistant of one tenant. Disabled and without provider by default.

	\ingroup imtassist
*/
class CAssistantConfiguration: virtual public IAssistantConfiguration
{
public:
	CAssistantConfiguration();

	// reimplemented (imtassist::IAssistantConfiguration)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual bool IsEnabled() const override;
	virtual void SetEnabled(bool isEnabled) override;
	virtual QByteArray GetPresetId() const override;
	virtual void SetPresetId(const QByteArray& presetId) override;
	virtual QString GetModel() const override;
	virtual void SetModel(const QString& model) override;
	virtual QString GetEndpoint() const override;
	virtual void SetEndpoint(const QString& endpoint) override;
	virtual QString GetRegion() const override;
	virtual void SetRegion(const QString& region) override;
	virtual QString GetProject() const override;
	virtual void SetProject(const QString& project) override;
	virtual QString GetResource() const override;
	virtual void SetResource(const QString& resource) override;
	virtual QByteArray GetProtectedApiKey() const override;
	virtual void SetProtectedApiKey(const QByteArray& protectedApiKey) override;
	virtual int GetMaxOutputTokens() const override;
	virtual void SetMaxOutputTokens(int maxOutputTokens) override;
	virtual QString GetAdditionalInstructions() const override;
	virtual void SetAdditionalInstructions(const QString& instructions) override;
	virtual QDateTime GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QDateTime& updatedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_tenantId;
	bool m_isEnabled;
	QByteArray m_presetId;
	QString m_model;
	QString m_endpoint;
	QString m_region;
	QString m_project;
	QString m_resource;
	QByteArray m_protectedApiKey;
	int m_maxOutputTokens;
	QString m_additionalInstructions;
	QDateTime m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CAssistantConfiguration> CIdentifiableAssistantConfiguration;


} // namespace imtassist


