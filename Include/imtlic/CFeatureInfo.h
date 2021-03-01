#pragma once


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


class CFeatureInfo: virtual public IFeatureInfo
{
public:
	CFeatureInfo();

	void SetFeatureId(const QByteArray& featureId);
	void SetFeatureName(const QString& featureName);

	// reimplemented (IFeatureInfo)
	virtual QByteArray GetFeatureId() const override;
	virtual QString GetFeatureName() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_id;
	QString m_name;
};


} // namespace imtlic


