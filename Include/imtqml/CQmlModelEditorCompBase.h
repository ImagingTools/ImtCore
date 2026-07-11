// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtqml/CQmlGuiCompBase.h>


namespace imtqml
{


/**
	\deprecated Part of the imperative \c setRepresentation /
	\c representationChanged editor contract. New code should use the
	declarative binding stack instead: \c CObjectViewModel +
	\c CDataModelController + \c IDataModelBridge. See
	Docs/Qml/DeclarativeModelBinding_MigrationGuide.md.
*/
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


