// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>

// Acula includes
#include <imthype/ITaskCollection.h>
#include <GeneratedFiles/imthypegui/ui_CTaskSettingsDialog.h>


namespace imthypegui
{


class CTaskSettingsDialog: public QDialog, public Ui::CTaskSettingsDialog
{
	Q_OBJECT
public:
	typedef QDialog BaseClass;

	CTaskSettingsDialog(QWidget* parent);
	~CTaskSettingsDialog();

	void SetCurrentSelectedInputId(const QByteArray& currentSelectedInputId);
	void RegisterInputsCollection(const imtbase::IObjectCollection* inputsCollectionPtr);

private:
	void OnTaskInputsChanged(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::IObjectCollection* inputsCollectionPtr);

private:
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CTaskSettingsDialog> m_taskInputsObserver;
	QByteArray m_currentSelectedInputId;
};



} // namespace imthypegui


