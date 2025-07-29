import argparse
import os
import re
import configparser

def ReadTextFromFile(path, aEncoding='utf-8'):
	with open(path, 'r', encoding = aEncoding) as f:
		return f.read()


def GetSchemaDataFromText(text):
	regExp = re.compile(r'schema\s*{([^}]*)}', re.MULTILINE)
	schemaDataMatch = regExp.findall(text)
	if (len(schemaDataMatch) == 1):
		schemaData = schemaDataMatch[0]
		dataRegExp = re.compile(r'\s*(?P<KEY>[^:\s]+)\s*:{1}\s*(?P<VALUE>[^\s]*)\s*')
		schemaMapMatch = dataRegExp.findall(schemaData)

		return schemaMapMatch

	return None


def GetFileSystemAcceptableEntryPath(originalText):
	retVal = originalText

	retVal = retVal.replace('*', '_')
	retVal = retVal.replace('?', '_')
	retVal = retVal.replace('\"', '_')
	retVal = retVal.replace('<', '_')
	retVal = retVal.replace('>', '_')
	retVal = retVal.replace('|', '_')

	while retVal.endswith('.'):
		retVal = retVal[:-1]

	return os.path.abspath(retVal)


def CalculateTargetCppFilesFromTemplateString(schemaName, schemaVersion, schemaNamespace, templateString, outputPath):
	retVal = []

	baseFilePath = templateString
	if not baseFilePath.startswith("$(output-directory)"):
		baseFilePath = "$(output-directory)/" + baseFilePath

	if not baseFilePath.endswith("$(schema.name)"):
		baseFilePath += ("/$(schema.name)")
	
	baseFilePath = baseFilePath.replace("$(output-directory)", outputPath)
	baseFilePath = baseFilePath.replace("$(schema.namespace)", schemaNamespace)
	baseFilePath = baseFilePath.replace("$(schema.name)", schemaName)
	baseFilePath = baseFilePath.replace("$(schema.version)", schemaVersion)

	retVal.append(GetFileSystemAcceptableEntryPath(baseFilePath + (".cpp")))
	retVal.append(GetFileSystemAcceptableEntryPath(baseFilePath + (".h")))

	return retVal


def CalculateTargetCppFilesFromSchemaParams(schemaName, schemaVersion, baseDirPath, defaultName):
	retVal = []

	baseFilePath = baseDirPath

	if schemaVersion != None and len(schemaVersion) > 0:
		if not baseFilePath.endswith('/'):
			baseFilePath += '/'

		baseFilePath += schemaVersion


	if not baseFilePath.endswith('/'):
		baseFilePath += '/'

	baseFilePath += ("CPP/")

	if schemaName != None and len(schemaName) > 0:
		baseFilePath += schemaName
	else:
		baseFilePath += defaultName

	retVal.append(GetFileSystemAcceptableEntryPath(baseFilePath + (".cpp")))
	retVal.append(GetFileSystemAcceptableEntryPath(baseFilePath + (".h")))

	return retVal

def CalculateTargetQmlFilesFromSchemaParams(schemaName, schemaNamespace, schemaVersion, outputDirectoryPath, defaultName):
	retVal = outputDirectoryPath
	if not retVal.endswith('/'):
		retVal += '/'

	retVal += schemaVersion + '/QML/' + schemaNamespace + schemaName + 'Sdl'
	retVal += '/' + schemaNamespace + schemaName +'Sdl.qrc'

	retVal = GetFileSystemAcceptableEntryPath(retVal)
	return retVal


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Helper script to extract CXX or QML file name from SDL schema')
	parser.add_argument('-S', '--schema-file', required=True, help='SDL schema file path')
	parser.add_argument('-O', '--output-directory', required=True, help='Directory where created files will be created')
	parser.add_argument('--auto-join', required=False, help='Indicates if an automatic join of output files into a single is enabled.', action='store_true')
	parser.add_argument('--CPP', required=False, help='C++ Modificator to generate code.', action='store_true', default=False)
	parser.add_argument('--GQL', required=False, help='GraphQL Modificator to generate GrqphQL wrap C++ code . (disables CPP and QML if it not setted explicit)', action='store_true', default=False)
	parser.add_argument('--QML', required=False, help='QML Modificator to generate code. (disables CPP and GQL if it not setted explicit)', action='store_true', default=False)
	parser.add_argument('--config', required=False, help='config file path')

	args, unknown_args = parser.parse_known_args()

	# first ensure, at least one general mode is enabled
	if not args.CPP and not args.GQL and not args.QML:
		raise BaseException('No expected arguments!')

	outputDirectoryPath = args.output_directory
	schemaFilePath = args.schema_file
	schemaFileData = ReadTextFromFile(schemaFilePath)
	schemaParams = GetSchemaDataFromText(schemaFileData)

	schemaName = ''
	schemaNamespace = ''
	schemaVersion = ''

	for key, value in schemaParams:
		if key == 'namespace':
			schemaNamespace = value
		elif key == 'name':
			schemaName = value
		elif key == 'version':
			schemaVersion = value


	isTemplateEnabled = False
	if not args.config is None:
		settings = configparser.ConfigParser(inline_comment_prefixes=(';'))
		settings.read(args.config)
		isTemplateEnabledValue = settings['Template']['enable']
		## BUG puthon reads value with comment separator
		isTemplateEnabled = (isTemplateEnabledValue == 'true' or isTemplateEnabledValue == 'true;')

	cumulatedFiles = ''
	if args.CPP or args.GQL:
		pathsList = []
		if isTemplateEnabled:
			pathsList = CalculateTargetCppFilesFromTemplateString(schemaName, schemaVersion, schemaNamespace, settings['Template']['output'], outputDirectoryPath)
		else:
			pathsList = CalculateTargetCppFilesFromSchemaParams(schemaName, schemaVersion, outputDirectoryPath, '')
		for path in pathsList:
			cumulatedFiles += path + ';'

	if args.QML:
		cumulatedFiles += CalculateTargetQmlFilesFromSchemaParams(schemaName, schemaNamespace, schemaVersion, outputDirectoryPath, '') + ';'

	cumulatedFiles = cumulatedFiles[:-1]
	print(cumulatedFiles, end='')
