#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>


namespace imtdesign
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
			The output file parameter
		*/
		AA_OUTPUT_FILE_PATH,

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
			The copy design token file to output dir behavior modificator
		*/
		AA_COPY_DESIGN_TOKEN_FILE,

		/**
			The web compiler parameter
		*/
		AA_WEB,

		/**
			The creating a QMLl file for icons
		*/
		AA_QML,

		/**
			The copy design token errors ignore behavior modificator
			\details Tells processors to ignore any errors, that could be emerged
			\warning The program with this modificator must finished with code 0 ALWAYS!!!
		*/
		AA_ERRORS_IGNORE_MODE
	};

	virtual void SetArguments(int argc, char**argv) = 0;
	virtual bool IsHelpRequested() const = 0;
	virtual bool IsErrorsIgnoreRequested() const = 0;
	virtual bool IsSplitModeRequired() const = 0;
	virtual bool IsCopyDesignTokenFileRequired() const = 0;
	virtual bool IsWebGenerateResourceRequired() const = 0;
	virtual bool IsQmlGenerateRequired() const = 0;
	virtual QByteArray GetDesignTokenFilePath() const = 0;
	virtual QByteArray GetImagesInputDirectoryPath() const = 0;
	virtual QByteArrayList GetImagesInputDirectoryMultiPath() const = 0;
	virtual QByteArray GetStyleSheetsInputDirectoryPath() const = 0;
	virtual QByteArray GetOutputDirectoryPath() const = 0;
	virtual QByteArray GetOutputFilePath() const = 0;
	virtual QByteArray GetProjectName() const = 0;

};


} // namespace imtdesign


