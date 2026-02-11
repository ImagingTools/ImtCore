// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iqtgui/IDialog.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>


namespace imtgui
{


template<typename Base>
class TSpecificationBasedDocumentEditorCompWrap : public Base
{
public:
	typedef Base BaseClass;

	I_BEGIN_BASE_COMPONENT(TSpecificationBasedDocumentEditorCompWrap)
		I_ASSIGN(m_specObjectTypeIdAttrPtr, "SpecificationObjectTypeId", "Type ID of the specification object", false, "Specification");
		I_ASSIGN(m_specCollectionCompPtr, "SpecificationCollection", "Collection of the printer specification", false, "SpecCollection");
		I_ASSIGN(m_specDialogCompPtr, "SpecificationEditorDialog", "Specification editor dialog", false, "SpecEditorDialog");
		I_ASSIGN(m_specObserverCompPtr, "SpecificationObserver", "Specification editor observer", false, "SpecObserver");
		I_ASSIGN(m_collectionFilterFactPtr, "CollectionFilter", "Filter factory for the specification collection", false, "CollectionFilter");
		I_ASSIGN(m_specFactPtr, "SpecificationFactory", "Specification factory", false, "SpecificationFactory");
	I_END_COMPONENT;

	TSpecificationBasedDocumentEditorCompWrap();

protected:
	struct SpecificationInfo
	{
		QByteArray id;
		QString name;
	};

	typedef QList<SpecificationInfo> SpecificationList;

protected:
	/**
		The method creates a new specification, opens an editor for it,
		and returns the object identifier after insertion into the collection
	*/
	QByteArray CreateNewSpecification();
	/**
		The final editor implementation may modify the specification filter.
		The editor must not remove parameters from the params set
	*/
	virtual bool PrepareSpecificationCollectionFilter(iprm::IParamsSet& filter) = 0;
	/**
		The method provides a list of specifications for the final editor implementation
	*/
	virtual void OnSpecificationListChanged(const SpecificationList& specList) = 0;
	/**
		The final editor implementation must set the name of the object
		in the collection for the newly created specification
	*/
	virtual QString CreateSpecificationName(const istd::IChangeable* specPtr) = 0;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

private:
	void OnSpecificationCollectionChanged(
		const istd::IChangeable::ChangeSet& changeset, const imtbase::IObjectCollection* collectionPtr);

protected:
	I_ATTR(QByteArray, m_specObjectTypeIdAttrPtr);
	I_REF(imtbase::IObjectCollection, m_specCollectionCompPtr);
	I_REF(iqtgui::IDialog, m_specDialogCompPtr);
	I_REF(imod::IObserver, m_specObserverCompPtr);
	I_FACT(iprm::IParamsSet, m_collectionFilterFactPtr);
	I_FACT(istd::IChangeable, m_specFactPtr);

private:
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, TSpecificationBasedDocumentEditorCompWrap> m_collectionObserver;
};


// public methods

template<typename BaseClass>
inline TSpecificationBasedDocumentEditorCompWrap<BaseClass>::TSpecificationBasedDocumentEditorCompWrap()
	: m_collectionObserver(*this)
{
}


// protected methods

template<typename BaseClass>
inline QByteArray TSpecificationBasedDocumentEditorCompWrap<BaseClass>::CreateNewSpecification()
{
	QByteArray retVal;

	if ((*m_specObjectTypeIdAttrPtr).isEmpty() || !m_specCollectionCompPtr.IsValid() || !m_specFactPtr.IsValid() ||
		!m_specDialogCompPtr.IsValid() || !m_specObserverCompPtr.IsValid())
	{
		return retVal;
	}

	istd::TUniqueInterfacePtr<istd::IChangeable> specPtr(m_specFactPtr.CreateInstance());
	imod::IModel* specModelPtr = dynamic_cast<imod::IModel*>(specPtr.GetPtr());
	if (specModelPtr != nullptr){
		if (specModelPtr->AttachObserver(m_specObserverCompPtr.GetPtr())){
			if (m_specDialogCompPtr->ExecuteDialog(this) == QDialog::Accepted){
				QString name = CreateSpecificationName(specPtr.GetPtr());
				if (!name.isEmpty()){
					retVal =
						m_specCollectionCompPtr->InsertNewObject(*m_specObjectTypeIdAttrPtr, name, "", specPtr.GetPtr());
				}
			}

			specModelPtr->DetachObserver(m_specObserverCompPtr.GetPtr());
		}
	}

	return retVal;
}


// reimplemented (imod::IObserver)

template<typename BaseClass>
inline bool TSpecificationBasedDocumentEditorCompWrap<BaseClass>::OnModelAttached(
	imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool retVal = BaseClass::OnModelAttached(modelPtr, changeMask);

	if (retVal && m_specCollectionCompPtr.IsValid()){
		m_collectionObserver.RegisterObject(
			m_specCollectionCompPtr.GetPtr(), &TSpecificationBasedDocumentEditorCompWrap::OnSpecificationCollectionChanged);
	}

	return retVal;
}


template<typename BaseClass>
inline bool TSpecificationBasedDocumentEditorCompWrap<BaseClass>::OnModelDetached(imod::IModel* modelPtr)
{
	m_collectionObserver.UnregisterAllObjects();

	return BaseClass::OnModelDetached(modelPtr);
}


// private methods

template<typename BaseClass>
inline void TSpecificationBasedDocumentEditorCompWrap<BaseClass>::OnSpecificationCollectionChanged(
	const istd::IChangeable::ChangeSet& changeset, const imtbase::IObjectCollection* collectionPtr)
{
	Q_ASSERT(collectionPtr != nullptr);

	if (m_collectionFilterFactPtr.IsValid()){
		istd::TUniqueInterfacePtr<iprm::IParamsSet> filterPtr(m_collectionFilterFactPtr.CreateInstance());
		Q_ASSERT(filterPtr.IsValid());

		if (PrepareSpecificationCollectionFilter(*filterPtr)){
			QByteArrayList ids = collectionPtr->GetElementIds(0, -1, filterPtr.GetPtr());

			SpecificationList specList;

			for (const QByteArray& id : ids){
				QString name = collectionPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();

				specList.append({id, name});
			}

			OnSpecificationListChanged(specList);
		}
	}
}


} // namespace imtgui


