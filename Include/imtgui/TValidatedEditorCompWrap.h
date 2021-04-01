#pragma once

#include <icomp/CComponentBase.h>

// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <ilog/IMessageConsumer.h>

namespace imtgui
{

template <class Editor>
class TValidatedEditorCompWrap: public Editor, virtual public ilog::IMessageConsumer
{
public:
	typedef Editor BaseClass;

	I_BEGIN_COMPONENT(Editor);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN_MULTI_0(m_validationMessageIdAttrPtr, "ValidationMessageIds", "Validation message ids", false);
		I_ASSIGN_MULTI_0(m_validationMessageWidgetPropertyAttrPtr, "ValidationMessageWidget«roperties", "Properties for identifying the validation message widgets", false);
	I_END_COMPONENT;

protected:
	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;


	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(int, m_validationMessageIdAttrPtr);
	I_MULTIATTR(QByteArray, m_validationMessageWidgetPropertyAttrPtr);

	QMap<int, QByteArray> m_propertyMap;
};


template <class Editor>
bool TValidatedEditorCompWrap<Editor>::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	return true;
}


template <class Editor>
void TValidatedEditorCompWrap<Editor>::AddMessage(const MessagePtr& messagePtr)
{
	QWidget* widget = GetQtWidget();
	QWidget* validationMessageWidgetPtr = nullptr;

	QByteArray property = m_propertyMap[messagePtr->GetInformationId()];

	QList<QWidget*> widgetList = widget->findChildren<QWidget*>();
	for (const QWidget* widgetPtr : widgetList){
		if (widgetPtr->property(property).isValid()){
			switch (messagePtr->GetInformationCategory()){
			case istd::IInformationProvider::IC_ERROR:
			case istd::IInformationProvider::IC_CRITICAL:
				widgetPtr->setProperty("text", messagePtr->GetInformationDescription());
				widgetPtr->setStyleSheet("color : red;");
				widgetPtr->show();
				break;
			case istd::IInformationProvider::IC_WARNING:
				widgetPtr->setProperty("text", messagePtr->GetInformationDescription());
				widgetPtr->setStyleSheet("color : yellow;");
				widgetPtr->show();
				break;
			default:
				widgetPtr->hide();
				break;
			}

			break;
		}
	}
}


// reimplemented (icomp::CComponentBase)

template <class Editor>
void TValidatedEditorCompWrap<Editor>::OnComponentCreated()
{
	int count = m_validationMessageIdAttrPtr.GetCount();
	count = qMin(count, m_validationMessageWidgetPropertyAttrPtr.GetCount());

	for (int i = 0; i < count; i++){
		m_propertyMap[m_validationMessageIdAttrPtr[i]] = m_validationMessageWidgetPropertyAttrPtr[i];
	}
}


} // namespace imtgui


