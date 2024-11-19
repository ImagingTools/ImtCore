#include <imtrest/CGuiElementModelComp.h>


namespace imtrest
{


// public methods

// reimplemented (imtrest::IGuiElementContainer)

QByteArray CGuiElementModelComp::GetElementId() const
{
	if (m_elementIdAttrPtr.IsValid()){
		return *m_elementIdAttrPtr;
	}

	return QByteArray();
}


QString CGuiElementModelComp::GetElementName() const
{
	if (m_elementNameAttrPtr.IsValid()){
		return *m_elementNameAttrPtr;
	}

	return QString();
}


QString CGuiElementModelComp::GetElementDescription() const
{
	if (m_elementDescriptionAttrPtr.IsValid()){
		return *m_elementDescriptionAttrPtr;
	}

	return QString();
}


QString CGuiElementModelComp::GetElementItemPath() const
{
	if (m_elementItemPathAttrPtr.IsValid()){
		return (*m_elementItemPathAttrPtr);
	}

	return QString();
}


QString CGuiElementModelComp::GetElementStatus() const
{
	if (m_elementStatusAttrPtr.IsValid()){
		return *m_elementStatusAttrPtr;
	}

	return QString();
}


bool CGuiElementModelComp::IsEnabled() const
{
	if (m_enabledAttrPtr.IsValid()){
		return *m_enabledAttrPtr;
	}

	return false;
}


bool CGuiElementModelComp::IsVisible() const
{
	if (m_visibleAttrPtr.IsValid()){
		return *m_visibleAttrPtr;
	}

	return true;
}


int CGuiElementModelComp::GetPriority() const
{
	if (m_priorityAttrPtr.IsValid()){
		return *m_priorityAttrPtr;
	}

	return 0;
}


int CGuiElementModelComp::GetAlignment() const
{
	if (m_alignmentAttrPtr.IsValid()){
		return *m_alignmentAttrPtr;
	}

	return 1;
}


const imtrest::IGuiElementContainer* CGuiElementModelComp::GetSubElements() const
{
	return m_subElementsCompPtr.GetPtr();
}


} // namespace imtrest


