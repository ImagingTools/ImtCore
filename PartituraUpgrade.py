import pathlib, os, subprocess, re


currentDir = os.path.dirname(os.path.realpath(__file__))
svnProcess = subprocess.Popen(
			['svn', 'info', '--show-item', 'revision', currentDir], 
			stdin=subprocess.PIPE,
			stdout=subprocess.PIPE)
svnProcessOutput = svnProcess.communicate()
svnRevision = svnProcessOutput[0].decode()
svnRevision = svnRevision.replace('\n', '')
print (f'SVN revision: {svnRevision}')

rootSearchPath = os.path.dirname(os.path.realpath(__file__))
xtrAttrFileList = pathlib.Path(rootSearchPath).rglob('*.acc.xtrsvn')



for xtrAttrFilePath in xtrAttrFileList:
	print(f'processing: {xtrAttrFilePath}')
	with open(xtrAttrFilePath, 'r') as xtrAttrFile:
		xtrAttrFileData = xtrAttrFile.read()
		outputFilePath = re.sub(r'.xtrsvn$', r'', str(xtrAttrFilePath))
		print (f'creating {outputFilePath}')
		with open(outputFilePath, 'w') as accOutputFile:
			accOutputFileData = xtrAttrFileData.replace('$WCREV$', svnRevision)
			print (f'writting {outputFilePath}')
			accOutputFile.write(accOutputFileData)