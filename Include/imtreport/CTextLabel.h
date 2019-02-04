#pragma once

// ACF includes
#include <i2d/CLabel.h>
#include <i2d/CRectangle.h>

// ImtCore
#include <imtreport/CFont.h>


namespace imtreport
{


class CTextLabel: public i2d::CLabel
{
public:
	typedef i2d::CLabel BaseClass;

	CTextLabel();

	/**
		Get label font.
	*/
	virtual const CFont& GetFont() const;

	/**
		Set label font.
	*/
	virtual void SetFont(const CFont& font);

	/**
		Get label's rectangle.
	*/
	virtual const i2d::CRectangle& GetRectangle() const;

	/**
		Set label's rectangle.
	*/
	virtual void SetRectangle(const i2d::CRectangle& rectangle);

	/**
		Get label's text alignment.
	*/
	virtual Qt::Alignment GetAlignment() const;

	/**
		Set label's text alignment.
	*/
	virtual void SetAlignment(const Qt::Alignment alignment);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;

private:
	CFont m_font;
	i2d::CRectangle m_rectangle;
	Qt::Alignment m_alignment;
};


} // namespace imtreport


