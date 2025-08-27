#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtdev/IEditableDeviceInstanceInfo.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


class CDeviceInstanceInfoBase : virtual public IEditableDeviceInstanceInfo
{
public:
	CDeviceInstanceInfoBase();
	~CDeviceInstanceInfoBase();

	// reimplemented (imtdev::IEditableDeviceInstanceInfo)
	virtual bool SetIdentifier(IdentifierTypes idType, const QByteArray& id) override;
	virtual bool SetVersion(
		int versionId,
		quint32 version,
		const QString& name,
		const QString& description) override;
	virtual iattr::IAttributesManager* GetAttributesManager() override;

	// reimplemented (imtdev::IDeviceInstanceInfo)
	virtual QByteArray GetIdentifier(int idType) const override;
	virtual const iser::IVersionInfo& GetVersion() const override;
	virtual const iattr::IAttributesProvider* GetAttributes() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	class VersionInfo : virtual public iser::IVersionInfo
	{
	public:
		// reimplemenbted (iser::IVersionInfo)
		virtual VersionIds GetVersionIds() const override;
		virtual bool GetVersionNumber(int versionId, quint32& result) const override;
		virtual QString GetVersionIdDescription(int versionId) const override;
		virtual QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;

		// reimplemented (istd::IChangeable)
		virtual int GetSupportedOperations() const override;
		virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
		virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
		virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	private:
		struct VersionItem
		{
			quint32 versionNumber;
			QString versionDescription;
		};

		QMap<int, VersionItem> m_versions;
	};

	mutable imod::CModelUpdateBridge m_updateBridge;
	mutable std::unique_ptr<iattr::IAttributesProvider> m_attributesPtr;

private:
	bool EnsureAttributesCreated() const;

private:
	QMap<int, QByteArray> m_identifiers;
	imod::TModelWrap<VersionInfo> m_versionInfo;
};


} // namespace imtdev
