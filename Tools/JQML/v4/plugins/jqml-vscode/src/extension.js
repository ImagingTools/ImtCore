const vscode = require('vscode')
const path = require('path')
const fs = require('fs')
const { pathToFileURL } = require('url')

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
}

const SYMBOL_KIND = {
    class: vscode.SymbolKind.Class,
    property: vscode.SymbolKind.Property,
    method: vscode.SymbolKind.Method,
    event: vscode.SymbolKind.Event,
    enum: vscode.SymbolKind.Enum,
}

const SEVERITY = {
    error: vscode.DiagnosticSeverity.Error,
    warning: vscode.DiagnosticSeverity.Warning,
    information: vscode.DiagnosticSeverity.Information,
}

const SELECTOR = { language: 'jqml4' }

let service = null
let loading = null
let diagnostics = null
let output = null
let debounceTimer = null

function log() {
    if (!output) output = vscode.window.createOutputChannel('JQML 4')
    return output
}

function expand(value, folders) {
    if (!value) return value
    let result = String(value)
    if (folders[0]) result = result.replace(/\$\{workspaceFolder\}/g, folders[0])
    return result.replace(/\$\{env:([^}]+)\}/g, (_, name) => process.env[name] || '')
}

function toRange(range) {
    return new vscode.Range(range.start.line, range.start.character, range.end.line, range.end.character)
}

function toLocations(items) {
    return (items || []).filter(item => item && item.filePath).map(item => new vscode.Location(vscode.Uri.file(item.filePath), toRange(item.range)))
}

function engineRoot(context) {
    const configured = vscode.workspace.getConfiguration('jqml4').get('enginePath')
    if (configured && fs.existsSync(path.join(configured, 'src', 'items.js'))) return configured
    // The extension lives in <engine>/plugins/jqml-vscode when it is run from the source tree.
    const parent = path.resolve(context.extensionPath, '..', '..')
    return fs.existsSync(path.join(parent, 'src', 'items.js')) ? parent : ''
}

async function createService(context) {
    const cfg = vscode.workspace.getConfiguration('jqml4')
    const folders = (vscode.workspace.workspaceFolders || []).map(folder => folder.uri.fsPath)
    const { LanguageService } = await import(pathToFileURL(path.join(context.extensionPath, 'src', 'service.mjs')).href)
    const lang = new LanguageService({
        engineIndexPath: path.join(context.extensionPath, 'lib', 'engine-index.json'),
        engineRoot: engineRoot(context),
        hideInternal: cfg.get('hideInternalMembers') !== false,
        unknownTypes: cfg.get('diagnostics.unknownTypes') !== false,
        unknownMembers: cfg.get('diagnostics.unknownMembers') !== false,
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

function publish(document) {
    if (!service || document.languageId !== 'jqml4') return
    try {
        const items = service.getDiagnostics(document.uri.fsPath, document.getText())
        diagnostics.set(document.uri, items.map(item => {
            const diagnostic = new vscode.Diagnostic(toRange(item.range), item.message, SEVERITY[item.severity] || vscode.DiagnosticSeverity.Warning)
            diagnostic.source = 'jqml4'
            return diagnostic
        }))
    } catch (error) {
        log().appendLine('Diagnostics failed for ' + document.uri.fsPath + ': ' + (error.stack || error.message))
    }
}

function guard(action, fallback) {
    try {
        return action()
    } catch (error) {
        log().appendLine('JQML 4 failed: ' + (error.stack || error.message))
        return fallback
    }
}

function symbolOf(item) {
    const symbol = new vscode.DocumentSymbol(item.name, item.detail || '', SYMBOL_KIND[item.kind] || vscode.SymbolKind.Object, toRange(item.range), toRange(item.selection))
    symbol.children = (item.children || []).map(symbolOf)
    return symbol
}

async function start(context) {
    service = null
    service = await createService(context)
    for (const document of vscode.workspace.textDocuments) publish(document)
}

function reload(context) {
    loading = start(context).catch(error => {
        log().appendLine('JQML 4 could not start: ' + (error.stack || error.message))
        vscode.window.showErrorMessage('JQML 4 could not start: ' + error.message + '. Run "npm run build" in the extension folder.')
    })
}

function activate(context) {
    diagnostics = vscode.languages.createDiagnosticCollection('jqml4')
    context.subscriptions.push(diagnostics, log())
    reload(context)

    const schedule = document => {
        if (document.languageId !== 'jqml4') return
        if (debounceTimer) clearTimeout(debounceTimer)
        debounceTimer = setTimeout(() => publish(document), 250)
    }

    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument(event => schedule(event.document)),
        vscode.workspace.onDidOpenTextDocument(document => ready().then(() => publish(document))),
        vscode.workspace.onDidCloseTextDocument(document => {
            diagnostics.delete(document.uri)
            if (service && document.languageId === 'jqml4') service.closeDocument(document.uri.fsPath)
        }),
        vscode.workspace.onDidSaveTextDocument(document => {
            if (!service || path.basename(document.fileName) !== 'qmldir') return
            guard(() => service.indexQmldir(document.uri.fsPath))
            for (const doc of vscode.workspace.textDocuments) publish(doc)
        }),
        vscode.workspace.onDidChangeConfiguration(event => {
            if (event.affectsConfiguration('jqml4')) reload(context)
        }),
        vscode.workspace.onDidDeleteFiles(event => {
            for (const file of event.files) if (service) service.removeFile(file.fsPath)
        }),
        vscode.commands.registerCommand('jqml4.reload', () => reload(context))
    )

    context.subscriptions.push(vscode.languages.registerCompletionItemProvider(SELECTOR, {
        async provideCompletionItems(document, position) {
            await ready()
            if (!service) return []
            return guard(() => service.getCompletions(document.uri.fsPath, document.getText(), document.offsetAt(position)).map(item => {
                const completion = new vscode.CompletionItem(item.label, KIND_MAP[item.kind] || vscode.CompletionItemKind.Text)
                completion.detail = item.detail
                completion.insertText = item.insertText || item.label
                return completion
            }), [])
        },
    }, '.'))

    context.subscriptions.push(vscode.languages.registerDefinitionProvider(SELECTOR, {
        async provideDefinition(document, position) {
            await ready()
            if (!service) return null
            return guard(() => toLocations(service.getDefinition(document.uri.fsPath, document.getText(), document.offsetAt(position))), null)
        },
    }))

    context.subscriptions.push(vscode.languages.registerReferenceProvider(SELECTOR, {
        async provideReferences(document, position) {
            await ready()
            if (!service) return []
            return guard(() => toLocations(service.getReferences(document.uri.fsPath, document.getText(), document.offsetAt(position))), [])
        },
    }))

    context.subscriptions.push(vscode.languages.registerHoverProvider(SELECTOR, {
        async provideHover(document, position) {
            await ready()
            if (!service) return null
            return guard(() => {
                const hover = service.getHover(document.uri.fsPath, document.getText(), document.offsetAt(position))
                if (!hover) return null
                const markdown = new vscode.MarkdownString()
                markdown.appendCodeblock(hover.detail || '', 'qml')
                return new vscode.Hover(markdown, toRange(hover.range))
            }, null)
        },
    }))

    context.subscriptions.push(vscode.languages.registerDocumentSymbolProvider(SELECTOR, {
        async provideDocumentSymbols(document) {
            await ready()
            if (!service) return []
            return guard(() => service.getSymbols(document.uri.fsPath, document.getText()).map(symbolOf), [])
        },
    }))
}

function deactivate() {
    if (debounceTimer) clearTimeout(debounceTimer)
    service = null
}

module.exports = { activate, deactivate }
