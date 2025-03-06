#pragma once


// Qt includes
#include <QtCore/QJsonDocument>

// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtqml/CQmlModelEditorCompBase.h>


namespace imtqml
{


template<typename ModelInterface>
class TQmlModelEditorCompBase:
			public CQmlModelEditorCompBase,
			public imod::TSingleModelObserverBase<ModelInterface>
{
public:
	typedef CQmlModelEditorCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<ModelInterface> BaseClass2;

	I_BEGIN_BASE_COMPONENT(TQmlModelEditorCompBase)
	I_END_COMPONENT;

	TQmlModelEditorCompBase();

protected:
	/**
		Create a JSON document for the data model object. The JSON document is a UI-related representation of the data model.
	*/
	virtual QJsonDocument CreateRepresentationFromObject(const ModelInterface& object) const = 0;

	/**
		Update the data model based on the UI-representation given as a JSON-document.
	*/
	virtual void UpdeteObjectFromRepresentation(const QJsonDocument& representation, ModelInterface& object) const = 0;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	class UpdateBlocker
	{
	public:
		explicit UpdateBlocker(const TQmlModelEditorCompBase<ModelInterface>* parentPtr);
		~UpdateBlocker();

	private:
		const TQmlModelEditorCompBase& m_parent;
	};

private:
	bool IsUpdateBlocked() const;

	// reimplemented (CQmlBasedEditorCompBase)
	virtual void OnRepresentationChanged() override;

	// reimplemented (CQuickObjectCompBase)
	virtual void OnItemCreated() override;

private:
	bool m_isUpdatePending = false;
	mutable int m_ignoreUpdatesCounter = 0;
};


// public methods

template<typename ModelInterface>
TQmlModelEditorCompBase<ModelInterface>::TQmlModelEditorCompBase()
{
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

template<typename ModelInterface>
inline void TQmlModelEditorCompBase<ModelInterface>::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (IsUpdateBlocked()){
		return;
	}

	UpdateBlocker blocker(this);

	ModelInterface* objectPtr = BaseClass2::GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	QQuickItem* quickItemPtr = BaseClass::GetQuickItem();
	if (quickItemPtr != nullptr){
		QJsonDocument jsonDoc = CreateRepresentationFromObject(*objectPtr);
		if (jsonDoc.isObject()){
			QString json = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));

			QMetaObject::invokeMethod(
				quickItemPtr, "setRepresentation", Q_ARG(QVariant, QVariant::fromValue(json)));
		}
	}
	else{
		m_isUpdatePending = true;
	}
}


// private methods

template<typename ModelInterface>
inline bool TQmlModelEditorCompBase<ModelInterface>::IsUpdateBlocked() const
{
	return (m_ignoreUpdatesCounter > 0);
}


// reimplemented (CQmlBasedEditorCompBase)

template<typename ModelInterface>
inline void TQmlModelEditorCompBase<ModelInterface>::OnRepresentationChanged()
{
	if (IsUpdateBlocked()){
		return;
	}

	UpdateBlocker blocker(this);

	ModelInterface* objectPtr = BaseClass2::GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	QQuickItem* quickItemPtr = BaseClass::GetQuickItem();
	if (quickItemPtr != nullptr){
		QVariant var;
		if (QMetaObject::invokeMethod(
			quickItemPtr, "getRepresentation", Qt::DirectConnection, Q_RETURN_ARG(QVariant, var)))
		{
			if (var.typeId() == QMetaType::QString){
				QString representation = var.toString();

				QJsonDocument document = QJsonDocument::fromJson(representation.toUtf8());

				UpdeteObjectFromRepresentation(document, *objectPtr);
			}
		}
	}
}


// private methods

// reimplemented (CQuickObjectCompBase)

template<typename ModelInterface>
inline void TQmlModelEditorCompBase<ModelInterface>::OnItemCreated()
{
	BaseClass::OnItemCreated();

	if (m_isUpdatePending){
		m_isUpdatePending = false;

		OnUpdate(istd::IChangeable::GetAnyChange());
	}
}


// public methods of embedded class UpdateBlocker

template<typename ModelInterface>
TQmlModelEditorCompBase<ModelInterface>::UpdateBlocker::UpdateBlocker(const TQmlModelEditorCompBase<ModelInterface>* parentPtr)
	:m_parent(*parentPtr)
{
	++m_parent.m_ignoreUpdatesCounter;
}


template<typename ModelInterface>
TQmlModelEditorCompBase<ModelInterface>::UpdateBlocker::~UpdateBlocker()
{
	--m_parent.m_ignoreUpdatesCounter;
}


} // namespace imtqml


