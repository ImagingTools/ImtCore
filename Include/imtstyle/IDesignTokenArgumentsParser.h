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
			The output directory parameter
		*/
		AA_OUTPUT_DIRECTORY_PATH,

		/**
			The output directory parameter
		*/
		AA_PROJECT_NAME,

		/**
			The input directory parameter for images
		*/
		AA_IMAGES_INPUT_DIR_PATH,

		/**
			The input directory parameter for style sheets
		*/
		AA_STYLE_SHEETS_INPUT_DIR_PATH,

		/**
			The split file by parts behavior modificator
		*/
		AA_SPLIT_FILE_MODE,

		/**
			The colpy design token file to output dir behavior modificator
		*/
		AA_COPY_DESIGN_TOKEN_FILE
	};

	virtual void SetArguments(int argc, char**argv) = 0;
	virtual bool IsHelpRequested() const = 0;
	virtual bool IsSplitModeRequired() const = 0;
	virtual bool IsCopyDesignTokenFileRequired() const = 0;
	virtual QByteArray GetDesignTokenFilePath() const = 0;
	virtual QByteArray GetImagesInputDirectoryPath() const = 0;
	virtual QByteArray GetStyleSheetsInputDirectoryPath() const = 0;
	virtual QByteArray GetOutputDirectoryPath() const = 0;
	virtual QByteArray GetProjectName() const = 0;

};


} // namespace imtstyle


