const assert = require('assert')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { spawnSync } = require('child_process')

const root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-list-'))
const includedDir = path.join(root, 'included-mod')
const rootDir = path.join(root, 'root-mod')
const configDir = path.join(root, 'cfg')
fs.mkdirSync(includedDir, { recursive: true })
fs.mkdirSync(path.join(rootDir, 'Sub'), { recursive: true })
fs.mkdirSync(configDir, { recursive: true })

fs.writeFileSync(path.join(includedDir, 'Card.qml'), 'import QtQuick\nItem {}\n')
fs.writeFileSync(path.join(includedDir, 'Util.js'), 'function f(){}\n')
fs.writeFileSync(path.join(includedDir, 'qmldir'), [
    'module included',
    'Card 1.0 Card.qml',
    'Card 2.0 Card.qml',
    'Util 1.0 Util.js',
    'singleton Info 1.0 Card.qml',
    '# comment',
    '',
].join('\n'))

fs.writeFileSync(path.join(rootDir, 'Sub', 'Page.qml'), 'import QtQuick\nItem {}\n')
fs.writeFileSync(path.join(rootDir, 'qmldir'), 'module rootmod\nPage 1.0 Sub/Page.qml\n')
fs.writeFileSync(path.join(configDir, 'Main.qml'), 'import QtQuick\nItem {}\n')
fs.writeFileSync(path.join(configDir, 'Helper.qml'), 'import QtQuick\nItem {}\n')
fs.writeFileSync(path.join(configDir, 'skip.js'), 'function f(){}\n')

fs.writeFileSync(path.join(root, 'included.json'), JSON.stringify({
    dirs: [includedDir.replace(/\\/g, '/')],
}))
fs.writeFileSync(path.join(configDir, 'app.json'), JSON.stringify({
    includes: ['${MODROOT}/included.json'],
    dirs: ['../root-mod'],
}))

const originalArgv = process.argv
process.argv = [originalArgv[0], path.join(__dirname, '../compiler/compiler.js')]
const { listQmldirQmlFiles } = require('../compiler/compiler')
process.argv = originalArgv

const env = Object.assign({}, process.env, { MODROOT: root })
const files = listQmldirQmlFiles(path.join(configDir, 'app.json'), env, 'Main.qml').map(filePath => filePath.replace(/\\/g, '/'))
const expectedModules = [
    path.resolve(includedDir, 'Card.qml').replace(/\\/g, '/'),
    path.resolve(rootDir, 'Sub/Page.qml').replace(/\\/g, '/'),
]
const expectedEntry = [
    path.resolve(configDir, 'Main.qml').replace(/\\/g, '/'),
    path.resolve(configDir, 'Helper.qml').replace(/\\/g, '/'),
]
assert.deepStrictEqual(files.slice(0, 2), expectedModules)
assert.deepStrictEqual(files.slice(2).sort(), expectedEntry.sort())

const cli = spawnSync(process.execPath, [
    path.join(__dirname, '../compiler/compiler.js'),
    '--list-qml',
    '-c', path.join(configDir, 'app.json'),
    '-e', 'Main.qml',
], { env, encoding: 'utf8' })
assert.strictEqual(cli.status, 0, cli.stderr)
assert.strictEqual(cli.stdout.replace(/\r/g, ''), files.join('\n') + '\n')
assert.strictEqual(cli.stderr, '')

console.log('listQmldirQmlFiles ok')
