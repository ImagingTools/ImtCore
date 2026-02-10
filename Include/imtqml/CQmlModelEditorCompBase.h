// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtqml/CQmlGuiCompBase.h>


namespace imtqml
{


class CQmlModelEditorCompBase: public CQmlGuiCompBase
{
	Q_OBJECT
public:
	typedef CQmlGuiCompBase BaseClass;

protected:
	// reimplemented (CQmlGuiCompBase)
	virtual void OnGuiCreated() override;

protected Q_SLOTS:
	virtual void OnRepresentationChanged();
};


} // namespace imtqml


