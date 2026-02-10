// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <i2d/CPolyline.h>
#include <imod/TSingleModelObserverBase.h>

/// \todo REMOVWE IT!
#include "dummy.h"


namespace imtqml2d
{


class CQmlPolyLineShape: public sdl::imtsdl2d::imt2d::CPolyline2d, public imod::TSingleModelObserverBase<i2d::CPolyline>
{
	Q_OBJECT

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef sdl::imtsdl2d::imt2d::CPolyline2d BaseClass2;

	[[nodiscard]] bool IsValid() const;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented CPolyline2d
public:
	virtual void SetPoints(const QList<sdl::imtsdl2d::imt2d::CPoint2d>& newPoints) override;
};


} // namespace imtqml2d
