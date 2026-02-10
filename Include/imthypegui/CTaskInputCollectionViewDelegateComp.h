// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ISummaryRepresentationController.h>
#include <imtgui/CStandardCollectionViewDelegateComp.h>


namespace imthypegui
{


class CTaskInputCollectionViewDelegateComp: public imtgui::CStandardCollectionViewDelegateComp
{
	Q_OBJECT
public:
	typedef imtgui::CStandardCollectionViewDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CTaskInputCollectionViewDelegateComp);
		I_ASSIGN(m_previewColumnNameAttrPtr, "PreviewColumnName", "Preview column name", true, "Gain");
		I_ASSIGN(m_paramSetRepresentationControllerCompPtr, "GainRepresentationController", "Gain text based representation controller", false, "TextBasedParamSetRepresentationController");
	I_END_COMPONENT;

	// reimplemented (ICollectionViewDelegate)
	virtual bool GetSummaryInformation(
				const QByteArray& objectId,
				const QVector<QByteArray>& fieldIds,
				ObjectMetaInfo& objectMetaInfo) const override;
	virtual QAbstractItemDelegate* GetColumnItemDelegate(const QByteArray& columnId) const override;
	virtual void OnColumnItemDelegateEditFinished(const QByteArray& objectId, const QByteArray& columnId, const QVariant& newValue) const override;
	virtual bool IsEditorEnabled(const QByteArray& columnId) const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray CreateNewObject(
				const QByteArray& typeId,
				const QString& objectName,
				const QString& description,
				const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual QString RenameObject(const QByteArray& objectId, const QString& newName) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (imtbase::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;

private:
	I_ATTR(QString, m_previewColumnNameAttrPtr);
	I_REF(imtbase::ISummaryRepresentationController, m_paramSetRepresentationControllerCompPtr);
};


} // namespace imthypegui


