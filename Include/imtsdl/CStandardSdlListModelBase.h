// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QAbstractListModel>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtsdl/TElementList.h>


namespace imtsdl
{

/**
	\brief C++ native analog of BaseModel.qml.
	\details Exposes the same API surface so that QML code written against BaseModel.qml works transparently with descendants of TSdlAbstractListModel.
	\note This class is used only for registrations in the QML engine.
	\warning DO NOT instantiate this class directly!!! -- use ONLY concrete SDL list model types.
	\warning ALL METHODS DO NOTHING IN THIS BASE CLASS (ASSERTS ONLY)!!!
*/
class CStandardSdlListModelBase : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QObject* owner READ getOwner WRITE setOwner NOTIFY ownerChanged)
	Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
	explicit CStandardSdlListModelBase(QObject* parent = nullptr);

	Q_INVOKABLE [[nodiscard]] 
	virtual QVariantMap get(int row) const;
	
	Q_INVOKABLE [[nodiscard]] 
	virtual QVariant getData(const QString& nameId, int index) const;
	
	Q_INVOKABLE 
	virtual void remove(int index);
	
	Q_INVOKABLE 
	virtual void clear();
	
	Q_INVOKABLE 
	virtual void addElement(QObject* element);
	
	Q_INVOKABLE 
	virtual void appendElement(QObject* element);
	
	Q_INVOKABLE 
	virtual void insertElement(int index, QObject* element);
	
	Q_INVOKABLE 
	virtual void removeElement(int index);
	
	Q_INVOKABLE [[nodiscard]] 
	virtual int getItemsCount() const;
	
	Q_INVOKABLE [[nodiscard]] 
	virtual bool containsKey(const QString& key, int index = 0) const;
	
	Q_INVOKABLE 
	virtual void setProperty(int index, const QString& propName, const QVariant& value);
	
	Q_INVOKABLE 
	virtual bool swapItems(int index1, int index2);
	
	Q_INVOKABLE [[nodiscard]]
	virtual QString toJson() const;
	
	Q_INVOKABLE 
	virtual void fromJSON(const QString& json);
	
	Q_INVOKABLE 
	virtual void createFromJson(const QString& json);
	
	Q_INVOKABLE 
	virtual bool isEqualWithModel(imtsdl::CStandardSdlListModelBase* model) const;
	
	Q_INVOKABLE [[nodiscard]] 
	virtual QObject* copyMe() const;

	[[nodiscard]] virtual QObject* getOwner() const;
	virtual void setOwner(QObject* owner);

	[[nodiscard]] virtual int count() const;

	virtual void ClearCache();
	[[nodiscard]] virtual QVariant GetOrCreateCachedObject(int index) const;

	// reimplemented (QAbstractListModel)
	[[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	[[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
	void finished();
	void ownerChanged();
	void countChanged();
};


} // namespace imtsdl


