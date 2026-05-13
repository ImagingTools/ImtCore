// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CProgressDialog.h>


// Qt includes
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtGui/QMovie>


namespace imtgui
{


// public methods

CProgressDialog::CProgressDialog(QWidget* parentPtr, const QString& text, bool showImmediately)
	:QDialog(parentPtr),
	m_progressAnimationMoviePtr(nullptr)
{
	setupUi(this);

	if (!text.isNull()){
		ProgressTextLabel->setText(text);
	}

	m_progressAnimationMoviePtr = new QMovie(":/AnimatedIcons/loading");
	m_progressAnimationMoviePtr->setScaledSize(QSize(25, 25));

	setWindowFlag(Qt::FramelessWindowHint);
	setModal(true);

	if (showImmediately){
		Show();
	}
}


CProgressDialog::~CProgressDialog()
{
	if (m_progressAnimationMoviePtr != nullptr){
		m_progressAnimationMoviePtr->stop();
		m_progressAnimationMoviePtr->deleteLater();
	}
}


void CProgressDialog::Show()
{
	if (m_progressAnimationMoviePtr != nullptr){
		ProgressAnimation->setAttribute(Qt::WA_NoSystemBackground);
		ProgressAnimation->setMovie(m_progressAnimationMoviePtr);
		m_progressAnimationMoviePtr->start();
	}

	QWidget* parentWidgetPtr = parentWidget();

	if (parentWidgetPtr != nullptr){

		QPoint centerOfParentWidget = parentWidgetPtr->mapToGlobal(parentWidgetPtr->rect().center());

		centerOfParentWidget.rx() -= this->width() / 2;
		centerOfParentWidget.ry() -= this->height() / 2;
		move(centerOfParentWidget);
	}

	show();

	QEventLoop showEventLoop;
	QTimer::singleShot(100, &showEventLoop, &QEventLoop::quit);
	showEventLoop.exec();
}


void CProgressDialog::SetText(const QString& text)
{
	ProgressTextLabel->setText(text);
}


void CProgressDialog::Hide()
{
	hide();

	if (m_progressAnimationMoviePtr != nullptr){
		m_progressAnimationMoviePtr->stop();
	}
}


} // namespace imtgui
