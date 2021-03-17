#include <imtlicgui/CItem.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtlicgui
{


// public methods

CItem::CItem()
{
	m_status = S_VALID;
	m_isActivated = false;
	m_isEnabled = true;
}


void CItem::AddChild(ItemTreePtr childItemPtr)
{
	istd::CChangeNotifier notifier(this);

	m_childs.append(childItemPtr);
}


// reimplemented (IItemTree)

QByteArray CItem::GetId() const
{
	return m_id;
}


void CItem::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
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

		m_name = name;
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

		m_status = status;
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

		m_isActivated = isActivated;
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

		m_isEnabled = isEnabled;
	}
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


bool CItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	const CItem* otherObjectPtr = dynamic_cast<const CItem*>(&object);
	if (otherObjectPtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_id = otherObjectPtr->m_id;
		m_name = otherObjectPtr->m_name;
		m_status = otherObjectPtr->m_status;
		m_isActivated = otherObjectPtr->m_isActivated;
		m_isEnabled = otherObjectPtr->m_isEnabled;

		m_childs.clear();

		for (int i = 0; i < otherObjectPtr->m_childs.count(); i++){
			ItemTreePtr itemPtr(new CItem());
			itemPtr->CopyFrom(*otherObjectPtr->m_childs[i].GetPtr());
			m_childs.append(itemPtr);
		}

		return true;
	}

	return false;
}


bool CItem::IsEqual(const istd::IChangeable& object) const
{
	const CItem* otherObjectPtr = dynamic_cast<const CItem*>(&object);
	if (otherObjectPtr != NULL){
		bool retVal = true;

		retVal = retVal && (m_id == otherObjectPtr->m_id);
		retVal = retVal && (m_name == otherObjectPtr->m_name);
		retVal = retVal && (m_status == otherObjectPtr->m_status);
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
	m_id.clear();
	m_name.clear();
	m_status = S_VALID;
	m_isActivated = false;
	m_isEnabled = true;

	m_childs.clear();

	return true;
}


} // namespace imtlicgui


