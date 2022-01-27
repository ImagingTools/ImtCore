#pragma once


//Qt includes


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>
#include <imtbase/CTreeItemModel.h>

#include <QTimer>


namespace imtqml
{


class CObserverQmlComp: public QObject, public ilog::CLoggerComponentBase
{
    Q_OBJECT
public:
		typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObserverQmlComp);
        I_ASSIGN(m_quickObjectComp, "QuickObject", "Main QML Component", true, "QuickObject");
	I_END_COMPONENT;

// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:

private Q_SLOTS:
    void OnChangeSourceItem(QString src);

private:

    I_REF(imtqml::IQuickObject, m_quickObjectComp);
};


} // namespace imtqml
