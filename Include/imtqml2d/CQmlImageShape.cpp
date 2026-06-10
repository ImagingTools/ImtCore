// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CQmlImageShape.h>
#include <imtqml2d/CBitmapImageProvider.h>

// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QUuid>


namespace imtqml2d
{


// public

CQmlImageShape::CQmlImageShape()
	: m_imageKey(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}


CQmlImageShape::~CQmlImageShape()
{
	CBitmapImageProvider* provider = CBitmapImageProvider::GetInstance();
	if (provider != nullptr){
		provider->RemoveImage(m_imageKey);
	}
}


bool CQmlImageShape::IsValid() const
{
	return !m_image.isNull();
}


QImage CQmlImageShape::GetImage() const
{
	return m_image;
}


QString CQmlImageShape::GetImageUrl() const
{
	if (m_image.isNull()){
		return QString();
	}

	return QStringLiteral("image://bitmap/") + m_imageKey;
}


// reimplemented (sdl::V1_0::imtsdl2d::CImageShape)

void CQmlImageShape::SetRect(sdl::V1_0::imtsdl2d::CRectangle2d newRect)
{
	BaseClass2::SetRect(newRect);
}


void CQmlImageShape::SetImageData(const QByteArray& newImageData)
{
	BaseClass2::SetImageData(newImageData);
}


void CQmlImageShape::SetImageFormat(int newImageFormat)
{
	BaseClass2::SetImageFormat(newImageFormat);
}


// reimplemented (imod::TSingleModelObserverBase)

bool CQmlImageShape::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	iimg::IBitmap* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		BaseClass::EnsureModelDetached();
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return false;
	}

	UpdateImageFromBitmap();

	return retVal;
}


bool CQmlImageShape::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass::OnModelDetached(modelPtr);

	m_image = QImage();

	CBitmapImageProvider* provider = CBitmapImageProvider::GetInstance();
	if (provider != nullptr){
		provider->RemoveImage(m_imageKey);
	}

	Q_EMIT imageChanged();

	return retVal;
}


void CQmlImageShape::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iimg::IBitmap* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		Q_ASSERT_X(false, Q_FUNC_INFO, "NULL model");

		return;
	}

	UpdateImageFromBitmap();
}


// private

void CQmlImageShape::UpdateImageFromBitmap()
{
	iimg::IBitmap* bitmapPtr = GetObjectPtr();
	if (bitmapPtr == nullptr || bitmapPtr->IsEmpty()){
		m_image = QImage();
		return;
	}

	istd::CIndex2d size = bitmapPtr->GetImageSize();
	int width = size.GetX();
	int height = size.GetY();

	if (width <= 0 || height <= 0){
		m_image = QImage();
		return;
	}

	QImage::Format qFormat = QImage::Format_Invalid;

	iimg::IBitmap::PixelFormat pixelFormat = bitmapPtr->GetPixelFormat();
	switch (pixelFormat){
		case iimg::IBitmap::PF_GRAY:
			qFormat = QImage::Format_Grayscale8;
			break;
		case iimg::IBitmap::PF_GRAY16:
			qFormat = QImage::Format_Grayscale16;
			break;
		case iimg::IBitmap::PF_RGB:
		case iimg::IBitmap::PF_RGB24:
			qFormat = QImage::Format_RGB888;
			break;
		case iimg::IBitmap::PF_RGBA:
			qFormat = QImage::Format_RGBA8888;
			break;
		case iimg::IBitmap::PF_RGBA64:
			qFormat = QImage::Format_RGBA64;
			break;
		case iimg::IBitmap::PF_MONO:
			qFormat = QImage::Format_Mono;
			break;
		default:
			// For unsupported formats, convert via color access
			qFormat = QImage::Format_RGBA8888;
			break;
	}

	bool useDirectAccess =
		pixelFormat == iimg::IBitmap::PF_GRAY
		|| pixelFormat == iimg::IBitmap::PF_GRAY16
		|| pixelFormat == iimg::IBitmap::PF_RGB
		|| pixelFormat == iimg::IBitmap::PF_RGB24
		|| pixelFormat == iimg::IBitmap::PF_RGBA
		|| pixelFormat == iimg::IBitmap::PF_RGBA64
		|| pixelFormat == iimg::IBitmap::PF_MONO;

	if (useDirectAccess){
		int bytesPerLine = bitmapPtr->GetLineBytesCount();
		m_image = QImage(width, height, qFormat);

		for (int y = 0; y < height; ++y){
			const quint8* srcLine = bitmapPtr->GetLinePtr(y);
			if (srcLine != nullptr){
				memcpy(m_image.scanLine(y), srcLine, qMin(bytesPerLine, m_image.bytesPerLine()));
			}
		}
	}
	else {
		// Fallback: pixel-by-pixel conversion via GetColorAt
		m_image = QImage(width, height, QImage::Format_RGBA8888);

		for (int y = 0; y < height; ++y){
			quint8* destLine = m_image.scanLine(y);
			for (int x = 0; x < width; ++x){
				icmm::CVarColor color = bitmapPtr->GetColorAt(istd::CIndex2d(x, y));
				int offset = x * 4;
				destLine[offset + 0] = static_cast<quint8>(color.GetComponent(0) * 255.0);
				destLine[offset + 1] = static_cast<quint8>(color.GetComponent(1) * 255.0);
				destLine[offset + 2] = static_cast<quint8>(color.GetComponent(2) * 255.0);
				destLine[offset + 3] = (color.GetComponentsCount() > 3)
					? static_cast<quint8>(color.GetComponent(3) * 255.0)
					: 255;
			}
		}
	}

	// Encode image to Base64 PNG and update SDL property
	QByteArray imageBytes;
	QBuffer buffer(&imageBytes);
	buffer.open(QIODevice::WriteOnly);
	m_image.save(&buffer, "PNG");

	BaseClass2::SetImageData(imageBytes.toBase64());
	BaseClass2::SetImageFormat(0); // PNG

	// Update rect from image dimensions if not already set
	sdl::V1_0::imtsdl2d::CRectangle2d rect = BaseClass2::GetRect();
	sdl::V1_0::imtsdl2d::CPoint2d bottomRight = rect.GetBottomRight();
	if (bottomRight.GetX() == 0. && bottomRight.GetY() == 0.){
		bottomRight.SetX(width);
		bottomRight.SetY(height);
		rect.SetBottomRight(bottomRight);
		BaseClass2::SetRect(rect);
	}

	// Register image with the provider for QML Canvas access
	CBitmapImageProvider* provider = CBitmapImageProvider::GetInstance();
	if (provider != nullptr){
		provider->SetImage(m_imageKey, m_image);
	}

	Q_EMIT imageChanged();
}


} // namespace imtqml2d
