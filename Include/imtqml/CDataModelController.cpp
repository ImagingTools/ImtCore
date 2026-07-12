// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDataModelController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QPointer>

// ImtCore includes
#include <imtqml/CDataModelBridgeDemultiplexer.h>
#include <imtqml/IDataModelBridge.h>


namespace imtqml
{


namespace
{

QString NoBridgeError()
{
	return QStringLiteral("CDataModelController: no IDataModelBridge available "
			"(CDataModelBridgeDemultiplexer component is not loaded)");
}

} // anonymous namespace


// public methods

CDataModelController::CDataModelController(QObject* parent)
	:BaseClass(parent),
	m_viewModelPtr(new CObjectViewModel(this))
{
	connect(m_viewModelPtr, &CObjectViewModel::isDirtyChanged,
			this, &CDataModelController::isDirtyChanged);
	connect(m_viewModelPtr, &CObjectViewModel::valueEdited,
			this, [this](const QString&, const QVariant&){ OnViewModelEdited(); });
}


CDataModelController::~CDataModelController()
{
	if (m_subscriptionId != 0){
		IDataModelBridge* bridge = CDataModelBridgeDemultiplexer::Instance();
		if (bridge != nullptr){
			bridge->UnsubscribeModel(m_subscriptionId);
		}
		m_subscriptionId = 0;
	}
}


const QString& CDataModelController::GetModelId() const
{
	return m_modelId;
}


void CDataModelController::SetModelId(const QString& modelId)
{
	if (m_modelId != modelId){
		m_modelId = modelId;
		Q_EMIT modelIdChanged(m_modelId);

		UpdateSubscription();
	}
}


const QVariantMap& CDataModelController::GetParameters() const
{
	return m_parameters;
}


void CDataModelController::SetParameters(const QVariantMap& parameters)
{
	if (m_parameters != parameters){
		m_parameters = parameters;
		Q_EMIT parametersChanged(m_parameters);

		UpdateSubscription();
	}
}


const QVariant& CDataModelController::GetModel() const
{
	return m_model;
}


bool CDataModelController::IsLoading() const
{
	return m_isLoading;
}


CObjectViewModel* CDataModelController::GetViewModel() const
{
	return m_viewModelPtr;
}


const QString& CDataModelController::GetError() const
{
	return m_error;
}


bool CDataModelController::IsDirty() const
{
	if (m_viewModelPtr == nullptr){
		return false;
	}

	return m_viewModelPtr->IsDirty();
}


bool CDataModelController::IsAutoSubmit() const
{
	return m_isAutoSubmit;
}


void CDataModelController::SetAutoSubmit(bool autoSubmit)
{
	if (m_isAutoSubmit != autoSubmit){
		m_isAutoSubmit = autoSubmit;
		Q_EMIT autoSubmitChanged(m_isAutoSubmit);
	}
}


bool CDataModelController::IsLive() const
{
	return m_isLive;
}


void CDataModelController::SetLive(bool live)
{
	if (m_isLive != live){
		m_isLive = live;
		Q_EMIT liveChanged(m_isLive);

		UpdateSubscription();
	}
}


// public slots

void CDataModelController::getModel()
{
	Q_EMIT startGetModel();

	IDataModelBridge* bridge = CDataModelBridgeDemultiplexer::Instance();
	if (bridge == nullptr){
		qWarning() << "CDataModelController::getModel():" << NoBridgeError()
				<< "(modelId=" << m_modelId << ")";
		Q_EMIT getModelFailed(NoBridgeError());
		return;
	}

	SetIsLoading(true);
	QPointer<CDataModelController> self(this);
	const QString modelId = m_modelId;
	bridge->GetModel(modelId, m_parameters,
			[self, modelId](QVariant model, QString errorMessage){
				if (self.isNull()){
					return;
				}
				self->SetIsLoading(false);
				if (!errorMessage.isEmpty()){
					self->SetError(errorMessage);
					Q_EMIT self->getModelFailed(errorMessage);
					return;
				}
				self->SetError(QString{});
				self->ApplySourceModel(model);
				Q_EMIT self->modelReceived(model);
			});
}


void CDataModelController::setModel(const QVariant& model)
{
	Q_EMIT startSetModel(model);

	IDataModelBridge* bridge = CDataModelBridgeDemultiplexer::Instance();
	if (bridge == nullptr){
		qWarning() << "CDataModelController::setModel():" << NoBridgeError()
				<< "(modelId=" << m_modelId << ")";
		Q_EMIT setModelFailed(NoBridgeError());
		return;
	}

	SetIsLoading(true);
	QPointer<CDataModelController> self(this);
	const QString modelId = m_modelId;
	bridge->SetModel(modelId, m_parameters, model,
			[self, model](QString errorMessage){
				if (self.isNull()){
					return;
				}
				self->SetIsLoading(false);
				if (!errorMessage.isEmpty()){
					self->SetError(errorMessage);
					Q_EMIT self->setModelFailed(errorMessage);
					return;
				}
				self->SetError(QString{});
				self->UpdateCachedModel(model);
				if (self->m_viewModelPtr != nullptr){
					self->m_viewModelPtr->MarkClean();
				}
				Q_EMIT self->modelSet();
			});
}


void CDataModelController::submit()
{
	if (m_viewModelPtr == nullptr){
		return;
	}

	setModel(QVariant::fromValue(m_viewModelPtr->GetValues()));
}


void CDataModelController::revert()
{
	if (m_viewModelPtr == nullptr){
		return;
	}

	m_viewModelPtr->revert();
}


// protected methods

void CDataModelController::UpdateCachedModel(const QVariant& model)
{
	if (m_model != model){
		m_model = model;
		Q_EMIT modelChanged(m_model);
	}
}


void CDataModelController::SetIsLoading(bool isLoading)
{
	if (m_isLoading != isLoading){
		m_isLoading = isLoading;
		Q_EMIT isLoadingChanged(m_isLoading);
	}
}


void CDataModelController::SetError(const QString& error)
{
	if (m_error != error){
		m_error = error;
		Q_EMIT errorChanged(m_error);
	}
}


// private methods

void CDataModelController::ApplySourceModel(const QVariant& model)
{
	UpdateCachedModel(model);

	if (m_viewModelPtr != nullptr && model.canConvert<QVariantMap>()){
		m_viewModelPtr->SetSourceValues(model.toMap());
	}
}


void CDataModelController::UpdateSubscription()
{
	IDataModelBridge* bridge = CDataModelBridgeDemultiplexer::Instance();
	if (bridge == nullptr){
		return;
	}

	if (m_subscriptionId != 0){
		bridge->UnsubscribeModel(m_subscriptionId);
		m_subscriptionId = 0;
	}

	if (!m_isLive || m_modelId.isEmpty()){
		return;
	}

	QPointer<CDataModelController> self(this);
	m_subscriptionId = bridge->SubscribeModel(m_modelId, m_parameters,
			[self](QVariant model){
				if (self.isNull()){
					return;
				}
				self->ApplySourceModel(model);
			});
}


void CDataModelController::OnViewModelEdited()
{
	if (m_isAutoSubmit){
		submit();
	}
}


} // namespace imtqml
