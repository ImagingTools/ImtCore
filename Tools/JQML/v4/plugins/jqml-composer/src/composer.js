// The composer of schemas: a custom editor of `.jqschema` files (see composer/schema.mjs). A schema is a tree of
// components that can inherit another schema and export properties; the application is generated from schemas
// (composer/generate.mjs, composer/app.mjs). The editor edits the JSON of the schema, so undo, save and the text
// editor work as they always do.

const vscode = require('vscode')
const fs = require('fs')
const path = require('path')
const { pathToFileURL } = require('url')

const VIEW_TYPE = 'jqml4composer.editor'
const EXTENSION = '.jqschema'

// The types that are shown at the top of the palette, in this order.
const COMMON_TYPES = [
    'Item', 'Rectangle', 'Text', 'Image', 'MouseArea', 'Column', 'Row', 'Flow', 'GridLayout', 'ColumnLayout', 'RowLayout',
    'StackLayout', 'Flickable', 'ListView', 'GridView', 'Repeater', 'Loader', 'TextInput', 'TextEdit', 'Canvas', 'Timer', 'ListModel',
]

function nonce() {
    const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'
    let text = ''
    for (let i = 0; i < 32; i++) text += chars[Math.floor(Math.random() * chars.length)]
    return text
}

// The smallest replacement that turns `before` into `after`: the common beginning and end stay untouched, so the
// cursor, the folding and the undo stack of the text editor are not disturbed.
function diffRange(before, after) {
    let start = 0
    const limit = Math.min(before.length, after.length)
    while (start < limit && before.charCodeAt(start) === after.charCodeAt(start)) start++
    let endBefore = before.length
    let endAfter = after.length
    while (endBefore > start && endAfter > start && before.charCodeAt(endBefore - 1) === after.charCodeAt(endAfter - 1)) {
        endBefore--
        endAfter--
    }
    return { start, end: endBefore, text: after.slice(start, endAfter) }
}

function webviewHtml(webview, extensionUri) {
    const css = webview.asWebviewUri(vscode.Uri.joinPath(extensionUri, 'media', 'composer.css'))
    const script = webview.asWebviewUri(vscode.Uri.joinPath(extensionUri, 'media', 'composer.js'))
    const token = nonce()
    const csp = "default-src 'none'; style-src " + webview.cspSource + "; script-src 'nonce-" + token + "' " + webview.cspSource
    return fs.readFileSync(path.join(extensionUri.fsPath, 'media', 'composer.html'), 'utf8')
        .replace('{{csp}}', csp)
        .replace('{{css}}', String(css))
        .replace('{{nonce}}', token)
        .replace('{{js}}', String(script))
}

const toPosix = file => file.split(path.sep).join('/')

function expandVariables(value) {
    if (!value) return value
    const folder = (vscode.workspace.workspaceFolders || [])[0]
    let result = String(value)
    if (folder) result = result.replace(/\$\{workspaceFolder\}/g, folder.uri.fsPath)
    return result.replace(/\$\{env:([^}]+)\}/g, (_, name) => process.env[name] || '')
}

class ComposerHost {
    constructor(context, getService, ready, engineRoot) {
        this.context = context
        this.getService = getService
        this.ready = ready
        this.engineRoot = engineRoot
        this.modules = null
        this.schemaFiles = []
    }

    async load() {
        if (this.modules === null) {
            const base = path.join(this.context.extensionPath, 'src', 'composer')
            const [schema, generate, app] = await Promise.all(['schema.mjs', 'generate.mjs', 'app.mjs'].map(name => import(pathToFileURL(path.join(base, name)).href)))
            this.modules = { schema, generate, app }
        }
        return this.modules
    }

    // The text of a schema file: the open document (with unsaved changes) or the file.
    readSchemaText(file) {
        const open = vscode.workspace.textDocuments.find(document => document.uri.scheme === 'file' && path.resolve(document.uri.fsPath) === file)
        if (open) return open.getText()
        try {
            return fs.readFileSync(file, 'utf8')
        } catch (error) {
            return null
        }
    }

    // resolve(ref, fromKey) -> { key, schema } | null. The key of a schema is the absolute path of its file.
    resolver(schemaModule) {
        return (ref, fromKey) => {
            const file = path.resolve(path.dirname(fromKey), ref)
            const text = this.readSchemaText(file)
            if (text === null) return null
            const parsed = schemaModule.parseSchema(text)
            if (parsed.error) return null
            return { key: file, schema: parsed.schema }
        }
    }

    async refreshSchemaFiles() {
        const found = await vscode.workspace.findFiles('**/*' + EXTENSION, '**/node_modules/**', 500)
        this.schemaFiles = found.map(uri => path.resolve(uri.fsPath))
    }

    // A QML text that has the imports of the schema, so the language service resolves the types the way the generated file will.
    importText(document) {
        let imports = []
        try {
            const text = document.getText()
            const parsed = JSON.parse(text.trim() === '' ? '{}' : text)
            imports = Array.isArray(parsed.imports) ? parsed.imports.filter(item => typeof item === 'string') : ['QtQuick 2.12']
        } catch (error) {
            imports = ['QtQuick 2.12']
        }
        return imports.map(item => 'import ' + item).join('\n') + '\nItem {}\n'
    }

    typesFor(document) {
        const service = this.getService()
        const result = { common: [], builtin: [], project: [], schemas: [] }
        const here = path.resolve(document.uri.fsPath)
        result.schemas = this.schemaFiles.filter(file => file !== here).map(file => ({ name: path.basename(file, EXTENSION), ref: toPosix(path.relative(path.dirname(here), file)) })).sort((a, b) => a.name.localeCompare(b.name))
        if (!service) return result
        const builtin = service.allTypeNames().sort((a, b) => a.localeCompare(b))
        result.common = COMMON_TYPES.filter(name => builtin.includes(name))
        result.builtin = builtin.filter(name => !result.common.includes(name))
        try {
            const ctx = service.context(here.replace(/\.jqschema$/i, '.qml'), '')
            result.project = service.project.visibleTypes(ctx.path, ctx.scan)
                .map(type => ({ name: type.name, file: path.basename(type.path) }))
                .sort((a, b) => a.name.localeCompare(b.name))
        } catch (error) {
            result.project = []
        }
        return result
    }

    // The properties of a type for the inspector: [{ name, typeName }], the group properties expanded.
    membersOf(document, typeName) {
        const service = this.getService()
        if (!service) return []
        try {
            const file = path.resolve(document.uri.fsPath).replace(/\.jqschema$/i, '.qml')
            const ctx = service.context(file, this.importText(document))
            const ref = service.resolveType(typeName, ctx.path, ctx.scan)
            if (ref === null) return []
            const found = service.members(ref)
            const out = []
            for (const member of found.map.values()) {
                if (member.kind !== 'property' || member.name.startsWith('_')) continue
                if (Array.isArray(member.group)) {
                    for (const sub of member.group) out.push({ name: member.name + '.' + sub, typeName: '' })
                } else if (member.typeName !== 'group') {
                    out.push({ name: member.name, typeName: member.typeName || '' })
                }
            }
            return out.sort((a, b) => a.name.localeCompare(b.name))
        } catch (error) {
            return []
        }
    }

    // The properties that an object of a schema has: the ones of the type at the bottom of its chain of extends.
    async membersOfSchema(document, ref) {
        const { schema } = await this.load()
        const key = path.resolve(document.uri.fsPath)
        const resolve = this.resolver(schema)
        const used = resolve(ref, key)
        if (used === null) return []
        try {
            return this.membersOf(document, schema.rootTypeOf(used, resolve))
        } catch (error) {
            return []
        }
    }

    resolveWebviewPanel(document, panel) {
        panel.webview.options = { enableScripts: true, localResourceRoots: [vscode.Uri.joinPath(this.context.extensionUri, 'media')] }
        panel.webview.html = webviewHtml(panel.webview, this.context.extensionUri)
        const session = new ComposerSession(this, document, panel)
        const subscriptions = [
            panel.webview.onDidReceiveMessage(message => session.onMessage(message)),
            vscode.workspace.onDidChangeTextDocument(event => {
                // the schema itself, or a schema that it reads
                if (event.document.uri.scheme === 'file' && event.document.uri.fsPath.toLowerCase().endsWith(EXTENSION)) session.schedulePush()
            }),
        ]
        panel.onDidDispose(() => {
            if (session.timer) clearTimeout(session.timer)
            for (const item of subscriptions) item.dispose()
        })
    }
}

class ComposerSession {
    constructor(host, document, panel) {
        this.host = host
        this.document = document
        this.panel = panel
        this.timer = null
    }

    key() {
        return path.resolve(this.document.uri.fsPath)
    }

    // The schema of the document and what is read through it, sent to the page.
    async push(select) {
        const { schema, generate } = await this.host.load()
        const parsed = schema.parseSchema(this.document.getText())
        const key = this.key()
        const resolve = this.host.resolver(schema)
        let view = null
        let exportsList = []
        let problems = []
        let failure = ''
        if (!parsed.error) {
            try {
                view = schema.viewOf(key, parsed.schema, resolve)
                exportsList = schema.describeExports(key, parsed.schema, resolve)
                problems = generate.validateSchema({ key, schema: parsed.schema }, resolve)
            } catch (error) {
                failure = error.message
            }
        }
        await this.panel.webview.postMessage({
            type: 'state',
            view,
            exports: exportsList,
            extends: parsed.error ? '' : parsed.schema.extends,
            imports: parsed.error ? [] : parsed.schema.imports,
            error: parsed.error || failure || null,
            problems,
            types: this.host.typesFor(this.document),
            select: select === undefined ? null : select,
        })
    }

    schedulePush() {
        if (this.timer) clearTimeout(this.timer)
        this.timer = setTimeout(() => this.push(), 120)
    }

    async replaceText(next) {
        const current = this.document.getText()
        if (next === current) return true
        const range = diffRange(current, next)
        const edit = new vscode.WorkspaceEdit()
        edit.replace(this.document.uri, new vscode.Range(this.document.positionAt(range.start), this.document.positionAt(range.end)), range.text)
        return vscode.workspace.applyEdit(edit)
    }

    async operate(message) {
        const { schema: model } = await this.host.load()
        const parsed = model.parseSchema(this.document.getText())
        if (parsed.error) throw new Error('The schema file is not valid JSON: ' + parsed.error)
        const key = this.key()
        const ctx = { key, resolve: this.host.resolver(model) }
        const current = parsed.schema
        const uid = String(message.uid || '')
        let next = current
        let select = uid
        if (message.op === 'set') {
            next = model.setProperty(current, ctx, uid, String(message.name), String(message.value))
        } else if (message.op === 'setMany') {
            for (const [name, value] of Object.entries(message.props || {})) next = model.setProperty(next, ctx, uid, String(name), String(value))
        } else if (message.op === 'setId') {
            next = model.setId(current, uid, String(message.id))
        } else if (message.op === 'add') {
            const added = model.addNode(current, ctx, uid, message.spec || {}, message.props || {}, typeof message.index === 'number' ? message.index : undefined)
            next = added.schema
            select = added.uid
        } else if (message.op === 'delete') {
            const found = model.findNode(current, uid)
            next = model.deleteNode(current, uid)
            select = found && found.parent ? found.parent.uid : current.root.uid
        } else if (message.op === 'move') {
            next = model.moveNode(current, uid, String(message.parent), typeof message.index === 'number' ? message.index : undefined)
        } else if (message.op === 'addExport') {
            next = model.addExport(current, ctx, uid, String(message.property), message.name ? String(message.name) : undefined, String(message.typeName || ''))
        } else if (message.op === 'removeExport') {
            next = model.removeExport(current, String(message.name))
        } else if (message.op === 'renameExport') {
            next = model.renameExport(current, ctx, String(message.name), String(message.to))
        } else if (message.op === 'setExtends') {
            const result = model.setExtends(current, ctx, String(message.ref || ''))
            next = result.schema
            if (result.dropped.length > 0) vscode.window.showWarningMessage('Removed from the root: ' + result.dropped.join(', ') + ' (the base schema does not export them)')
        } else if (message.op === 'setImports') {
            next = model.setImports(current, Array.isArray(message.imports) ? message.imports : [])
        } else {
            return
        }
        await this.replaceText(model.stringifySchema(next))
        await this.push(select)
    }

    async generateApp() {
        const { schema: model } = await this.host.load()
        const parsed = model.parseSchema(this.document.getText())
        if (parsed.error) throw new Error('The schema file is not valid JSON: ' + parsed.error)
        await this.document.save()
        await this.host.generateFor(this.document)
    }

    async onMessage(message) {
        if (!message || !message.type) return
        try {
            if (message.type === 'ready') {
                await this.host.refreshSchemaFiles()
                await this.push()
            } else if (message.type === 'op') await this.operate(message)
            else if (message.type === 'generate') await this.generateApp()
            else if (message.type === 'members') {
                const members = message.ref ? await this.host.membersOfSchema(this.document, message.ref) : this.host.membersOf(this.document, message.typeName)
                await this.panel.webview.postMessage({ type: 'members', typeName: message.typeName || message.ref, members })
            }
        } catch (error) {
            await this.panel.webview.postMessage({ type: 'failure', message: error.message })
        }
    }
}

// Asks where to put the application and generates it from the schema of the document.
ComposerHost.prototype.generateFor = async function (document) {
    const { schema: model, app } = await this.load()
    const key = path.resolve(document.uri.fsPath)
    const parsed = model.parseSchema(document.getText())
    const name = await vscode.window.showInputBox({ title: 'Application name', value: model.typeNameOfKey(key), prompt: 'The name of the compiled application' })
    if (!name) return
    const folder = await vscode.window.showOpenDialog({ canSelectFiles: false, canSelectFolders: true, canSelectMany: false, openLabel: 'Generate here', title: 'Folder of the application', defaultUri: vscode.Uri.file(path.dirname(key)) })
    if (!folder || folder.length === 0) return
    const config = vscode.workspace.getConfiguration('jqml4composer')
    const configPath = expandVariables(config.get('configPath'))
    const env = config.get('env') || {}
    let result
    try {
        result = await vscode.window.withProgress({ location: vscode.ProgressLocation.Notification, title: 'Generating ' + name }, async () => app.generateApplication({
            entry: { key, schema: parsed.schema },
            resolve: this.resolver(model),
            outDir: folder[0].fsPath,
            name,
            enginePath: this.engineRoot(),
            includes: configPath ? [configPath] : [],
            env,
        }))
    } catch (error) {
        vscode.window.showErrorMessage('Generation failed: ' + error.message)
        return
    }
    const open = result.compiled ? 'Open the page' : 'Open the sources'
    const pick = await vscode.window.showInformationMessage(result.compiled ? 'The application is generated: ' + result.htmlPath : 'The QML is generated (no JQML 4 engine was found to compile it): ' + result.srcDir, open)
    if (pick) await vscode.env.openExternal(vscode.Uri.file(result.compiled ? result.htmlPath : result.srcDir))
}

function registerComposer(context, getService, ready, engineRoot) {
    const host = new ComposerHost(context, getService, ready, engineRoot)
    context.subscriptions.push(vscode.window.registerCustomEditorProvider(VIEW_TYPE, {
        async resolveCustomTextEditor(document, panel) {
            await ready()
            host.resolveWebviewPanel(document, panel)
        },
    }, { webviewOptions: { retainContextWhenHidden: true } }))

    context.subscriptions.push(vscode.commands.registerCommand('jqml4composer.newSchema', async resource => {
        const folder = resource instanceof vscode.Uri ? (fs.existsSync(resource.fsPath) && fs.statSync(resource.fsPath).isDirectory() ? resource.fsPath : path.dirname(resource.fsPath)) : ((vscode.workspace.workspaceFolders || [])[0] || {}).uri && vscode.workspace.workspaceFolders[0].uri.fsPath
        if (!folder) {
            vscode.window.showInformationMessage('Open a folder first.')
            return
        }
        const name = await vscode.window.showInputBox({ title: 'New schema', prompt: 'The name of the schema (it is the name of the QML type that is generated)', value: 'MainPage', validateInput: value => (/^[A-Za-z_][A-Za-z0-9_]*$/.test(value) ? null : 'Letters, digits and _ only') })
        if (!name) return
        const { schema } = await host.load()
        const file = path.join(folder, name + EXTENSION)
        if (fs.existsSync(file)) {
            vscode.window.showErrorMessage(name + EXTENSION + ' already exists.')
            return
        }
        fs.writeFileSync(file, schema.stringifySchema(schema.emptySchema()))
        await vscode.commands.executeCommand('vscode.openWith', vscode.Uri.file(file), VIEW_TYPE)
    }))

    context.subscriptions.push(vscode.commands.registerCommand('jqml4composer.generateApp', async resource => {
        const uri = resource instanceof vscode.Uri ? resource : (vscode.window.activeTextEditor && vscode.window.activeTextEditor.document.uri)
        if (!uri || !uri.fsPath.toLowerCase().endsWith(EXTENSION)) {
            vscode.window.showInformationMessage('Open a ' + EXTENSION + ' schema first.')
            return
        }
        const document = await vscode.workspace.openTextDocument(uri)
        await document.save()
        await host.generateFor(document)
    }))

    context.subscriptions.push(vscode.commands.registerCommand('jqml4composer.openSchemaAsText', async resource => {
        const uri = resource instanceof vscode.Uri ? resource : undefined
        const tab = vscode.window.tabGroups.activeTabGroup.activeTab
        const target = uri || (tab && tab.input && tab.input.uri)
        if (target) await vscode.commands.executeCommand('vscode.openWith', target, 'default')
    }))
}

module.exports = { registerComposer, diffRange }
