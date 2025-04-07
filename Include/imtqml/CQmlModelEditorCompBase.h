#pragma once


// ImtCore includes
#include <imtqml/CQmlGuiCompBase.h>


namespace imtqml
{


class CQmlModelEditorCompBase: public QObject, public CQmlGuiCompBase
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


