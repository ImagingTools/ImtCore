#include <imtlicgui/CFeatureInfoEditorGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgui
{


// public methods

CFeatureInfoEditorGuiComp::CFeatureInfoEditorGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	IdEdit->setText(featureInfoPtr->GetFeatureId());
	NameEdit->setText(featureInfoPtr->GetFeatureName());
}


void CFeatureInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CFeatureInfoEditorGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CFeatureInfoEditorGuiComp::UpdateModel() const
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	imtlic::CFeatureInfo featureInfo;

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


void CFeatureInfoEditorGuiComp::on_PackageCombo_currentTextChanged(const QString& /*text*/)
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::on_ExpiredDate_dateTimeChanged(const QDateTime& /*datetime*/)
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::on_ExpireGroup_toggled(bool /*on*/)
{
	DoUpdateModel();
}


} // namespace imtlicgui


