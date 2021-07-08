#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Interface for providing binary file contents.
 */
class IFileProvider: virtual public istd::IPolymorphic
{
public:
	/**
		\brief extracts data from providing file and puts it in data param
		\param data [OUTPUT] - data of providing file
		\param filePath [INPUT] - path of the file
		\return result of extracting data
	*/
	virtual bool LoadData(QByteArray& data, const QByteArray& filePath) const = 0;
};


} // namespace imtfile


