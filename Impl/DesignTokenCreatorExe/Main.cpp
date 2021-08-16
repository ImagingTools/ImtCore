// STL includes
#include <iostream>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// project includes
#include <imtstyle/CDesignTokenArgumentsParser.h>

#include <GeneratedFiles/DesignTokenCreatorExe/CDesignTokenCreatorExe.h>

static bool SetParams(int argc, char** argv, iprm::IParamsSet& paramSet, QString& errorString)
{
	using imtstyle::CDesignTokenArgumentsParser;
	bool retval = false;
	QByteArrayList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << argv[i];
	}
	CDesignTokenArgumentsParser parser(argc, argv);

	if (parser.CheckArgument(CDesignTokenArgumentsParser::AA_HELP)){
		QByteArray paramId = "HelpParam";

		iprm::IEnableableParam* templateIconColorParamPtr = dynamic_cast<iprm::IEnableableParam*>(paramSet.GetEditableParameter(paramId));
		Q_ASSERT(templateIconColorParamPtr != nullptr);

		templateIconColorParamPtr->SetEnabled(true);
	}
	if (parser.CheckArgument(CDesignTokenArgumentsParser::AA_TEMPLATE_ICON_COLOR)){QByteArray paramId = "TemplateIconColor";
		QByteArray paramValue = parser.GetArgumentValue(CDesignTokenArgumentsParser::AA_TEMPLATE_ICON_COLOR);

		iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(paramSet.GetEditableParameter(paramId));
		Q_ASSERT(templateIconColorParamPtr != nullptr);

		templateIconColorParamPtr->SetId(paramValue);
	}
	if (parser.CheckArgument(CDesignTokenArgumentsParser::AA_TEMPLATE_PRIMARY_COLOR)){
		QByteArray paramId = "TemplatePrimaryColor";
		QByteArray paramValue = parser.GetArgumentValue(CDesignTokenArgumentsParser::AA_TEMPLATE_PRIMARY_COLOR);

		iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(paramSet.GetEditableParameter(paramId));
		Q_ASSERT(templateIconColorParamPtr != nullptr);

		templateIconColorParamPtr->SetId(paramValue);
	}
	if (parser.CheckArgument(CDesignTokenArgumentsParser::AA_TEMPLATE_SECONDARY_COLOR)){
		QByteArray paramId = "TemplateSecondaryColor";
		QByteArray paramValue = parser.GetArgumentValue(CDesignTokenArgumentsParser::AA_TEMPLATE_SECONDARY_COLOR);

		iprm::IIdParam* templateIconColorParamPtr = dynamic_cast<iprm::IIdParam*>(paramSet.GetEditableParameter(paramId));
		Q_ASSERT(templateIconColorParamPtr != nullptr);

		templateIconColorParamPtr->SetId(paramValue);
	}
	if (parser.CheckArgument(CDesignTokenArgumentsParser::AA_DESIGN_TOKEN_FILE_PATH)){
		QByteArray paramId = "DesignTokenFilePath";
		QByteArray paramValue = parser.GetArgumentValue(CDesignTokenArgumentsParser::AA_DESIGN_TOKEN_FILE_PATH);

		ifile::IFileNameParam* designTokenFilePathParamPtr = dynamic_cast<ifile::IFileNameParam*>(paramSet.GetEditableParameter(paramId));
		Q_ASSERT(designTokenFilePathParamPtr != nullptr);

		designTokenFilePathParamPtr->SetPath(paramValue);
	}
	return retval;
}


int main(int argc, char *argv[])
{
	CDesignTokenCreatorExe instance;

	QString errorString = "unknown error";

	iprm::IParamsSet* paramSetPtr = instance.GetInterface<iprm::IParamsSet>("ConversionParams");
	if (paramSetPtr == nullptr){
		std::cerr << "Component configuration failed" << std::endl;
		return -1;
	}
	if (!SetParams(argc, argv, *paramSetPtr, errorString)){
		std::cerr << errorString.toLocal8Bit().constData();
		return -2;
	}

	/// \todo DO NOT forget check DesignTokenProcessor!!! DesignTokenProcessor->Check();

	ibase::IApplication* applicationPtr = instance.GetInterface<ibase::IApplication>();
	if (applicationPtr != NULL){
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


