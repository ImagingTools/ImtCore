// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtGui/QImage>

// ACF includes
#include <iimg/IBitmap.h>
#include <imod/TSingleModelObserverBase.h>

#include "dummy.h"


namespace imtqml2d
{


class CImageShape
{
public:
	sdl::V1_0::imtsdl2d::CRectangle2d m_rect;
	QByteArray m_imageData;
	int m_imageFormat = 0; // 0=PNG, 1=JPEG, 2=SVG

	[[nodiscard]] sdl::V1_0::imtsdl2d::CRectangle2d GetRect() const { return m_rect; }
	virtual void SetRect(sdl::V1_0::imtsdl2d::CRectangle2d newRect) { m_rect = newRect; }
	[[nodiscard]] QByteArray GetImageData() const { return m_imageData; }
	virtual void SetImageData(const QByteArray& newImageData) { m_imageData = newImageData; }
	[[nodiscard]] int GetImageFormat() const { return m_imageFormat; }
	virtual void SetImageFormat(int newImageFormat) { m_imageFormat = newImageFormat; }
};


class CQmlImageShape: public CImageShape, public imod::TSingleModelObserverBase<iimg::IBitmap>
{
	Q_OBJECT

	Q_PROPERTY(QString imageUrl READ GetImageUrl NOTIFY imageChanged)

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef CImageShape BaseClass2;

	CQmlImageShape();
	~CQmlImageShape() override;

	[[nodiscard]] bool IsValid() const;

	/*!
	 * \brief Returns the QImage converted from the attached IBitmap model.
	 */
	[[nodiscard]] QImage GetImage() const;

	/*!
	 * \brief Returns the image URL for use in QML Canvas drawImage().
	 *        Format: "image://bitmap/<unique-key>"
	 */
	[[nodiscard]] QString GetImageUrl() const;

Q_SIGNALS:
	void imageChanged();

public:
	// reimplemented (imtqml2d::CImageShape)
	virtual void SetRect(sdl::V1_0::imtsdl2d::CRectangle2d newRect) override;
	virtual void SetImageData(const QByteArray& newImageData) override;
	virtual void SetImageFormat(int newImageFormat) override;

protected:
	// reimplemented (imod::TSingleModelObserverBase)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void UpdateImageFromBitmap();

	QImage m_image;
	QString m_imageKey;
};


} // namespace imtqml2d
