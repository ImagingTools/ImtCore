#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtimg
{


/**
	Meta-info creator for the bitmap object.
*/
class IBitmapMetaInfo: virtual public idoc::IDocumentMetaInfo
{
public:
	enum MetaInfoType
	{
		/**
			Width of the image.
		*/
		MIT_BITMAP_DIMENSION_X = idoc::IDocumentMetaInfo::MIT_USER,
	
		/**
			Height of the image.
		*/
		MIT_BITMAP_DIMENSION_Y,

		/**
			Depth of the 3D-image. Not used for 2D-bitmaps.
		*/
		MIT_BITMAP_DIMENSION_Z,

		/**
			Resulting bitmap pixel depth in bits.
		*/
		MIT_PIXEL_BIT_DEPTH,

		/**
			Format of the bitmap representation.
		*/
		MIT_BITMAP_FORMAT,

		/**
			Mathematical color space type (e.g. RGB, RGBA, Lab, XYZ, CMYK, N-channel).
		*/
		MIT_COLOR_SPACE_TYPE,

		/**
			Human-readable description of the color space or color standard/gamut.
		*/
		MIT_COLOR_SPACE_DESCRIPTION,

		/**
			Summary information about the acquisition device.
		*/
		MIT_ACQUISITION_DEVICE
	};
};


} // namespace imtimg


