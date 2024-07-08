#include <imtgui/CCommandGroupComp.h>


namespace imtgui
{


// public methods

// reimplemented (imtgui::ICommandGroup)

int CCommandGroupComp::GetAlignment() const
{
	if (m_alignmentAttrPtr.IsValid()){
		return *m_alignmentAttrPtr;
	}

	return 1;
}


} // namespace imtgui


