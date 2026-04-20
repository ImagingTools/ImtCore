// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlBasedDataModelController.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtConcurrent/QtConcurrentRun>

// ImtCore includes
#include <imtqml/CGqlClientBridge.h>


namespace imtqml
{


namespace
{


/**
	Schedule a callable on the GUI thread. Same semantics as the
	helper used by other controllers in this package.
*/
template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


} // namespace


// public methods

CGqlBasedDataModelController::CGqlBasedDataModelController(QObject* parent)
	:BaseClass(parent)
{
}


CGqlBasedDataModelController::~CGqlBasedDataModelController() = default;


const QVariantMap& CGqlBasedDataModelController::GetParameters() const
{
	return m_parameters;
}


void CGqlBasedDataModelController::SetParameters(const QVariantMap& parameters)
{
	if (m_parameters != parameters){
		m_parameters = parameters;
		Q_EMIT parametersChanged(m_parameters);
	}
}


// public slots

void CGqlBasedDataModelController::getModel()
{
	Q_EMIT startGetModel();

	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT getModelFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	SetIsLoading(true);

	const QString modelId = GetModelId();
	const QVariantMap parameters = m_parameters;
	QPointer<CGqlBasedDataModelController> self(this);

	QtConcurrent::run([self, bridge, modelId, parameters]{
		QString errorMessage;
		QVariant model;
		if (self){
			model = self->FetchModel(bridge, modelId, parameters, errorMessage);
		}

		PostToMainThread([self, model, errorMessage]{
			if (!self){
				return;
			}
			self->SetIsLoading(false);
			if (!errorMessage.isEmpty()){
				Q_EMIT self->getModelFailed(errorMessage);
				return;
			}
			self->UpdateCachedModel(model);
			Q_EMIT self->modelReceived(model);
		});
	});
}


void CGqlBasedDataModelController::setModel(const QVariant& model)
{
	Q_EMIT startSetModel(model);

	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT setModelFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	SetIsLoading(true);

	const QString modelId = GetModelId();
	const QVariantMap parameters = m_parameters;
	QPointer<CGqlBasedDataModelController> self(this);

	QtConcurrent::run([self, bridge, modelId, parameters, model]{
		QString errorMessage;
		bool ok = false;
		if (self){
			ok = self->StoreModel(bridge, modelId, parameters, model, errorMessage);
		}

		PostToMainThread([self, model, ok, errorMessage]{
			if (!self){
				return;
			}
			self->SetIsLoading(false);
			if (!ok){
				const QString msg = errorMessage.isEmpty()
						? QStringLiteral("Failed to store model")
						: errorMessage;
				Q_EMIT self->setModelFailed(msg);
				return;
			}
			self->UpdateCachedModel(model);
			Q_EMIT self->modelSet();
		});
	});
}


// protected methods

QVariant CGqlBasedDataModelController::FetchModel(
		CGqlClientBridge* /*bridge*/,
		const QString& modelId,
		const QVariantMap& /*parameters*/,
		QString& errorMessage) const
{
	qWarning() << "CGqlBasedDataModelController::FetchModel() should be implemented in a subclass"
			   << "(modelId=" << modelId << ")";
	errorMessage = QStringLiteral("CGqlBasedDataModelController::FetchModel() not implemented");
	return QVariant();
}


bool CGqlBasedDataModelController::StoreModel(
		CGqlClientBridge* /*bridge*/,
		const QString& modelId,
		const QVariantMap& /*parameters*/,
		const QVariant& /*model*/,
		QString& errorMessage) const
{
	qWarning() << "CGqlBasedDataModelController::StoreModel() should be implemented in a subclass"
			   << "(modelId=" << modelId << ")";
	errorMessage = QStringLiteral("CGqlBasedDataModelController::StoreModel() not implemented");
	return false;
}


CGqlClientBridge* CGqlBasedDataModelController::ResolveBridge() const
{
	return CGqlClientBridge::Instance();
}


} // namespace imtqml
