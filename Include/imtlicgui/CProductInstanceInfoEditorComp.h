#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInstanceInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CProductInstanceInfoEditorComp.h>


namespace imtlicgui
{


class DateTimeDelegate;


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductInstanceInfoEditorComp, imtlic::IProductInstanceInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductInstanceInfoEditorComp, imtlic::IProductInstanceInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoEditorComp);
	I_END_COMPONENT;

	CProductInstanceInfoEditorComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_ProductInstanceIdEdit_editingFinished();
	void on_ProductCombo_currentIndexChanged(int index);
	void on_LicenseInstancesEdit_itemChanged(QTreeWidgetItem *item, int column);
	void on_CustomerCombo_currentIndexChanged(int index);

private:
	void OnProductsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* productCollectionPtr);
	void OnLicensesUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* productCollectionPtr);
	void OnCustomersUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* productCollectionPtr);
	void UpdateProductsCombo();
	void UpdateLicenseInstancesEdit();

private:
	class DateTimeDelegate: public QStyledItemDelegate
	{
	public:
		typedef QStyledItemDelegate BaseClass;

		DateTimeDelegate(QObject *parent = nullptr);

		// reimplemented (QItemDelegate)
		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	};

private:
	bool m_itemUpdateBlocked;
	QMap<QByteArray, QDateTime> m_cachedLicenseExpiration;

	DateTimeDelegate m_dateDelegate;

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductInstanceInfoEditorComp> m_productCollectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductInstanceInfoEditorComp> m_licenseCollectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductInstanceInfoEditorComp> m_customerCollectionObserver;
};


} // namespace imtlicgui


