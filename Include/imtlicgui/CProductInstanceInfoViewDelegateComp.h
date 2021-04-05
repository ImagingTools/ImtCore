#pragma once


// ImtCore includes
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


namespace imtlicgui
{


class CProductInstanceInfoViewDelegateComp:
			public imtrepogui::CDocumentBasedFileCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewDelegateComp);
	I_END_COMPONENT;

	CProductInstanceInfoViewDelegateComp();

protected:
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
	virtual void SetupCommands() override;

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

protected Q_SLOTS:
	virtual void OnExportLicense();

private:
	iqtgui::CHierarchicalCommand m_exportLicenseCommand;
};


} // namespace imtlicgui


