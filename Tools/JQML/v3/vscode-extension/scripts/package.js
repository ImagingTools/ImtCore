const fs = require('fs')
const path = require('path')
const { spawnSync } = require('child_process')
const { loadEngine, serializeEngine } = require('../../compiler/language')

const extensionRoot = path.resolve(__dirname, '..')
const engineRoot = path.resolve(extensionRoot, '..')
const libDir = path.join(extensionRoot, 'lib')

function copyFile(from, to) {
    fs.mkdirSync(path.dirname(to), { recursive: true })
    fs.copyFileSync(from, to)
}

fs.mkdirSync(libDir, { recursive: true })
copyFile(path.join(engineRoot, 'compiler', 'parser.js'), path.join(libDir, 'parser.js'))
copyFile(path.join(engineRoot, 'compiler', 'language.js'), path.join(libDir, 'language.js'))

const engine = loadEngine(engineRoot)
fs.writeFileSync(path.join(libDir, 'engine-index.json'), JSON.stringify(serializeEngine(engine)))

const vsce = spawnSync(
    process.platform === 'win32' ? 'npx.cmd' : 'npx',
    ['--yes', '@vscode/vsce', 'package', '--allow-missing-repository', '--skip-license'],
    { cwd: extensionRoot, stdio: 'inherit', shell: process.platform === 'win32' }
)

if (vsce.status !== 0) {
    process.exit(vsce.status || 1)
}

const vsix = fs.readdirSync(extensionRoot).filter(name => name.endsWith('.vsix')).sort().pop()
if (vsix) {
    console.log('Packed ' + path.join(extensionRoot, vsix))
}
