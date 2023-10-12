#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/ILicenseDefinition.h>
#include <GeneratedFiles/imtlicgui/ui_CLicenseInfoEditorGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CLicenseInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CLicenseInfoEditorGuiComp, imtlic::ILicenseDefinition>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CLicenseInfoEditorGuiComp, imtlic::ILicenseDefinition> BaseClass;

	I_BEGIN_COMPONENT(CLicenseInfoEditorGuiComp);
	I_END_COMPONENT;

Q_SIGNALS:
	void EmitItemChanged();

protected:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
};


} // namespace imtlicgui


