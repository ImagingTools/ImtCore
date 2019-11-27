#pragma once

// ACF includes
#include <i2d/CLabel.h>
#include <i2d/CRectangle.h>


namespace imtreport
{


class CImageRectangle: public i2d::CRectangle
{
public:
	typedef i2d::CRectangle BaseClass;

	/**
		Get image path.
	*/
	virtual const QString& GetImagePath() const;

	/**
		Set image path.
	*/
	virtual void SetImagePath(const QString& imagePath);


	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_imagePath;
};


} // namespace imtreport


