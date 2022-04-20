#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QByteArray>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class ICollectionInfo;


/**
	Single selection of the elements identified by an unique ID.
*/
class ISingleSelection: virtual public iser::ISerializable
{
public:
	typedef QByteArray Id;

	static const QByteArray s_cidSelectionChanged;
	struct SelectionChangeInfo
	{
		Id selectedId;
	};

	/**
		Get the list of options defining possible selection range.
	*/
	virtual const ICollectionInfo* GetSelectionConstraints() const = 0;

	/**
		Get selected ID.
	*/
	virtual Id GetSelectedId() const = 0;

	/**
		Set selected ID.
	*/
	virtual bool SetSelectedId(const Id& selectedId) = 0;
};


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::ISingleSelection::SelectionChangeInfo);


