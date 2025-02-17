import argparse
import os
import re


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

	retVal.replace('*', '_')
	retVal.replace('?', '_')
	retVal.replace('\"', '_')
	retVal.replace('<', '_')
	retVal.replace('>', '_')
	retVal.replace('|', '_')

	while retVal.endswith('.'):
		retVal = retVal[:-1]

	return os.path.abspath(retVal)


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
	parser.add_argument('--auto-link', required=False, help='Defines the compilation order of the schema files.\n'
												   '0 - disabled. ALL files will be compiled.\n'
												   '1 - only those schemas with the same namespace as the original one will be compiled\n'
												   '2 - only the schema will be compiled. See the \'schema-file\' option', default=2)
	parser.add_argument('--CPP', required=False, help='C++ Modificator to generate code.', action='store_true', default=False)
	parser.add_argument('--GQL', required=False, help='GraphQL Modificator to generate GrqphQL wrap C++ code . (disables CPP and QML if it not setted explicit)', action='store_true', default=False)
	parser.add_argument('--QML', required=False, help='QML Modificator to generate code. (disables CPP and GQL if it not setted explicit)', action='store_true', default=False)

	args, unknown_args = parser.parse_known_args()

	# first ensure, at least one general mode is enabled
	if not args.CPP and not args.GQL and not args.QML:
		raise BaseException('No expected arguments!')

	outputDirectoryPath = args.output_directory
	schemaFilePath = args.schema_file
	schemaFileData = ReadTextFromFile(schemaFilePath)
	schemaParams = GetSchemaDataFromText(schemaFileData)

	if args.auto_link != '2':
		raise BaseException(f'Unexpected auto link! L2 only support. actual {args.auto_link}')

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

	cumulatedFiles = ''
	if args.CPP or args.GQL:
		for path in CalculateTargetCppFilesFromSchemaParams(schemaName, schemaVersion, outputDirectoryPath, ''):
			cumulatedFiles += path + ';'

	if args.QML:
		cumulatedFiles += CalculateTargetQmlFilesFromSchemaParams(schemaName, schemaNamespace, schemaVersion, outputDirectoryPath, '') + ';'

	cumulatedFiles = cumulatedFiles[:-1]
	print(cumulatedFiles, end='')
