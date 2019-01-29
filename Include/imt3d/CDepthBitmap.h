#pragma once


// ACF includes
#include <iimg/CReflectedBitmapBase.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imt3d
{


/**
	Implementation of a depth bitmap with integrated conversion to QImage (using for displaying the bitmap)
*/
class CDepthBitmap: public iimg::CReflectedBitmapBase, public idoc::CStandardDocumentMetaInfo 
{
public:
	typedef iimg::CReflectedBitmapBase BaseClass;
	typedef idoc::CStandardDocumentMetaInfo BaseClass2;

	enum
	{
		MIT_MIN_DEPTH = idoc::IDocumentMetaInfo::MIT_USER,
		MIT_MAX_DEPTH
	};

	void SetDepthRange(const istd::CRange& depthRange);
	istd::CRange GetDepthRange() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (iimg::CReflectedBitmapBase)
	virtual bool ConvertFromQImage(const QImage& image) override;
	virtual bool ConvertToQImage(QImage& result) const override;

private:
	void EnsureMetaInfoCreated();

private:
	istd::CRange m_depthRange;
};


} // namespace imt3d


