#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{

/**
 * \brief The IBinaryDataProvider class provides file from disc, resources or memory
 */
class IBinaryDataProvider:  virtual public istd::IPolymorphic
{
public:
	/*!
	 * \brief extracts data from providing file and puts it in data param
	 * \param data [OUTPUT] - data of providing file
	 * \param dataId [INPUT] - name of providing file
	 * \return result of extracting data
	 */
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const = 0;
};


} // namespace imtbase


