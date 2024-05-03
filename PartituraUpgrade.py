import pathlib, os, subprocess, re, argparse


def GetSvnVersion(dirPath):
	svnProcess = subprocess.Popen(
				['svn', 'info', '--show-item', 'last-changed-revision', dirPath], 
				stdin=subprocess.PIPE,
				stdout=subprocess.PIPE)
	svnProcessOutput = svnProcess.communicate()
	svnRevision = svnProcessOutput[0].decode()
	svnRevision = svnRevision.replace('\n', '')
	svnRevision = svnRevision.replace('\t', '')
	svnRevision = svnRevision.replace('\r', '')
	print(f'Version: {svnRevision} of {dirPath}')

	return svnRevision;


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Helper script to upgrade partitura")
	parser.add_argument('-D', '--working-directory', required=False, help='The directory in which you want to find version template files')
	args = parser.parse_args()

	if (not args.working_directory is None )and (len(args.working_directory) > 0):
		workingDirectory = args.working_directory
	else:
		workingDirectory = os.path.dirname(os.path.realpath(__file__))

	print (f'Lookup starts at: {workingDirectory}')
	xtrAttrFileList = pathlib.Path(workingDirectory).rglob('*.acc.xtrsvn')
	for xtrAttrFilePath in xtrAttrFileList:
		print(f'processing: {xtrAttrFilePath}')
		#cdUp X2 ->.arp ->Partitura ->dest
		svnRevision = GetSvnVersion(xtrAttrFilePath.parents[2])
		with open(xtrAttrFilePath, 'r') as xtrAttrFile:
			xtrAttrFileData = xtrAttrFile.read()
			outputFilePath = re.sub(r'.xtrsvn$', r'', str(xtrAttrFilePath))
			print (f'creating {outputFilePath}')
			with open(outputFilePath, 'w') as accOutputFile:
				accOutputFileData = xtrAttrFileData.replace('$WCREV$', svnRevision)
				print (f'writting {outputFilePath}')
				accOutputFile.write(accOutputFileData)
