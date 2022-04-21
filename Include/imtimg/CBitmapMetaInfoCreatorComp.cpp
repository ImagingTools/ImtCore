#include <imtimg/CBitmapMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iimg/IBitmap.h>

// ImtCore includes
#include <imtimg/CBitmapMetaInfo.h>


namespace imtimg
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CBitmapMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CBitmapMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const iimg::IBitmap* bitmapPtr = dynamic_cast<const iimg::IBitmap*>(dataPtr);
	if (bitmapPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_DIMENSION_X, QString::number(bitmapPtr->GetImageSize().GetX()));
	metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_DIMENSION_Y, QString::number(bitmapPtr->GetImageSize().GetY()));
	metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_PIXEL_BIT_DEPTH, QString::number(bitmapPtr->GetPixelBitsCount()) + QObject::tr(" bits"));

	iimg::IBitmap::PixelFormat format = bitmapPtr->GetPixelFormat();
	switch (format){
	case iimg::IBitmap::PF_GRAY32:
	case iimg::IBitmap::PF_GRAY16:
	case iimg::IBitmap::PF_MONO:
	case iimg::IBitmap::PF_GRAY:
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_FORMAT, QObject::tr("Gray"));
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_COLOR_SPACE_TYPE, QObject::tr("Grayscale"));
		break;
	case iimg::IBitmap::PF_RGB24:
	case iimg::IBitmap::PF_RGB:
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_FORMAT, QObject::tr("RGB"));
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_COLOR_SPACE_TYPE, QObject::tr("RGB"));
		break;
	case iimg::IBitmap::PF_RGBA:
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_FORMAT, QObject::tr("RGBA"));
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_COLOR_SPACE_TYPE, QObject::tr("RGBA"));
		break;

	case iimg::IBitmap::PF_FLOAT32:
	case iimg::IBitmap::PF_FLOAT64:
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_BITMAP_FORMAT, QObject::tr("Float"));
		metaInfoPtr->SetMetaInfo(IBitmapMetaInfo::MIT_COLOR_SPACE_TYPE, QObject::tr("Unknown"));
		break;
	}

	return true;
}


} // namespace imtimg


