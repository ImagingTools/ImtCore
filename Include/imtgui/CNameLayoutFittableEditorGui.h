#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

// ACF includes
#include <istd/TRange.h>
#include <istd/TPointerVector.h>
#include <imod/TModelWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iprm/INameParam.h>

#include <imtgui/ILayoutFittableModelEditor.h>


namespace imtgui
{


class CNameLayoutFittableEditorGui:
			public QObject,
			public ILayoutFittableModelEditor,
			public imod::TSingleModelObserverBase<iprm::INameParam>
{
	Q_OBJECT

public:
	typedef imod::TSingleModelObserverBase<iprm::INameParam> BaseClass;

	CNameLayoutFittableEditorGui(QObject* parentPtr = nullptr);

	// reimplemented (ILayoutFittableWidget)
	bool PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex = -1, int columnIndex = 1) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;

	// reimplemented (imod::IModelEditor)
	virtual void UpdateEditor(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateModelFromEditor() const override;
	virtual bool IsReadOnly() const override;
	virtual void SetReadOnly(bool state) override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private Q_SLOTS:
	void OnObjectNameChanged(const QString newObjectName);
	void OnInputValueChanged();

private:
	QPointer<QLineEdit> m_nameLineEditPtr;
};


} // namespace imtgui



