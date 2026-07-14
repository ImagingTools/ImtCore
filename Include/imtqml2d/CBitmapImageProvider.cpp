// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml2d/CBitmapImageProvider.h>

// Qt includes
#include <QtCore/QMutexLocker>


namespace imtqml2d
{


CBitmapImageProvider* CBitmapImageProvider::s_instance = nullptr;


CBitmapImageProvider::CBitmapImageProvider()
	: QQuickImageProvider(QQuickImageProvider::Image)
{
	s_instance = this;
}


// reimplemented (QQuickImageProvider)

QImage CBitmapImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
	QMutexLocker locker(&m_mutex);

	QImage image = m_images.value(id);
	if (image.isNull()){
		if (size != nullptr){
			*size = QSize();
		}
		return QImage();
	}

	if (size != nullptr){
		*size = image.size();
	}

	if (requestedSize.isValid()){
		return image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	return image;
}


void CBitmapImageProvider::SetImage(const QString& key, const QImage& image)
{
	QMutexLocker locker(&m_mutex);
	m_images[key] = image;
}


void CBitmapImageProvider::RemoveImage(const QString& key)
{
	QMutexLocker locker(&m_mutex);
	m_images.remove(key);
}


CBitmapImageProvider* CBitmapImageProvider::GetInstance()
{
	return s_instance;
}


} // namespace imtqml2d
