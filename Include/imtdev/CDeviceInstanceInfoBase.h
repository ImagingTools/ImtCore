#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtdev/IDeviceInstanceInfoEditor.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


class CDeviceInstanceInfoBase : virtual public IDeviceInstanceInfoEditor
{
public:
	CDeviceInstanceInfoBase();
	~CDeviceInstanceInfoBase();

	// reimplemented (imtdev::IDeviceInstanceInfoEditor)
	bool SetIdentifier(IdentifierTypes idType, const QByteArray& id) override;
	bool SetVersion(
		int versionId,
		quint32 version,
		const QString& name,
		const QString& description) override;
	iattr::IAttributesManager* GetAttributesManager() override;

	// reimplemented (imtdev::IDeviceInstanceInfo)
	QByteArray GetIdentifier(int idType) const override;
	const iser::IVersionInfo& GetVersion() const override;
	const iattr::IAttributesProvider* GetAttributes() const override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual const IDeviceStaticInfo& GetStaticInfo() const override = 0;

protected:
	class VersionInfo : virtual public iser::IVersionInfo
	{
	public:
		// reimplemenbted (iser::IVersionInfo)
		VersionIds GetVersionIds() const override;
		bool GetVersionNumber(int versionId, quint32& result) const override;
		QString GetVersionIdDescription(int versionId) const override;
		QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;

		// reimplemented (istd::IChangeable)
		int GetSupportedOperations() const override;
		bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
		IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
		bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	private:
		struct VersionItem
		{
			quint32 versionNumber;
			QString versionDescription;
		};

		QMap<int, VersionItem> m_versions;
	};

	mutable imod::CModelUpdateBridge m_updateBridge;

private:
	bool EnsureAttributesCreated() const;

private:
	QMap<int, QByteArray> m_identifiers;
	imod::TModelWrap<VersionInfo> m_versionInfo;
	mutable std::unique_ptr<iattr::IAttributesProvider> m_attributesPtr;
};


} // namespace imtdev
