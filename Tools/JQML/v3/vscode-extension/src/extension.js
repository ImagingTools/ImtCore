const vscode = require('vscode')
const path = require('path')
const fs = require('fs')

const KIND_MAP = {
    class: vscode.CompletionItemKind.Class,
    property: vscode.CompletionItemKind.Property,
    method: vscode.CompletionItemKind.Method,
    signal: vscode.CompletionItemKind.Event,
    handler: vscode.CompletionItemKind.Event,
    keyword: vscode.CompletionItemKind.Keyword,
    module: vscode.CompletionItemKind.Module,
    variable: vscode.CompletionItemKind.Variable,
    enum: vscode.CompletionItemKind.EnumMember,
    snippet: vscode.CompletionItemKind.Snippet,
}

let service = null
let diagnostics = null
let debounceTimer = null
let output = null
const reported = new Set()
let dialogCount = 0

function toVsLocations(locations) {
    return (locations || []).filter(item => item && item.filePath).map(item => {
        const start = item.range && item.range.start ? item.range.start : { line: 0, character: 0 }
        const end = item.range && item.range.end ? item.range.end : { line: start.line, character: start.character + 1 }
        return new vscode.Location(
            vscode.Uri.file(item.filePath),
            new vscode.Range(start.line, start.character, end.line, end.character)
        )
    })
}

function exists(filePath) {
    return !!(filePath && fs.existsSync(filePath))
}

function log() {
    if (!output) output = vscode.window.createOutputChannel('JQML')
    return output
}

function report(message, severity) {
    const text = String(message || 'JQML failed')
    log().appendLine(text)
    const key = severity + ':' + text
    if (reported.has(key) || dialogCount >= 4) return
    reported.add(key)
    dialogCount += 1
    const show = severity === 'warning' ? vscode.window.showWarningMessage : vscode.window.showErrorMessage
    show(text, 'Log').then(choice => {
        if (choice === 'Log') log().show()
    })
}

function reportStartup(lang) {
    if (!lang) return
    if (lang.engineLoadError) {
        report('JQML could not load engine types: ' + lang.engineLoadError.message, 'error')
    }
    const issues = lang.configIssues || []
    const errors = issues.filter(item => item.severity === 'error')
    const warnings = issues.filter(item => item.severity !== 'error')
    for (const item of errors) report(item.message, 'error')
    for (const item of warnings) log().appendLine(item.message)
    if (warnings.length) {
        report('JQML skipped ' + warnings.length + ' config paths that were not found. Open the JQML log for the list.', 'warning')
    }
}

function guard(action, fallback) {
    try {
        return action()
    } catch (error) {
        report('JQML failed: ' + (error && error.message ? error.message : error), 'error')
        return fallback
    }
}

function engineRoot(context) {
    const configured = vscode.workspace.getConfiguration('jqml').get('enginePath')
    if (configured) return configured
    const parent = path.resolve(context.extensionPath, '..')
    if (exists(path.join(parent, 'QtQuick', 'QtQuick.js'))) return parent
    return ''
}

function languageEntry(context) {
    const fromEngine = path.join(engineRoot(context) || path.resolve(context.extensionPath, '..'), 'compiler', 'language.js')
    if (exists(fromEngine)) return fromEngine
    const bundled = path.join(context.extensionPath, 'lib', 'language.js')
    if (exists(bundled)) return bundled
    throw new Error('JQML language service was not found. Package the extension or set jqml.enginePath.')
}

function expandSettingPath(value, folders) {
    if (!value) return value
    let result = String(value)
    if (folders && folders[0]) result = result.replace(/\$\{workspaceFolder\}/g, folders[0])
    result = result.replace(/\$\{env:([^}]+)\}/g, (_, name) => process.env[name] || '')
    return result
}

function createService(context) {
    const cfg = vscode.workspace.getConfiguration('jqml')
    const hideInternal = cfg.get('hideInternalMembers') !== false
    const root = engineRoot(context)
    const { LanguageService } = require(languageEntry(context))
    const lang = new LanguageService({
        enginePath: root,
        engineIndexPath: path.join(context.extensionPath, 'lib', 'engine-index.json'),
        hideInternal,
        extraEnv: cfg.get('env') || {},
    })
    const folders = (vscode.workspace.workspaceFolders || []).map(folder => folder.uri.fsPath)
    lang.setWorkspaceFolders(folders)

    const configPath = expandSettingPath(cfg.get('configPath'), folders) || ''
    if (configPath) {
        log().appendLine('JQML config: ' + configPath)
        lang.loadConfig(configPath)
    }

    return lang
}

async function indexWorkspace(lang) {
    const files = await vscode.workspace.findFiles('**/*.{qml,qmldir}', '**/{node_modules,dist}/**')
    for (const file of files) {
        try {
            if (file.fsPath.toLowerCase().endsWith('qmldir')) lang.indexQmldir(file.fsPath)
            else lang.indexQmlFile(file.fsPath)
        } catch (error) {
            report('JQML failed to index ' + file.fsPath + ': ' + error.message, 'error')
        }
    }
}

function refreshDocument(document) {
    if (!service || document.languageId !== 'jqml') return
    guard(() => service.indexQmlFile(document.uri.fsPath, document.getText()))
}

function activate(context) {
    diagnostics = vscode.languages.createDiagnosticCollection('jqml')
    context.subscriptions.push(diagnostics, log())
    try {
        service = createService(context)
    } catch (error) {
        report(error.message, 'error')
        return
    }
    reportStartup(service)

    const reindex = () => {
        indexWorkspace(service).then(() => {
            for (const document of vscode.workspace.textDocuments) {
                refreshDocument(document)
                publishDiagnostics(document)
            }
        }).catch(error => {
            report('JQML failed to index the workspace: ' + error.message, 'error')
        })
    }
    reindex()

    context.subscriptions.push(vscode.workspace.onDidChangeTextDocument(event => {
        if (event.document.languageId === 'jqml') scheduleDiagnostics(event.document)
    }))

    context.subscriptions.push(vscode.workspace.onDidOpenTextDocument(document => {
        refreshDocument(document)
        publishDiagnostics(document)
    }))

    context.subscriptions.push(vscode.workspace.onDidSaveTextDocument(document => {
        guard(() => {
            if (document.fileName.toLowerCase().endsWith('qmldir')) {
                service.indexQmldir(document.uri.fsPath)
            } else {
                refreshDocument(document)
            }
        })
        publishDiagnostics(document)
    }))

    context.subscriptions.push(vscode.workspace.onDidCreateFiles(event => {
        for (const file of event.files) {
            guard(() => {
                if (file.fsPath.toLowerCase().endsWith('qmldir')) service.indexQmldir(file.fsPath)
                if (file.fsPath.toLowerCase().endsWith('.qml')) service.indexQmlFile(file.fsPath)
            })
        }
    }))

    context.subscriptions.push(vscode.workspace.onDidDeleteFiles(event => {
        for (const file of event.files) guard(() => service.removeFile(file.fsPath))
    }))

    context.subscriptions.push(vscode.workspace.onDidChangeConfiguration(event => {
        if (!event.affectsConfiguration('jqml')) return
        reported.clear()
        dialogCount = 0
        try {
            service = createService(context)
        } catch (error) {
            report(error.message, 'error')
            return
        }
        reportStartup(service)
        reindex()
    }))

    context.subscriptions.push(vscode.languages.registerCompletionItemProvider('jqml', {
        provideCompletionItems(document, position) {
            if (!service) return []
            return guard(() => {
                const text = document.getText()
                const offset = document.offsetAt(position)
                return service.getCompletions(document.uri.fsPath, text, offset).map(item => {
                    const completion = new vscode.CompletionItem(item.label, KIND_MAP[item.kind] || vscode.CompletionItemKind.Text)
                    completion.detail = item.detail
                    completion.insertText = item.insertText || item.label
                    if (item.sortText) completion.sortText = item.sortText
                    return completion
                })
            }, [])
        }
    }, '.'))

    context.subscriptions.push(vscode.languages.registerDefinitionProvider('jqml', {
        provideDefinition(document, position) {
            if (!service) return null
            return guard(() => {
                const text = document.getText()
                const offset = document.offsetAt(position)
                const locations = service.getDefinition(document.uri.fsPath, text, offset)
                return toVsLocations(locations)
            }, null)
        }
    }))

    context.subscriptions.push(vscode.languages.registerReferenceProvider('jqml', {
        provideReferences(document, position) {
            if (!service) return []
            return guard(() => {
                const text = document.getText()
                const offset = document.offsetAt(position)
                return toVsLocations(service.getReferences(document.uri.fsPath, text, offset))
            }, [])
        }
    }))

    context.subscriptions.push(vscode.languages.registerHoverProvider('jqml', {
        provideHover(document, position) {
            if (!service) return null
            return guard(() => {
                const text = document.getText()
                const offset = document.offsetAt(position)
                const hover = service.getHover(document.uri.fsPath, text, offset)
                if (!hover) return null
                const md = new vscode.MarkdownString()
                md.appendCodeblock(hover.detail || '', 'qml')
                if (hover.range && hover.range.start && hover.range.end) {
                    const range = new vscode.Range(
                        hover.range.start.line,
                        hover.range.start.character,
                        hover.range.end.line,
                        hover.range.end.character
                    )
                    return new vscode.Hover(md, range)
                }
                return new vscode.Hover(md)
            }, null)
        }
    }))
}

function publishDiagnostics(document) {
    if (!service || !diagnostics || document.languageId !== 'jqml') return
    const items = guard(() => service.getDiagnostics(document.uri.fsPath, document.getText()), [])
    const result = []
    for (const item of items || []) {
        if (!item || !item.range || !item.range.start || !item.range.end) continue
        const range = new vscode.Range(
            item.range.start.line,
            item.range.start.character,
            item.range.end.line,
            item.range.end.character
        )
        const severity = item.severity === 'warning'
            ? vscode.DiagnosticSeverity.Warning
            : vscode.DiagnosticSeverity.Error
        result.push(new vscode.Diagnostic(range, item.message, severity))
    }
    diagnostics.set(document.uri, result)
}

function scheduleDiagnostics(document) {
    if (debounceTimer) clearTimeout(debounceTimer)
    debounceTimer = setTimeout(() => {
        refreshDocument(document)
        publishDiagnostics(document)
    }, 250)
}


function deactivate() {
    if (debounceTimer) clearTimeout(debounceTimer)
    service = null
}

module.exports = {
    activate,
    deactivate,
}
