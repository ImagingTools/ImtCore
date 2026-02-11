// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtddl
{


class IDdlCodeCreatorArgumentsParser
{
public:
	enum AvailableArguments
	{
		/**
			Help display requirenment
		*/
		AA_HELP,

		/**
			The input template file path
		*/
		AA_TEMPLATE_FILE_PATH,

		/**
			The input template dir
		*/
		AA_TEMPLATE_DIR_PATH,

		/**
			The output cpp dir
		*/
		AA_OUTPUT_CPP_DIR_PATH,

		/**
			The output qml dir
		*/
		AA_OUTPUT_QML_DIR_PATH,

		/**
			The output *.h file path parameter for c++ generation
		*/
		AA_OUTPUT_H_FILE_PATH,

		/**
			The output *.cpp file path parameter for c++ generation
		*/
		AA_OUTPUT_CPP_FILE_PATH,

		/**
			The output file path parameter for qml generation
		*/
		AA_OUTPUT_QML_FILE_PATH,

		/**
			The output file path parameter for qml generation
		*/
		AA_OUTPUT_QMLDIR_FILE_PATH,

		/**
			The module name for cpp generation
		*/
		AA_MODULE_CPP_NAME,

		/**
			The module name for qml generation
		*/
		AA_MODULE_QML_NAME
	};

	virtual void SetArguments(int argc, char**argv) = 0;
	virtual QByteArray GetTemplateDirPath() const = 0;
	virtual QByteArray GetCppDirPath() const = 0;
	virtual QByteArray GetQmlDirPath() const = 0;
	virtual QString GetModuleCppName() const = 0;
	virtual QString GetModuleQmlName() const = 0;
};


} // namespace imtddl


