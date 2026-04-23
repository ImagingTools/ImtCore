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
	:BaseClass(parent)
{
}


CDataModelController::~CDataModelController() = default;


const QString& CDataModelController::GetModelId() const
{
	return m_modelId;
}


void CDataModelController::SetModelId(const QString& modelId)
{
	if (m_modelId != modelId){
		m_modelId = modelId;
		Q_EMIT modelIdChanged(m_modelId);
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
					Q_EMIT self->getModelFailed(errorMessage);
					return;
				}
				self->UpdateCachedModel(model);
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
					Q_EMIT self->setModelFailed(errorMessage);
					return;
				}
				self->UpdateCachedModel(model);
				Q_EMIT self->modelSet();
			});
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


} // namespace imtqml
