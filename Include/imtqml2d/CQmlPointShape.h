// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <i2d/CVector2d.h>
#include <imod/TSingleModelObserverBase.h>

/// \todo REMOVWE IT!
#include "dummy.h"


namespace imtqml2d
{


class CQmlPointShape: public sdl::imtsdl2d::imt2d::CPoint2d, public imod::TSingleModelObserverBase<i2d::CVector2d>
{
	Q_OBJECT

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef sdl::imtsdl2d::imt2d::CPoint2d BaseClass2;

	[[nodiscard]] bool IsValid() const;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented CPoint2d
	virtual void SetX(double newX) override;
	virtual void SetY(double newY) override;
};


} // namespace imtqml2d
