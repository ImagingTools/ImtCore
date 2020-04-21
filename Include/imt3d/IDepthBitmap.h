#pragma once


// ACF includes
#include <istd/TRange.h>
#include <iimg/IBitmap.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imt3d/imt3d.h>


namespace imt3d
{


/**
	Interface for a depth (range) image.
*/
class IDepthBitmap: virtual public iimg::IBitmap
{
public:
	enum
	{
		MIT_MIN_DEPTH = idoc::IDocumentMetaInfo::MIT_USER,
		MIT_MAX_DEPTH,

		/**
			Matrix used for transofrmation of depth values into the world coordinates given as QMatrix4x4
		*/
		MIT_REPROJECTION_MATRIX,

		MIT_LAST = MIT_REPROJECTION_MATRIX
	};

	/**
		Get the depth range of the bitmap space. The depth range defines the possible range of the pixel values in the bitmap.
	*/
	virtual istd::CRange GetDepthRange() const = 0;

	/**
		Create bitmap with specified size and format.
		\param	size			bitmap size.
		\param	pixelBitsCount	bits per pixel. Default value \c 0 means that the number of bits per pixel is defined by format specification.
		\param	componentsCount	components per pixel. Default value \c 0 means that the number of bits per pixel is defined by format specification.
	*/
	virtual bool CreateDepthBitmap(
				const istd::CRange& depthRange,
				const istd::CIndex2d& size) = 0;

	/**
		Create bitmap with specified size and format using external image data buffer.
		\param	size			bitmap size.
		\param	dataPtr			pointer to external image buffer.
		\param	releaseFlag		if its true, external buffer will be managed (removed) by this object.
		\param	linesDifference	address difference between next and previos line. If it equals 0, the value will be taken from size and number of bits per pixel.
	*/
	virtual bool CreateDepthBitmap(
				const istd::CRange& depthRange,
				const istd::CIndex2d& size,
				void* dataPtr,
				bool releaseFlag,
				int linesDifference = 0) = 0;

};


} // namespace imt3d


