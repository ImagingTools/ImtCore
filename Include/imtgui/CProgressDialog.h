// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ACF includes
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CProgressDialog.h>


namespace imtgui
{


class CProgressDialog : public QDialog, public Ui_CProgressDialog
{
	Q_OBJECT
public:
	CProgressDialog(QWidget* parentPtr, const QString& text = QString(), bool showImmediately = false);
	~CProgressDialog();

public Q_SLOTS:
	void Show();
	void SetText(const QString& text);
	void Hide();


private:
	QMovie* m_progressAnimationMoviePtr;
};


} // namespace imtgui
