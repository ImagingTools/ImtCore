#!/usr/bin/env python3
"""Keep the knowledge of the AI assistant compiled into the servers.

Writes the <qresource prefix="..."> blocks of the configuration into every listed .qrc file:
the files of the listed directories plus the extra files.
The production servers have no access to GitHub: the knowledge exists for them only as
these resources, built into the binaries.

Usage: update_qrc.py [--check]   (from the root of the repository)
  --check  change nothing; exit with 1 if a .qrc file is not up to date
"""

import json
import os
import re
import sys


def collect_files(root, block):
	files = {}
	for directory in block.get('directories', []):
		path = directory['path']
		suffix = directory.get('suffix', '.md')
		alias_prefix = directory.get('aliasPrefix', '')
		for name in sorted(os.listdir(os.path.join(root, path))):
			if name.endswith(suffix):
				files[alias_prefix + name] = path + '/' + name

	for alias, path in block.get('extraFiles', {}).items():
		if alias in files:
			sys.exit('Alias %s is used twice' % alias)
		files[alias] = path

	for path in files.values():
		if not os.path.isfile(os.path.join(root, path)):
			sys.exit('Knowledge file %s does not exist' % path)

	return files


def create_block(root, qrc_dir, prefix, files):
	lines = ['    <qresource prefix="%s">' % prefix]
	for alias in sorted(files):
		relative = os.path.relpath(os.path.join(root, files[alias]), qrc_dir).replace(os.sep, '/')
		lines.append('        <file alias="%s">%s</file>' % (alias, relative))
	lines.append('    </qresource>')

	return '\n'.join(lines) + '\n'


def main():
	check = '--check' in sys.argv[1:]
	root = os.getcwd()
	config_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'config.json')
	with open(config_path, encoding='utf-8') as config_file:
		config = json.load(config_file)

	blocks = [(block['prefix'], collect_files(root, block)) for block in config['blocks']]

	outdated = []
	for qrc in config['qrcFiles']:
		qrc_path = os.path.join(root, qrc)
		with open(qrc_path, 'rb') as qrc_file:
			data = qrc_file.read().decode('utf-8')

		newline = '\r\n' if '\r\n' in data else '\n'
		text = data.replace('\r\n', '\n')
		new_text = text
		for prefix, files in blocks:
			block = create_block(root, os.path.dirname(qrc_path), prefix, files)
			pattern = re.compile(r'    <qresource prefix="%s">\n.*?    </qresource>\n' % re.escape(prefix), re.S)
			if pattern.search(new_text):
				new_text = pattern.sub(lambda match: block, new_text, count=1)
			else:
				new_text = new_text.replace('</RCC>', block + '</RCC>', 1)

		if new_text != text:
			outdated.append(qrc)
			if not check:
				with open(qrc_path, 'wb') as qrc_file:
					qrc_file.write(new_text.replace('\n', newline).encode('utf-8'))

	if check and outdated:
		print('The knowledge resources are not up to date: %s' % ', '.join(outdated))
		print('Run: python3 .github/assistant-knowledge/update_qrc.py')
		return 1

	for qrc in outdated:
		print('Updated %s' % qrc)

	return 0


if __name__ == '__main__':
	sys.exit(main())
