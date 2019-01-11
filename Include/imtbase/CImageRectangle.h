#pragma once

// ACF includes
#include <i2d/CLabel.h>
#include <i2d/CRectangle.h>


namespace imtbase
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
	virtual int GetSupportedOperations() const;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;

private:
	QString m_imagePath;
};


} // namespace imtbase


