// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDataModelController.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtqml
{


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
	qWarning() << "CDataModelController::getModel() should be implemented in a subclass"
			   << "(modelId=" << m_modelId << ")";
	Q_EMIT modelReceived(m_model);
}


void CDataModelController::setModel(const QVariant& model)
{
	Q_EMIT startSetModel(model);
	qWarning() << "CDataModelController::setModel() should be implemented in a subclass"
			   << "(modelId=" << m_modelId << ")";
	UpdateCachedModel(model);
	Q_EMIT modelSet();
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
