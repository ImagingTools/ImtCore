#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>


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
			The TEMPLATE ICON COLOR setting
		*/
		AA_TEMPLATE_ICON_COLOR,

		/**
			The template primary color setting
		*/
		AA_TEMPLATE_PRIMARY_COLOR,

		/**
			The template secondary color setting
		*/
		AA_TEMPLATE_SECONDARY_COLOR,

		/**
			The design token file path setting
		*/
		AA_DESIGN_TOKEN_FILE_PATH
	};

	virtual void SetArguments(int argc, char**argv) = 0;
	virtual bool IsHelpRequested() const = 0;
	virtual QByteArray GetDesignTokenFilePath() const = 0;
	virtual QByteArray GetTemplateIconColor() const = 0;
	virtual QByteArray GetTemplatePrimaryColor() const = 0;
	virtual QByteArray GetTemplateSecondaryColor() const = 0;

};


} // namespace imtstyle


