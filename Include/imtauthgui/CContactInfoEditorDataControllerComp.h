#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataController.h>
#include <imod/TModelWrap.h>


namespace imtauthgui
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CContactInfoEditorDataControllerComp:
		public icomp::CComponentBase,
		public imod::TSingleModelObserverBase<imtauth::IContactInfo>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtauth::IContactInfo> BaseClass2;

	I_BEGIN_COMPONENT(CContactInfoEditorDataControllerComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_ASSIGN(m_representationDataControllerCompPtr, "ItemBasedRepresentationDataControllerCompPtr", "reference to ItemBasedRepresentationDataController", true, "ItemBasedRepresentationDataController");
		I_REGISTER_SUBELEMENT(CItemBasedRepresentationDataProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(CItemBasedRepresentationDataProvider, imtbase::IItemBasedRepresentationDataProvider, ExtractCommands);
	I_END_COMPONENT;

	CContactInfoEditorDataControllerComp();

protected:
	class CItemBasedRepresentationDataProvider : public imtbase::IItemBasedRepresentationDataProvider
	{
	public:
//		imod::TModelWrap<imtbase::CTreeItemModel> m_treeItemModel;
		imtbase::CTreeItemModel m_treeItemModel;

		// reimplemented (imtbase::IItemBasedRepresentationProvider)
//		virtual const imtbase::ICollectionInfo& GetModelsList() const override;
		virtual imtbase::CTreeItemModel* GetTreeItemModel(const QByteArray& modelId) override;
	} m_itemBesedRepresentetionProvider;

	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractCommands(CContactInfoEditorDataControllerComp& component)
	{
		return &component.m_itemBesedRepresentetionProvider;
	}
	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask);

	// reimplimented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_REF(imtbase::IItemBasedRepresentationDataController, m_representationDataControllerCompPtr);

	class CTreeItemModelObserver : public imod::TSingleModelObserverBase<imod::IModel>
	{
	public:
		CTreeItemModelObserver(CContactInfoEditorDataControllerComp *parent);
		~CTreeItemModelObserver();
	protected:
		// reimplimented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CContactInfoEditorDataControllerComp *m_parent;
	} m_treeItemModelObserver;

	imtauth::IContactInfo* m_contactInfoPtr;

};


} // namespace imtauthgui


