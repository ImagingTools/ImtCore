#pragma once


// ACF includes
#include <istd/THierarchicalBase.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtlicgui/IItem.h>


namespace imtlicgui
{


class CItem: public istd::THierarchicalBase<imtlicgui::IItemTree>
{
public:
	CItem();

	void AddChild(ItemTreePtr childItemPtr);

	// reimplemented (IItem)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual Status GetStatus() const override;
	virtual void GetStatus(Status status) override;
	virtual bool IsActivated() const override;
	virtual void SetActivated(bool isActivated) override;
	virtual bool IsEnabled() const override;
	virtual void SetEnabled(bool isEnabled) override;

	// reimplemented (IItemTree)
	virtual int GetChildsCount() const override;
	virtual IItemTree* GetChild(int index) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const;
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool IsEqual(const istd::IChangeable& object) const;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;
	virtual bool ResetData(CompatibilityMode mode);

private:
	QByteArray m_id;
	QString m_name;
	Status m_status;
	bool m_isActivated;
	bool m_isEnabled;

	QList<ItemTreePtr> m_childs;
};


} // namespace imtlicgui


