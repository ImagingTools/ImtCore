#pragma once

// ACF includes
#include <i2d/CLabel.h>
#include <i2d/CRectangle.h>


namespace imtreports
{


class CTextLabel: public i2d::CLabel
{
public:
	typedef i2d::CLabel BaseClass;

	CTextLabel();

	/**
	Get label font name.
	*/
	virtual const QString& GetFontName() const;

	/**
		Set label font name.
	*/
	virtual void SetFontName(const QString& fontName);

	/**
		Get label font size.
	*/
	virtual double GetFontSize() const;

	/**
		Set label font size.
	*/
	virtual void SetFontSize(double fontSize);

	/**
		Get label rectangle.
	*/
	virtual const i2d::CRectangle& GetRectangle() const;

	/**
		Set label rectangle.
	*/
	virtual void SetRectangle(const i2d::CRectangle& rectangle);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;

private:
	QString m_fontName;
	double m_fontSize;
	i2d::CRectangle m_rectangle;
};


} // namespace imtreports


