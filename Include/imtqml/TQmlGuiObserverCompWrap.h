#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/TGuiObserverWrap.h>

// ImtCore includes
#include <imtqml/CQmlGuiCompBase.h>


namespace imtqml
{


template <class QmlAdapter, class ModelInterface>
class TQmlGuiObserverCompWrap: public iqtgui::TGuiObserverWrap<
	CQmlGuiCompBase,
	imod::TSingleModelObserverBase<ModelInterface>>
{
public:
	typedef iqtgui::TGuiObserverWrap<
		CQmlGuiCompBase,
		imod::TSingleModelObserverBase<ModelInterface>> BaseClass;

	I_BEGIN_BASE_COMPONENT(TQmlGuiObserverCompWrap)
		I_REGISTER_INTERFACE(imod::IObserver)
	I_END_COMPONENT;

	// reimplemented (iqtgui::IGuiObject)
	virtual bool CreateGui(QWidget* parentPtr) override;
	virtual bool DestroyGui() override;

protected:
	virtual void OnQmlAdapterInitialized();

protected:
	struct QObjectDeleter
	{
		Q_ALWAYS_INLINE void operator()(QObject* objectPtr) const
		{
			if (objectPtr != nullptr){
				objectPtr->deleteLater();
			}
		}
	};

protected:
	std::unique_ptr<QmlAdapter, QObjectDeleter> m_qmlAdapterPtr;

private:
	// reimplemented (CQmlGuiCompBase)
	void OnQuickItemDestroyed() override;
};


// reimplemented (iqtgui::IGuiObject)

template <class QmlAdapter, class ModelInterface>
bool TQmlGuiObserverCompWrap<QmlAdapter, ModelInterface>::CreateGui(QWidget* parentPtr)
{
	bool retVal = BaseClass::CreateGui(parentPtr);

	if (retVal){
		Q_ASSERT(m_qmlAdapterPtr == nullptr);

		if (BaseClass::m_quickWidget == nullptr){
			qCritical() << "QuickObject is NOT VALID!";

			return retVal;
		}

		QQuickItem* quickItemPtr = BaseClass::m_quickWidget->rootObject();
		if (quickItemPtr == nullptr){
			qCritical() << "QuickObject hasn't a valid item!";

			return retVal;
		}

		m_qmlAdapterPtr.reset(new QmlAdapter(this, quickItemPtr));

		OnQmlAdapterInitialized();
	}

	return retVal;
}


template <class QmlAdapter, class ModelInterface>
bool TQmlGuiObserverCompWrap<QmlAdapter, ModelInterface>::DestroyGui()
{
	Q_ASSERT(m_qmlAdapterPtr != nullptr);

	m_qmlAdapterPtr.release()->deleteLater();

	return BaseClass::DestroyGui();
}


// protected methods

template <class QmlAdapter, class ModelInterface>
void TQmlGuiObserverCompWrap<QmlAdapter, ModelInterface>::OnQmlAdapterInitialized()
{
}


// private methods

// reimplemented (CQmlGuiCompBase)

template <class QmlAdapter, class ModelInterface>
void TQmlGuiObserverCompWrap<QmlAdapter, ModelInterface>::OnQuickItemDestroyed()
{
	Q_ASSERT(BaseClass::m_quickWidget != nullptr);
	BaseClass::m_quickWidget->setSource(QUrl(*BaseClass::m_pathToQmlAttrPtr));

	QQuickItem* quickItemPtr = BaseClass::m_quickWidget->rootObject();
	if (m_qmlAdapterPtr != nullptr && quickItemPtr != nullptr){
		BaseClass::connect(quickItemPtr, &QQuickItem::destroyed, this, &BaseClass::OnQuickItemDestroyed);

		m_qmlAdapterPtr->AcquireQuickItem(quickItemPtr, true);
	}
}


} // namespace imtqml



