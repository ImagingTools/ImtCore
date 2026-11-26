// The JQML 4 Composer: a custom editor of `*.jqschema` files that generates a JQML 4 application (see composer.js). It is a separate
// extension. For the palette of the types and the properties in the inspector it runs its own copy of the JQML 4 language service
// (language/, prepared by "npm run build" from the jqml-vscode folder), so it does not need the JQML 4 language extension.

const vscode = require('vscode')
const path = require('path')
const fs = require('fs')
const { pathToFileURL } = require('url')
const { registerComposer } = require('./composer')

let service = null
let loading = null
let output = null

function log() {
    if (!output) output = vscode.window.createOutputChannel('JQML 4 Composer')
    return output
}

function expand(value, folders) {
    if (!value) return value
    let result = String(value)
    if (folders[0]) result = result.replace(/\$\{workspaceFolder\}/g, folders[0])
    return result.replace(/\$\{env:([^}]+)\}/g, (_, name) => process.env[name] || '')
}

function engineRoot(context) {
    const configured = vscode.workspace.getConfiguration('jqml4composer').get('enginePath')
    if (configured && fs.existsSync(path.join(configured, 'src', 'items.js'))) return configured
    // The extension lives in <engine>/plugins/jqml-composer when it is run from the source tree.
    const parent = path.resolve(context.extensionPath, '..', '..')
    return fs.existsSync(path.join(parent, 'src', 'items.js')) ? parent : ''
}

async function createService(context) {
    const cfg = vscode.workspace.getConfiguration('jqml4composer')
    const folders = (vscode.workspace.workspaceFolders || []).map(folder => folder.uri.fsPath)
    const { LanguageService } = await import(pathToFileURL(path.join(context.extensionPath, 'language', 'src', 'service.mjs')).href)
    const lang = new LanguageService({
        engineIndexPath: path.join(context.extensionPath, 'language', 'lib', 'engine-index.json'),
        engineRoot: engineRoot(context),
        hideInternal: true,
        unknownTypes: false,
        unknownMembers: false,
    })
    lang.setImportPaths((cfg.get('importPaths') || []).map(item => expand(item, folders)).filter(Boolean))
    const configPath = expand(cfg.get('configPath'), folders)
    if (configPath) {
        log().appendLine('JQML 4 config: ' + configPath)
        lang.loadConfig(configPath, cfg.get('env') || {})
        for (const issue of lang.configIssues) log().appendLine(issue.message)
        const errors = lang.configIssues.filter(issue => issue.severity === 'error')
        if (errors.length) vscode.window.showErrorMessage(errors[0].message)
    }
    const qmldirs = await vscode.workspace.findFiles('**/qmldir', '**/node_modules/**', 2000)
    for (const file of qmldirs) {
        try {
            lang.indexQmldir(file.fsPath)
        } catch (error) {
            log().appendLine('Cannot index ' + file.fsPath + ': ' + error.message)
        }
    }
    return lang
}

function ready() {
    return loading || Promise.resolve(null)
}

function reload(context) {
    service = null
    loading = createService(context)
        .then(created => {
            service = created
        })
        .catch(error => {
            log().appendLine('JQML 4 Composer could not start: ' + (error.stack || error.message))
            vscode.window.showErrorMessage('JQML 4 Composer could not start: ' + error.message + '. Run "npm run build" in the extension folder.')
        })
    return loading
}

function activate(context) {
    context.subscriptions.push(log())
    reload(context)
    registerComposer(context, () => service, ready, () => engineRoot(context))
    context.subscriptions.push(
        vscode.commands.registerCommand('jqml4composer.reload', () => reload(context)),
        vscode.workspace.onDidChangeConfiguration(event => {
            if (event.affectsConfiguration('jqml4composer')) reload(context)
        }),
    )
}

function deactivate() {
    service = null
}

module.exports = { activate, deactivate }
