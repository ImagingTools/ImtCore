#pragma once


// Qt includes
#include <QtCore/QObject>

namespace imtloggui
{


class CRepresentationControllerWorkerBase: public QObject
{
	Q_OBJECT
public:
	CRepresentationControllerWorkerBase();

Q_SIGNALS:
	void EmitNewJobAdded();
	void EmitResultReady();
	
protected Q_SLOTS:
	virtual void OnNewJobAdded() = 0;
	virtual void OnResultReady() = 0;
};


} // namespace imtloggui


