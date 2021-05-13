#include <imtlicgui/CItem.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtlicgui
{


// public methods

CItem::CItem()
	: m_status(S_VALID),
	m_isActivationEnabled(false),
	m_isActivated(false),
	m_isEnabled(true),
	m_itemChangeHandlerPtr(nullptr)
{
}


void CItem::SetItemChangeHandler(IItemChangeHandler* itemChangeHandlerPtr)
{
	m_itemChangeHandlerPtr = itemChangeHandlerPtr;
}


// reimplemented (IItem)

QByteArray CItem::GetId() const
{
	return m_id;
}


void CItem::SetId(const QByteArray& id)
{

	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		QByteArray oldId = m_id;
		m_id = id;
		
		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(id);
			params.append(oldId);
			itemChangeHandler->OnItemChanged(oldId, IItemChangeHandler::CI_ITEM_ID, params);
		}
	}
}


QString CItem::GetName() const
{
	return m_name;
}


void CItem::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);

		QString oldName = m_name;
		m_name = name;

		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(name);
			params.append(oldName);
			itemChangeHandler->OnItemChanged(m_id, IItemChangeHandler::CI_ITEM_NAME, params);
		}
	}
}


IItem::Status CItem::GetStatus() const
{
	return m_status;
}


void CItem::GetStatus(Status status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);

		Status oldStatus = m_status;
		m_status = status;

		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(status);
			params.append(oldStatus);
			itemChangeHandler->OnItemChanged(m_id, IItemChangeHandler::CI_ITEM_STATUS, params);
		}
	}
}


bool CItem::IsActivationEnabled() const
{
	return m_isActivationEnabled;
}


void CItem::SetActivationEnabled(bool isActivationEnabled)
{
	if (m_isActivationEnabled != isActivationEnabled){
		istd::CChangeNotifier notifier(this);

		bool oldIsActivationEnabled = m_isActivationEnabled;
		m_isActivationEnabled = isActivationEnabled;

		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(isActivationEnabled);
			params.append(oldIsActivationEnabled);
			itemChangeHandler->OnItemChanged(m_id, IItemChangeHandler::CI_ITEM_ACTIVATION_ENABLED, params);
		}
	}
}


bool CItem::IsActivated() const
{
	return m_isActivated;
}


void CItem::SetActivated(bool isActivated)
{
	if (m_isActivated != isActivated){
		istd::CChangeNotifier notifier(this);

		bool oldIsActivated = m_isActivated;
		m_isActivated = isActivated;

		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(isActivated);
			params.append(oldIsActivated);
			itemChangeHandler->OnItemChanged(m_id, IItemChangeHandler::CI_ITEM_ACTIVATED, params);
		}
	}
}


bool CItem::IsEnabled() const
{
	return m_isEnabled;
}


void CItem::SetEnabled(bool isEnabled)
{
	if (m_isEnabled != isEnabled){
		istd::CChangeNotifier notifier(this);

		bool oldIsEnabled = m_isEnabled;
		m_isEnabled = isEnabled;

		IItemChangeHandler* itemChangeHandler = FindItemChangeHandler();
		if (itemChangeHandler != nullptr){
			QVariantList params;
			params.append(isEnabled);
			params.append(oldIsEnabled);
			itemChangeHandler->OnItemChanged(m_id, IItemChangeHandler::CI_ITEM_ENABLED, params);
		}
	}
}


void CItem::AddChild(ItemTreePtr childItemPtr)
{
	istd::CChangeNotifier notifier(this);

	m_childs.append(childItemPtr);
}


IItemChangeHandler* CItem::FindItemChangeHandler() const
{
	if (m_itemChangeHandlerPtr != nullptr){
		return m_itemChangeHandlerPtr;
	}

	const IItem* parentItemPtr = GetParent();
	if (parentItemPtr != nullptr){
		return parentItemPtr->FindItemChangeHandler();
	}

	return nullptr;
}


// reimplemented (ilog::IHierarchicalMessageContainer)

int CItem::GetChildsCount() const
{
	return m_childs.count();
}


IItemTree* CItem::GetChild(int index) const
{
	if (index < m_childs.count()){
		return const_cast<IItemTree*>(m_childs[index].GetPtr());
	}

	return nullptr;
}


// reimplemented (istd::IChangeable)

int CItem::GetSupportedOperations() const
{
	return SO_COMPARE | SO_RESET;
}


bool CItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CItem* otherObjectPtr = dynamic_cast<const CItem*>(&object);
	if (otherObjectPtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_id = otherObjectPtr->m_id;
		m_name = otherObjectPtr->m_name;
		m_status = otherObjectPtr->m_status;
		m_isActivationEnabled = otherObjectPtr->m_isActivationEnabled;
		m_isActivated = otherObjectPtr->m_isActivated;
		m_isEnabled = otherObjectPtr->m_isEnabled;

		m_itemChangeHandlerPtr = otherObjectPtr->m_itemChangeHandlerPtr;

		m_childs.clear();

		for (int i = 0; i < otherObjectPtr->m_childs.count(); i++){
			ItemTreePtr itemPtr(new CItem());
			if (!itemPtr->CopyFrom(*otherObjectPtr->m_childs[i].GetPtr())){
				return false;
			}

			m_childs.append(itemPtr);
		}

		return true;
	}

	return false;
}


bool CItem::IsEqual(const istd::IChangeable& object) const
{
	const CItem* otherObjectPtr = dynamic_cast<const CItem*>(&object);
	if (otherObjectPtr != nullptr){
		bool retVal = true;

		retVal = retVal && (m_id == otherObjectPtr->m_id);
		retVal = retVal && (m_name == otherObjectPtr->m_name);
		retVal = retVal && (m_status == otherObjectPtr->m_status);
		retVal = retVal && (m_isActivationEnabled == otherObjectPtr->m_isActivationEnabled);
		retVal = retVal && (m_isActivated == otherObjectPtr->m_isActivated);
		retVal = retVal && (m_isEnabled == otherObjectPtr->m_isEnabled);

		return retVal;
	}

	return false;
}


istd::IChangeable* CItem::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CItem> clonePtr(new CItem);

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CItem::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_name.clear();
	m_status = S_VALID;
	m_isActivationEnabled = false;
	m_isActivated = false;
	m_isEnabled = true;

	m_childs.clear();
	m_itemChangeHandlerPtr = nullptr;

	return true;
}


} // namespace imtlicgui


