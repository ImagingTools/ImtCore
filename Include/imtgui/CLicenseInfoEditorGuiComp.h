#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/ILicenseInfo.h>
#include <GeneratedFiles/imtgui/ui_CLicenseInfoEditorGuiComp.h>


namespace imtgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CLicenseInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CLicenseInfoEditorGuiComp, imtbase::ILicenseInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CLicenseInfoEditorGuiComp, imtbase::ILicenseInfo> BaseClass;

	I_BEGIN_COMPONENT(CLicenseInfoEditorGuiComp);
	I_END_COMPONENT;

	CLicenseInfoEditorGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
	void on_PackageCombo_currentTextChanged(const QString &text);
	void on_ExpiredDate_dateTimeChanged(const QDateTime &datetime);
	void on_ExpireGroup_toggled(bool on);
};


} // namespace imtgui


