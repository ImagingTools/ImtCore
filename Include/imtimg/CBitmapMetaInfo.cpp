#include <imtimg/CBitmapMetaInfo.h>


// ACF includes
#include <iimg/IBitmap.h>


namespace imtimg
{


QString CBitmapMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IBitmapMetaInfo::MIT_ACQUISITION_DEVICE:
		return QObject::tr("Device");
	case IBitmapMetaInfo::MIT_BITMAP_DIMENSION_X:
		return QObject::tr("Width");
	case IBitmapMetaInfo::MIT_BITMAP_DIMENSION_Y:
		return QObject::tr("Height");
	case IBitmapMetaInfo::MIT_BITMAP_DIMENSION_Z:
		return QObject::tr("Depth");
	case IBitmapMetaInfo::MIT_BITMAP_FORMAT:
		return QObject::tr("Format");
	case IBitmapMetaInfo::MIT_COLOR_SPACE_DESCRIPTION:
		return QObject::tr("Color Space");
	case IBitmapMetaInfo::MIT_COLOR_SPACE_TYPE:
		return QObject::tr("Color Space Type");
	case IBitmapMetaInfo::MIT_PIXEL_BIT_DEPTH:
		return QObject::tr("Pixel Depth");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtimg


