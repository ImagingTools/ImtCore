#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/IDialog.h>

// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtgui
{


/**
	\ingroup Collection
*/
class CStandardCollectionViewDelegateComp:
			public CObjectCollectionViewDelegate,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CObjectCollectionViewDelegate BaseClass2;

	I_BEGIN_COMPONENT(CStandardCollectionViewDelegateComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(imtgui::ICollectionViewDelegate);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID supported by this delegate", true, "ObjectTypeId");
		I_ASSIGN(m_statusIconsProviderCompPtr, "StatusIcons", "Icons for delegate visual status", false, "StatusIcons");
		I_ASSIGN(m_editDialogCompPtr, "EditDialog", "Dialog used for editing the collection items", false, "EditDialog");
		I_ASSIGN_TO(m_editDialogObserverCompPtr, m_editDialogCompPtr, true);
	I_END_COMPONENT;

	CStandardCollectionViewDelegateComp();

	// reimplemented (ICollectionViewDelegate)
	virtual QByteArray GetSupportedTypeId() const override;
	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

protected Q_SLOTS:
	virtual void OnEdit();

protected:
	iqtgui::CHierarchicalCommand m_editContentsCommand;

private:
	void InitializeVisualStatus();

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_REF(iqtgui::IIconProvider, m_statusIconsProviderCompPtr);

	I_REF(iqtgui::IDialog, m_editDialogCompPtr);
	I_REF(imod::IObserver, m_editDialogObserverCompPtr);

private:
	ifile::IFileMetaInfoProvider::MetaInfoPtr m_selectedMetaInfoPtr;
};


} // namespace imtgui


