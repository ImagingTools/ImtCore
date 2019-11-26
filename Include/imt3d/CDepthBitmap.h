#pragma once


// ACF includes
#include <iimg/CReflectedBitmapBase.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imt3d/IDepthBitmap.h>


namespace imt3d
{


/**
	Implementation of a depth bitmap with integrated conversion to QImage (using for displaying the bitmap)
*/
class CDepthBitmap:
			virtual public imt3d::IDepthBitmap,
			public iimg::CReflectedBitmapBase,
			public idoc::CStandardDocumentMetaInfo
{
public:
	/**
		Types of color table used in the image view for visualization of the bitmap pixels.
	*/
	enum ColorMapType
	{
		/**
			Use jet color table.
		*/
		CMT_JET,

		/**
			Use grayscale table.
		*/
		CMT_GRAY
	};

	typedef iimg::CReflectedBitmapBase BaseClass;
	typedef idoc::CStandardDocumentMetaInfo BaseClass2;

	CDepthBitmap();

	void SetDepthRange(const istd::CRange& depthRange);
	void SetColorMap(ColorMapType colorMapType);

	// reimplemented (IDepthBitmap)
	istd::CRange GetDepthRange() const override;
	virtual bool CreateDepthBitmap(const istd::CRange & depthRange, const istd::CIndex2d & size) override;
	virtual bool CreateDepthBitmap(const istd::CRange & depthRange, const istd::CIndex2d & size, void * dataPtr, bool releaseFlag, int linesDifference = 0) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (iimg::CReflectedBitmapBase)
	virtual bool ConvertFromQImage(const QImage& image) override;
	virtual bool ConvertToQImage(QImage& result) const override;

private:
	void EnsureMetaInfoCreated();

private:
	istd::CRange m_depthRange;
	ColorMapType m_colorMapType;
};


} // namespace imt3d


