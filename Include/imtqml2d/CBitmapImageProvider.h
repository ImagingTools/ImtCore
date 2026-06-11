// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtQuick/QQuickImageProvider>


namespace imtqml2d
{


/*!
 * \brief QML image provider for IBitmap-backed images.
 *
 * Registers under the provider id "bitmap" and serves images by string key.
 * CQmlImageShape instances register / unregister their QImage here so that
 * the QML Canvas can load them with:
 * \code
 *   canvas.loadImage("image://bitmap/<key>")
 *   ctx.drawImage("image://bitmap/<key>", x, y, w, h)
 * \endcode
 */
class CBitmapImageProvider: public QQuickImageProvider
{
public:
	CBitmapImageProvider();

	// reimplemented (QQuickImageProvider)
	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

	void SetImage(const QString& key, const QImage& image);
	void RemoveImage(const QString& key);

	static CBitmapImageProvider* GetInstance();

private:
	QHash<QString, QImage> m_images;
	mutable QMutex m_mutex;

	static CBitmapImageProvider* s_instance;
};


} // namespace imtqml2d
