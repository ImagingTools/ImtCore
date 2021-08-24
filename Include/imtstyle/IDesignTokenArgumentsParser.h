#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtstyle
{


class IDesignTokenArgumentsParser
{
public:
	enum AvailableArguments
	{
		/**
			Help display requirenment
		*/
		AA_HELP,

		/**
			The input directory parameter
		*/
		AA_DESIGN_TOKEN_FILE_PATH,

		/**
			The input directory parameter
		*/
		AA_INPUT_DIRECTORY_PATH,

		/**
			The output directory parameter
		*/
		AA_OUTPUT_DIRECTORY_PATH,

		/**
			The output directory parameter
		*/
		AA_PROJECT_NAME,

		/**
			The output directory parameter
		*/
		AA_IMAGE_MODE,

		/**
			The output directory parameter
		*/
		AA_STYLE_SHEET_MODE
	};

	virtual void SetArguments(int argc, char**argv) = 0;
	virtual bool IsHelpRequested() const = 0;
	virtual bool IsImageModeRequired() const = 0;
	virtual bool IsStyleSheetModeRequired() const = 0;
	virtual QByteArray GetDesignTokenFilePath() const = 0;
	virtual QByteArray GetInputDirectoryPath() const = 0;
	virtual QByteArray GetOutputDirectoryPath() const = 0;
	virtual QByteArray GetProjectName() const = 0;

};


} // namespace imtstyle


