#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <icomp/CComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtgui
{


class CMenuPanelComp:
            public ibase::TModelObserverCompBaseWrap<
                        iqtgui::TGuiObserverWrap<imtwidgets::CMenuPanel, imod::TSingleModelObserverBase<iprm::ISelectionParam>>>
{
public:
    typedef ibase::TModelObserverCompBaseWrap<iqtgui::TGuiObserverWrap<imtwidgets::CMenuPanel, imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;

    I_BEGIN_COMPONENT(CMenuPanelComp);
    I_END_COMPONENT;

    CMenuPanelComp();

protected:
    // reimplemented (iqtgui::TGuiObserverWrap)
    virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
    virtual void OnGuiModelAttached() override;

    // reimplemented (iqtgui::CGuiComponentBase)
    virtual void OnGuiCreated() override;
    virtual void OnGuiDestroyed() override;
    virtual void OnGuiRetranslate() override;

    // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated() override;
    virtual void OnComponentDestroyed() override;

private:

};


} // namespace imtgui


