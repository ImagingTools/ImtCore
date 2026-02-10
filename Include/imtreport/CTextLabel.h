// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	CFont m_font;
	i2d::CRectangle m_rectangle;
	Qt::Alignment m_alignment;
};


} // namespace imtreport


