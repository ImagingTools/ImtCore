#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{

/**
 * \brief The IBinaryDataController provides managment of the file from disc, resources or memory
 */
class IBinaryDataController:  virtual public istd::IPolymorphic
{
public:
	/**
		\brief writes data to providing file
		\param data [INPUT] - data of providing file
		\param dataId [INPUT] - name of providing file
		\return result of extracting data
	 */
    virtual bool SetData(const QByteArray& data, QByteArray& dataId) const = 0;

	/**
		\brief removes data from providing file
		\param dataId [INPUT] - name of providing file
		\return result of extracting data
	 */
	virtual bool RemoveData(const QByteArray& dataId) const = 0;

	/**
		\brief Checks if file by provided id alerady exists
		\param dataId - id for checking file
		\return existence of file
		\returns \c true if file exists and \c false otherwise
	 */
	virtual bool EnsureDataExists(const QByteArray& dataId) const = 0;
};


} // namespace imtbase


