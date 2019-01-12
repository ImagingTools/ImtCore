#pragma once


// ACF includes
#include <iimg/CReflectedBitmapBase.h>


namespace imt3d
{


/**
	Implementation of a depth bitmap with integrated conversion to QImage (using for displaying the bitmap)
*/
class CDepthBitmap: public iimg::CReflectedBitmapBase
{
public:
	typedef iimg::CReflectedBitmapBase BaseClass;

	void SetDepthRange(const istd::CRange& depthRange);
	istd::CRange GetDepthRange() const;

protected:
	// reimplemented (iimg::CReflectedBitmapBase)
	virtual bool ConvertFromQImage(const QImage& image) override;
	virtual bool ConvertToQImage(QImage& result) const override;

private:
	istd::CRange m_depthRange;
};


} // namespace imt3d


