#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{

/**
 * \brief The IFileProvider class provides file from disc, resources or memory
 */
class IFileProvider:  virtual public istd::IPolymorphic
{

public:
	/*!
	 * \brief extracts data from providing file and puts it in data param
	 * \param data [OUTPUT] - data of providing file
	 * \param name [INPUT] - name of providing file
	 * \return result of extracting data
	 */
	virtual bool LoadData(QByteArray& data, const QByteArray& name) const = 0;


};


} // namespace imtfile


