#include <imtlicgui/CFeatureInfoEditorGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeatureDependenciesManager.h>

namespace imtlicgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	IdEdit->setText(featureInfoPtr->GetFeatureId());
	NameEdit->setText(featureInfoPtr->GetFeatureName());
}


void CFeatureInfoEditorGuiComp::UpdateModel() const
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	imtlic::CFeatureInfo featureInfo;

	if (IdEdit->text().isEmpty()){
		QString id = NameEdit->text();

		id.replace(" ", "");
		id.prepend("#");

		IdEdit->setText(id);
	}

	featureInfo.SetFeatureId(IdEdit->text().toUtf8());
	featureInfo.SetFeatureName(NameEdit->text());

	featureInfoPtr->CopyFrom(featureInfo);
}


// private slots

void CFeatureInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtlicgui


