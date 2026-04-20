// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtqml/IRepresentationControllerBridge.h>


namespace imtqml
{


/**
	\brief Default implementation of \c IRepresentationControllerBridge:
	a single, transport-agnostic registry mapping \c documentId to
	the \c CDocumentRepresentationController that handles it.

	\details
	Per reviewer feedback the registry is a single icomp component
	shared by all \c CDocumentRepresentationController instances
	regardless of document type. It is intentionally NOT exposed to
	QML — controllers self-register through the
	\c IRepresentationControllerBridge interface in their
	\c componentComplete() override.

	Controllers are stored as \c QPointer so that destruction of a
	controller automatically clears its mapping; lookups for a
	destroyed controller return \c nullptr without crashing.
*/
class CRepresentationControllerBridge:
			public icomp::CComponentBase,
			virtual public IRepresentationControllerBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationControllerBridge);
		I_REGISTER_INTERFACE(IRepresentationControllerBridge);
	I_END_COMPONENT;

	CRepresentationControllerBridge();
	~CRepresentationControllerBridge() override;

	// reimplemented (IRepresentationControllerBridge)
	virtual void RegisterRepresentationController(const QString& documentId, QObject* controller) override;
	virtual void UnregisterRepresentationController(const QString& documentId) override;
	virtual QObject* GetRepresentationController(const QString& documentId) const override;
	virtual QStringList RegisteredRepresentationControllerIds() const override;

private:
	QHash<QString, QPointer<QObject>> m_representationControllers;
};


} // namespace imtqml
