/********************************************************************************
**
**	Copyright (C) 2007-2017 Witold Gantzke & Kirill Lepskiy
**
**	This file is part of the ACF Toolkit.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
** 	See http://www.ilena.org or write info@imagingtools.de for further
** 	information about the ACF.
**
********************************************************************************/


#ifndef imtgui_CLayoutManager_included
#define imtgui_CLayoutManager_included


// Qt includes
//#include <QtCore/QTimer>
//#include <QtWidgets/QAction>
#include <QtWidgets>

// ACF includes
//#include <ifile/IFilePersistence.h>
//#include <iprm/CEnableableParam.h>
//#include <ilog/CMessageContainer.h>
//#include <ilog/CMessage.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <GeneratedFiles/imtgui/ui_CLayoutManagerComp.h>


namespace imtgui
{


/**
    Layout editor.
*/
class CLayoutManagerComp:
	public iqtgui::TDesignerGuiCompBase<Ui::CLayoutManagerComp>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CLayoutManagerComp> BaseClass;


    I_BEGIN_COMPONENT(CLayoutManagerComp);
	I_END_COMPONENT;

    CLayoutManagerComp();

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	void UpdateVisualStatus();

private:

};

} // namespace iloggui


#endif // !imtgui_CLayoutManager_included
