#pragma once


// Qt includes
#include <QtCore/QVector>

// ACF includes
#include <istd/TComposedFactory.h>
#include <istd/TSmartPtr.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectContainer.h>


namespace imtbase
{


/**
	Implementation of an object container.
*/
class CObjectContainer:
			virtual public IObjectContainer,
			virtual protected iprm::IOptionsList,
			public istd::TComposedFactory<istd::IChangeable>
{
public:
	typedef istd::TComposedFactory<istd::IChangeable> BaseClass;

	CObjectContainer();
	virtual ~CObjectContainer();

	// reimplemented (IObjectContainer)
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;

	// reimplemented (IObjectProvider)
	virtual const iprm::IOptionsList& GetObjectInfoList() const override;
	virtual const istd::IChangeable* GetObject(const QByteArray& objectId) const override;
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;

protected:
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const;

private:
	// reimplemented (IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

protected:
	typedef istd::TSmartPtr<istd::IChangeable> ObjectPtr;

	struct ObjectInfo
	{
		ObjectPtr object;
		QString name;
		QString description;
		QByteArray id;
		QByteArray typeId;
	};

	typedef QVector<ObjectInfo> Objects;

private:
	imod::CModelUpdateBridge m_modelUpdateBridge;

	Objects m_objects;
};


} // namespace imtbase


