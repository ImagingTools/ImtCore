const fs = require('fs')
const path = require('path')
const util = require('util')
const parser = require('./parser')

const KIND = {
    class: 'class',
    property: 'property',
    method: 'method',
    signal: 'signal',
    handler: 'handler',
    keyword: 'keyword',
    module: 'module',
    variable: 'variable',
    enum: 'enum',
    snippet: 'snippet',
}

const QML_KEYWORDS = [
    'import', 'pragma', 'as', 'property', 'signal', 'function', 'alias',
    'readonly', 'required', 'default', 'enum', 'id', 'Singleton',
]

const HEADER_KEYWORDS = ['import', 'pragma']
const ELEMENT_KEYWORDS = ['property', 'signal', 'function', 'readonly', 'required', 'default', 'enum', 'id', 'alias']

const PROPERTY_TYPE_NAMES = [
    'int', 'real', 'double', 'string', 'bool', 'var', 'variant', 'color',
    'list', 'alias', 'date', 'url', 'point', 'rect',
]

const INTERNAL_NAME = /^(JQ|__)|^(AX|AY|AWidth|AHeight)(Changed)?$/

const COMPLETION_NOISE = new Set([
    'hasOwnProperty', 'setParent', '$complete', 'blockSignals', 'signalsBlocked',
    'deleteLater', 'toString', 'valueOf', 'isPrototypeOf', 'propertyIsEnumerable',
    'toLocaleString', 'constructor',
    'model', 'modelData', 'index', 'modelChanged', 'modelDataChanged', 'indexChanged',
])

const JS_GLOBALS = ['parent', 'this', 'console', 'Qt', 'true', 'false', 'null', 'undefined', 'Math']

const ENGINE_MODULES = [
    ['Qt', 'Qt/Qt.js'],
    ['QtQml', 'QtQml/QtQml.js'],
    ['QtQuick', 'QtQuick/QtQuick.js'],
    ['QtTest', 'QtTest/QtTest.js'],
    ['Qt5Compat', 'Qt5Compat/Qt5Compat.js'],
    ['QtWebSockets', 'QtWebSockets/QtWebSockets.js'],
    ['QtPositioning', 'QtPositioning/QtPositioning.js'],
    ['QtLocation', 'QtLocation/QtLocation.js'],
    ['QtWebView', 'QtWebView/QtWebView.js'],
]

const SKIP_STATIC = new Set([
    'meta', 'create', 'isAssignableFrom', 'getDefaultValue', 'cachedComponents',
    'cachedComponent', 'singleton', 'uid', 'handle', 'name', 'length', 'prototype',
    'arguments', 'caller',
])

function ensureEngineGlobals() {
    if (!global.window) {
        global.window = { addEventListener() {} }
    }
}

function normalizePath(filePath) {
    return String(filePath || '').replace(/\\/g, '/')
}

function typeNameOf(node) {
    if (!node) return ''
    if (typeof node === 'string') return node
    if (Array.isArray(node)) {
        if (node[0] === 'dot') return typeNameOf(node[1]) + '.' + typeNameOf(node[2])
        if (typeof node[1] === 'string' && node[0] !== 'qmlelem') return node[1]
    }
    return String(node)
}

function propNameOf(node) {
    return typeNameOf(node)
}

function infoOf(node) {
    if (!node) return null
    if (node.info) return Array.isArray(node.info) ? node.info[0] : node.info
    return null
}

function rangeFromInfo(info, length) {
    if (!info || info.line == null) {
        return {
            start: { line: 0, character: 0 },
            end: { line: 0, character: 1 },
        }
    }
    const line = info.line
    const character = info.col || 0
    const size = length != null ? length : Math.max(1, (info.endpos || 0) - (info.pos || 0) || (info.value ? String(info.value).length : 1))
    return {
        start: { line, character },
        end: { line, character: character + size },
    }
}

function rangeFromName(info, name) {
    return rangeFromInfo(info, String(name || '').length || 1)
}

function envKeysLongestFirst(env) {
    return Object.keys(env || {}).sort((a, b) => b.length - a.length)
}

function envFill(source, env) {
    const vars = env || process.env
    let result = String(source)
    for (const key of envKeysLongestFirst(vars)) {
        if (vars[key] == null) continue
        result = result.replaceAll('${' + key + '}', String(vars[key]).replaceAll('\\', '\\\\').trim())
    }
    return result
}

function envFillPath(filePath, env) {
    const vars = env || process.env
    let result = String(filePath || '')
    for (const key of envKeysLongestFirst(vars)) {
        if (vars[key] == null) continue
        result = result.replaceAll('${' + key + '}', String(vars[key]).trim())
    }
    return result
}

function isAbsolutePath(filePath) {
    if (!filePath) return false
    if (path.isAbsolute(filePath)) return true
    if (/^[A-Za-z]:[\\/]/.test(filePath)) return true
    if (filePath.slice(0, 2) === '\\\\') return true
    return false
}

function resolveConfigRef(baseDirPath, filePath, env) {
    const filled = envFillPath(filePath, env)
    if (isAbsolutePath(filled)) return path.normalize(filled)
    if (filled.indexOf('${') >= 0) return path.normalize(filled)
    return path.resolve(baseDirPath, filled)
}

function pathExists(filePath) {
    try {
        return !!(filePath && fs.existsSync(filePath))
    } catch {
        return false
    }
}

function parentDirs(startPath) {
    const result = []
    let dir = path.resolve(startPath)
    while (dir) {
        result.push(dir)
        const parent = path.dirname(dir)
        if (parent === dir) break
        dir = parent
    }
    return result
}

function detectImtCoreRoot(dir) {
    return pathExists(path.join(dir, 'Qml', 'web', 'imtcore.json'))
}

function detectProLifeRoot(dir) {
    return pathExists(path.join(dir, 'Qml', 'Include', 'prolifeqml', 'prolife.json'))
}

function firstAuxTarget(root) {
    if (!root) return ''
    const aux = path.join(root, 'AuxInclude')
    if (!pathExists(aux)) return ''
    try {
        const names = fs.readdirSync(aux).filter(name => {
            try {
                return fs.statSync(path.join(aux, name)).isDirectory()
            } catch {
                return false
            }
        })
        return names[0] || ''
    } catch {
        return ''
    }
}

function inferProjectEnv(seedPaths) {
    const inferred = {}
    const seeds = (seedPaths || []).filter(Boolean)

    for (const seed of seeds) {
        for (const dir of parentDirs(seed)) {
            if (!inferred.IMTCOREDIR && detectImtCoreRoot(dir)) inferred.IMTCOREDIR = dir
            if (!inferred.PROLIFEDIR && detectProLifeRoot(dir)) inferred.PROLIFEDIR = dir
        }
    }

    if (inferred.PROLIFEDIR && !inferred.IMTCOREDIR) {
        const sibling = path.join(path.dirname(inferred.PROLIFEDIR), 'ImtCore')
        if (detectImtCoreRoot(sibling)) inferred.IMTCOREDIR = sibling
    }
    if (inferred.IMTCOREDIR && !inferred.PROLIFEDIR) {
        const sibling = path.join(path.dirname(inferred.IMTCOREDIR), 'ProLife')
        if (detectProLifeRoot(sibling)) inferred.PROLIFEDIR = sibling
    }

    if (inferred.IMTCOREDIR && !inferred.IMTCOREDIR_BUILD) inferred.IMTCOREDIR_BUILD = inferred.IMTCOREDIR
    if (!inferred.TARGETNAME) {
        inferred.TARGETNAME = firstAuxTarget(inferred.IMTCOREDIR_BUILD) || firstAuxTarget(inferred.IMTCOREDIR) || firstAuxTarget(inferred.PROLIFEDIR) || ''
        if (!inferred.TARGETNAME) delete inferred.TARGETNAME
    }
    return inferred
}

const KNOWN_CPP_MODULES = new Set([
    'com.imtcore.imtqml',
])

function isClassWithMeta(value) {
    return typeof value === 'function' && value.meta && typeof value.meta === 'object'
}

function isEngineInstance(value) {
    return !!(value && typeof value === 'object' && (value.__proxy || value.__self || util.types.isProxy(value)))
}

function typeClassOf(value) {
    if (isClassWithMeta(value)) return value
    if (value && value.constructor && isClassWithMeta(value.constructor)) return value.constructor
    return null
}

function isNamespace(value) {
    return !!(value && typeof value === 'object' && !Array.isArray(value) && !typeClassOf(value) && !isEngineInstance(value))
}

function classLooksLikeElement(name, cls) {
    if (!name) return false
    if (name[0] !== name[0].toUpperCase()) return false
    if (name === 'Signal' || name === 'Method' || name === 'Property' || name === 'SpecialSignal' || name === 'SpecialProperty') return false
    return typeof cls.isAssignableFrom === 'function'
}

function primitiveTypeName(cls) {
    if (!cls || !cls.name) return 'var'
    const map = {
        Int: 'int',
        Real: 'real',
        Double: 'double',
        String: 'string',
        Bool: 'bool',
        LinkedBool: 'bool',
        Var: 'var',
        Variant: 'variant',
        Color: 'color',
        List: 'list',
        Date: 'date',
        Geometry: 'real',
        Alias: 'alias',
        PropertyAuto: 'var',
        ProxyProperty: 'var',
    }
    return map[cls.name] || cls.name
}

function isDunderName(name) {
    return String(name || '').split(/[./]/).some(part => part.slice(0, 2) === '__')
}

function isInternalMember(name) {
    if (isDunderName(name)) return true
    if (INTERNAL_NAME.test(name)) return true
    const signal = handlerToSignal(name)
    if (signal && (isDunderName(signal) || INTERNAL_NAME.test(signal))) return true
    return false
}

function isCompletionNoise(member) {
    const name = member && member.name
    if (!name) return true
    if (isDunderName(name) || isInternalMember(name)) return true
    if (name.indexOf('.') >= 0) return true
    if (name[0] === '$') return true
    if (COMPLETION_NOISE.has(name)) return true
    if (COMPLETION_NOISE.has(handlerToSignal(name))) return true
    return false
}

function escapeRegExp(value) {
    return String(value).replace(/[.*+?^${}()|[\]\\]/g, '\\$&')
}

function detectEngineRoot(enginePath) {
    if (enginePath && pathExists(path.join(enginePath, 'QtQuick', 'QtQuick.js'))) return enginePath
    const fromLang = path.resolve(__dirname, '..')
    if (pathExists(path.join(fromLang, 'QtQuick', 'QtQuick.js'))) return fromLang
    return enginePath || ''
}

function engineFileOfClass(cls, engineRoot) {
    if (!cls || !engineRoot) return ''
    try {
        for (const file of Object.keys(require.cache)) {
            const cached = require.cache[file]
            if (cached && cached.exports === cls) {
                return normalizePath(path.relative(engineRoot, cached.filename))
            }
        }
    } catch {
        return ''
    }
    return ''
}

function guessEngineFile(engineRoot, moduleName, typeName) {
    const parts = (moduleName || '').split('.').filter(Boolean)
    const candidates = [
        path.join(engineRoot, ...parts, typeName + '.js'),
        path.join(engineRoot, ...parts, typeName, typeName + '.js'),
        path.join(engineRoot, ...parts, typeName, 'index.js'),
    ]
    for (const candidate of candidates) {
        if (pathExists(candidate)) return normalizePath(path.relative(engineRoot, candidate))
    }
    return parts.length ? normalizePath(path.join(...parts, typeName + '.js')) : (typeName + '.js')
}

function locationInText(text, filePath, name) {
    if (!text || !name) return null
    const escaped = escapeRegExp(name)
    const patterns = [
        new RegExp('^[ \\t]*[\'"`]?' + escaped + '[\'"`]?[ \\t]*:', 'm'),
        new RegExp('^[ \\t]*(?:static[ \\t]+)?(?:async[ \\t]+)?function[ \\t]+' + escaped + '\\b', 'm'),
        new RegExp('^[ \\t]*' + escaped + '[ \\t]*\\(', 'm'),
        new RegExp('\\bsignal\\s+' + escaped + '\\b'),
        new RegExp('\\bfunction\\s+' + escaped + '\\b'),
        new RegExp('\\bproperty\\s+(?:(?:readonly|required|default)\\s+)*(?:alias\\s+|[A-Za-z_][\\w.<>]*\\s+)' + escaped + '\\b'),
        new RegExp('\\bid\\s*:\\s*' + escaped + '\\b'),
        new RegExp('\\b(?:class|id)\\s+' + escaped + '\\b'),
    ]
    for (const re of patterns) {
        const match = re.exec(text)
        if (!match) continue
        let index = match.index + match[0].lastIndexOf(name)
        if (index < match.index) index = match.index
        const start = offsetToPosition(text, index)
        return {
            filePath: filePath ? normalizePath(filePath) : '',
            range: {
                start,
                end: { line: start.line, character: start.character + String(name).length },
            },
        }
    }
    return null
}

function locationInFile(filePath, name) {
    if (!filePath || !pathExists(filePath) || !name) return null
    const loc = locationInText(readFile(filePath), normalizePath(path.resolve(filePath)), name)
    return loc
}

function idDeclarationLocation(text, filePath, info, idName) {
    if (!filePath || !idName) return null
    if (info && info.pos != null) {
        const window = text.slice(info.pos, info.pos + 96)
        const match = window.match(new RegExp('id\\s*:\\s*(' + escapeRegExp(idName) + ')\\b'))
        if (match) {
            const index = info.pos + match.index + match[0].length - idName.length
            const start = offsetToPosition(text, index)
            return {
                filePath,
                range: {
                    start,
                    end: { line: start.line, character: start.character + idName.length },
                },
            }
        }
    }
    return locationFromInfo(text, filePath, info, idName)
        || locationInText(text, filePath, idName)
}

function locationFromInfo(text, filePath, info, name) {
    if (!filePath || !name) return null
    if (info && info.pos != null) {
        const from = info.pos
        const window = text.slice(from, from + 160)
        const idx = window.indexOf(name)
        if (idx >= 0) {
            const start = offsetToPosition(text, from + idx)
            return {
                filePath,
                range: {
                    start,
                    end: { line: start.line, character: start.character + String(name).length },
                },
            }
        }
    }
    if (info && info.line != null) {
        return { filePath, range: rangeFromName(info, name) }
    }
    return null
}

function tokenAtOffset(text, offset) {
    const clamped = Math.max(0, Math.min(offset, text.length))
    let start = clamped
    let end = clamped
    if (start > 0 && !/[A-Za-z0-9_]/.test(text[start]) && /[A-Za-z0-9_]/.test(text[start - 1])) start--
    while (start > 0 && /[A-Za-z0-9_]/.test(text[start - 1])) start--
    while (end < text.length && /[A-Za-z0-9_]/.test(text[end])) end++
    if (start >= end) return null
    let pathStart = start
    while (pathStart > 0 && text[pathStart - 1] === '.') {
        let prev = pathStart - 1
        while (prev > 0 && /[A-Za-z0-9_]/.test(text[prev - 1])) prev--
        if (prev >= pathStart - 1 || !/[A-Za-z_]/.test(text[prev])) break
        pathStart = prev
    }
    const full = text.slice(pathStart, end)
    const parts = full.split('.')
    const name = parts.pop()
    return {
        name,
        path: parts.filter(Boolean),
        start,
        end,
        range: {
            start: offsetToPosition(text, start),
            end: offsetToPosition(text, end),
        },
    }
}

function positionInRange(pos, range) {
    if (!pos || !range || !range.start || !range.end) return false
    if (pos.line < range.start.line || pos.line > range.end.line) return false
    if (pos.line === range.start.line && pos.character < range.start.character) return false
    if (pos.line === range.end.line && pos.character > range.end.character) return false
    return true
}

function handlerToSignal(handlerName) {
    const last = String(handlerName).split('.').pop()
    if (!last || last.slice(0, 2) !== 'on' || last.length < 3) return ''
    if (last[2] !== last[2].toUpperCase()) return ''
    return last[2].toLowerCase() + last.slice(3)
}

function isHandlerName(name) {
    return !!handlerToSignal(name)
}

function readFile(filePath) {
    return fs.readFileSync(filePath, { encoding: 'utf8', flag: 'r' })
}

function matchingBrace(text, openIndex) {
    let depth = 0
    let inStr = false
    let quote = ''
    let escape = false
    let lineComment = false
    let blockComment = false

    for (let i = openIndex; i < text.length; i++) {
        const ch = text[i]
        const next = text[i + 1]

        if (lineComment) {
            if (ch === '\n') lineComment = false
            continue
        }
        if (blockComment) {
            if (ch === '*' && next === '/') {
                blockComment = false
                i++
            }
            continue
        }
        if (inStr) {
            if (escape) {
                escape = false
                continue
            }
            if (ch === '\\') {
                escape = true
                continue
            }
            if (ch === quote) inStr = false
            continue
        }
        if (ch === '/' && next === '/') {
            lineComment = true
            i++
            continue
        }
        if (ch === '/' && next === '*') {
            blockComment = true
            i++
            continue
        }
        if (ch === '"' || ch === "'" || ch === '`') {
            inStr = true
            quote = ch
            continue
        }
        if (ch === '{') {
            depth++
        } else if (ch === '}') {
            depth--
            if (depth === 0) return i
        }
    }
    return text.length
}

function braceRangeAfter(text, fromPos) {
    const open = text.indexOf('{', fromPos)
    if (open < 0) return { start: fromPos, end: fromPos }
    return { start: open, end: matchingBrace(text, open) }
}

function skipStringOrComment(text, i) {
    const ch = text[i]
    const next = text[i + 1]
    if (ch === '/' && next === '/') {
        const nl = text.indexOf('\n', i)
        return nl < 0 ? text.length : nl + 1
    }
    if (ch === '/' && next === '*') {
        const end = text.indexOf('*/', i + 2)
        return end < 0 ? text.length : end + 2
    }
    if (ch === '"' || ch === "'" || ch === '`') {
        const quote = ch
        i++
        while (i < text.length) {
            if (text[i] === '\\') {
                i += 2
                continue
            }
            if (text[i] === quote) return i + 1
            i++
        }
        return text.length
    }
    return i
}

function innermostBraceStart(text, offset, from, to) {
    const stack = from != null ? [from] : []
    const limit = Math.min(offset, to != null ? to : text.length)
    let i = (from != null ? from + 1 : 0)
    while (i < limit) {
        const next = skipStringOrComment(text, i)
        if (next !== i) {
            i = next
            continue
        }
        if (text[i] === '{') stack.push(i)
        else if (text[i] === '}' && stack.length) stack.pop()
        i++
    }
    return stack.length ? stack[stack.length - 1] : from
}

function looksLikeQmlTypeToken(token) {
    return /^[A-Z][\w.]*$/.test(String(token || ''))
}

function isBindingValueOnLine(text, offset) {
    const lineStart = text.lastIndexOf('\n', offset - 1) + 1
    const line = text.slice(lineStart, offset)
    if (/^\s*on[A-Z]\w*\s*:/.test(line)) return true
    if (/^\s*(?:readonly|required|default\s+)?property\b[^:\n]*:\s*/.test(line)) return true
    if (/^\s*[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*\s*:\s*$/.test(line)) return true
    if (/^\s*[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*\s*:\s+\S/.test(line) && !/^\s*[A-Z][\w.]*\s*\{/.test(line)) return true
    return false
}

function valueRangeAfterColon(text, from, limit) {
    const endLimit = limit != null ? limit : Math.min(text.length, from + 400)
    let colon = -1
    for (let i = from; i < endLimit; i++) {
        const next = skipStringOrComment(text, i)
        if (next !== i) {
            i = next - 1
            continue
        }
        if (text[i] === '\n' || text[i] === '{') break
        if (text[i] === ':') {
            colon = i
            break
        }
    }
    if (colon < 0) return null
    let i = colon + 1
    while (i < text.length && (text[i] === ' ' || text[i] === '\t')) i++
    if (i < text.length && text[i] === '{') {
        return { start: i, end: matchingBrace(text, i) }
    }
    const rest = text.slice(i, i + 80)
    if (looksLikeQmlTypeToken(rest.trim().split(/\s+/)[0]) && rest.replace(/^[A-Za-z_][\w.]*/, '').trim().charAt(0) === '{') {
        return null
    }
    let end = i
    let depth = 0
    while (end < text.length) {
        const next = skipStringOrComment(text, end)
        if (next !== end) {
            end = next
            continue
        }
        const ch = text[end]
        if (ch === '(' || ch === '[' || ch === '{') depth++
        else if ((ch === ')' || ch === ']' || ch === '}') && depth > 0) depth--
        else if (depth === 0 && (ch === '\n' || ch === ';')) break
        end++
    }
    return { start: colon + 1, end }
}

function wordPrefix(lineBefore) {
    const match = lineBefore.match(/([A-Za-z_]\w*(?:\.[A-Za-z_]\w*)*\.?)$/)
    if (!match) return { path: [], prefix: '', token: '' }
    const token = match[1]
    const parts = token.split('.')
    const endsWithDot = token.endsWith('.')
    if (endsWithDot) parts.pop()
    const prefix = endsWithDot ? '' : (parts.pop() || '')
    return { path: parts.filter(Boolean), prefix, token }
}

function offsetToPosition(text, offset) {
    let line = 0
    let last = 0
    const clamped = Math.max(0, Math.min(offset, text.length))
    for (let i = 0; i < clamped; i++) {
        if (text[i] === '\n') {
            line++
            last = i + 1
        }
    }
    return { line, character: clamped - last }
}

function positionToOffset(text, position) {
    const lines = text.split('\n')
    let offset = 0
    const line = Math.max(0, position.line || 0)
    for (let i = 0; i < line && i < lines.length; i++) {
        offset += lines[i].length + 1
    }
    return offset + (position.character || 0)
}

class TypeRecord {
    constructor(data) {
        this.name = data.name
        this.qualifiedName = data.qualifiedName || data.name
        this.module = data.module || ''
        this.kind = data.kind || 'element'
        this.baseName = data.baseName || ''
        this.filePath = data.filePath || ''
        this.singleton = !!data.singleton
        this.members = data.members || []
        this.statics = data.statics || []
        this.context = data.context || null
        this.engine = !!data.engine
    }
}

function memberMap(members) {
    const map = new Map()
    for (const member of members) {
        if (!map.has(member.name)) map.set(member.name, member)
    }
    return map
}

function collectPrototypeMethods(cls) {
    const methods = []
    const seen = new Set()
    let proto = cls && cls.prototype
    while (proto && proto !== Object.prototype) {
        for (const name of Object.getOwnPropertyNames(proto)) {
            if (seen.has(name)) continue
            seen.add(name)
            if (name === 'constructor') continue
            if (name.startsWith('__') || name.startsWith('SLOT_') || name.startsWith('PROXY_') || name.startsWith('$')) continue
            if (COMPLETION_NOISE.has(name)) continue
            if (typeof proto[name] === 'function') {
                methods.push({
                    name,
                    kind: KIND.method,
                    detail: 'method',
                })
            }
        }
        proto = Object.getPrototypeOf(proto)
    }
    return methods
}

function collectStatics(cls) {
    const result = []
    if (!cls) return result
    for (const name of Object.getOwnPropertyNames(cls)) {
        if (SKIP_STATIC.has(name)) continue
        if (name.startsWith('_')) continue
        const value = cls[name]
        if (isClassWithMeta(value)) continue
        if (typeof value === 'function' && value.meta) continue
        result.push({
            name,
            kind: KIND.enum,
            detail: 'enum',
        })
    }
    return result
}

function buildEngineType(name, moduleName, cls, helpers, engineRoot) {
    const members = []
    const meta = cls.meta || {}
    for (const key of Object.keys(meta)) {
        if (isDunderName(key)) continue
        const node = meta[key]
        if (!node || !node.type) continue
        const typeCls = node.typeTarget || node.type
        let kind = KIND.property
        let detail = primitiveTypeName(typeCls)
        if (typeof node.type.isAssignableFrom === 'function') {
            if (node.type.isAssignableFrom(helpers.Signal)) {
                kind = KIND.signal
                const args = Array.isArray(node.args) ? node.args : []
                detail = 'signal ' + key + '(' + args.join(', ') + ')'
            } else if (node.type.isAssignableFrom(helpers.Method)) {
                kind = KIND.method
                detail = 'method'
            } else if (node.type.isAssignableFrom(helpers.GroupProperty)) {
                kind = KIND.property
                detail = typeCls && typeCls.name ? typeCls.name : 'group'
                members.push({
                    name: key,
                    kind,
                    detail,
                    group: true,
                    typeName: typeCls && typeCls.name ? typeCls.name : key,
                    groupTypeName: typeCls && typeCls.name ? typeCls.name : key,
                })
                continue
            }
        }
        members.push({
            name: key,
            kind,
            detail,
            readonly: !!(node.modifiers && node.modifiers.readonly),
            typeName: primitiveTypeName(typeCls),
        })
    }

    for (const method of collectPrototypeMethods(cls)) {
        if (!members.some(member => member.name === method.name)) members.push(method)
    }

    const attached = typeof cls.isAssignableFrom === 'function' && cls.isAssignableFrom(helpers.GroupProperty)

    let baseName = ''
    const proto = Object.getPrototypeOf(cls)
    if (proto && proto !== Function.prototype && proto !== Object && isClassWithMeta(proto) && proto.name && proto.name !== name) {
        baseName = proto.name
    }

    return new TypeRecord({
        name,
        qualifiedName: moduleName ? moduleName + '.' + name : name,
        module: moduleName,
        kind: attached ? 'group' : (classLooksLikeElement(name, cls) ? 'element' : 'type'),
        baseName,
        members,
        statics: collectStatics(cls),
        engine: true,
        filePath: engineFileOfClass(cls, engineRoot) || (engineRoot ? guessEngineFile(engineRoot, moduleName, name) : ''),
    })
}

function walkEngineModule(mod, moduleName, out, helpers, visited, engineRoot) {
    if (!mod) return
    const seen = visited || new WeakSet()
    if (typeof mod === 'object') {
        if (seen.has(mod)) return
        seen.add(mod)
    }

    const typeCls = typeClassOf(mod)
    if (typeCls) {
        const shortName = moduleName.split('.').pop()
        const record = buildEngineType(shortName, moduleName.slice(0, Math.max(0, moduleName.lastIndexOf('.'))), typeCls, helpers, engineRoot)
        if (!isClassWithMeta(mod)) record.singleton = true
        out.types.push(record)
        return
    }
    if (!isNamespace(mod)) return

    out.modules.add(moduleName)

    for (const key of Object.keys(mod)) {
        if (/_v\d/.test(key)) continue
        if (isDunderName(key)) continue
        const value = mod[key]
        const nextName = moduleName ? moduleName + '.' + key : key
        const childCls = typeClassOf(value)
        if (childCls) {
            const record = buildEngineType(key, moduleName, childCls, helpers, engineRoot)
            if (!isClassWithMeta(value)) record.singleton = true
            out.types.push(record)
        } else if (typeof value === 'function') {
            out.qtMembers.push({
                name: nextName.startsWith('Qt.') ? nextName.slice(3) : key,
                kind: KIND.method,
                detail: 'Qt.' + (nextName.startsWith('Qt.') ? nextName.slice(3) : key),
                module: 'Qt',
            })
        } else if (isNamespace(value)) {
            walkEngineModule(value, nextName, out, helpers, seen, engineRoot)
        } else if (moduleName === 'Qt' || moduleName.startsWith('Qt.')) {
            if (isEngineInstance(value) || util.types.isProxy(value)) continue
            out.qtMembers.push({
                name: nextName.startsWith('Qt.') ? nextName.slice(3) : key,
                kind: KIND.enum,
                detail: 'Qt.' + (nextName.startsWith('Qt.') ? nextName.slice(3) : key),
                module: 'Qt',
            })
        }
    }
}

function loadEngine(enginePath) {
    ensureEngineGlobals()
    const abs = path.resolve(enginePath)
    const QtQml = require(path.join(abs, 'QtQml/QtQml.js'))
    const helpers = {
        Signal: QtQml.Signal,
        Method: QtQml.Method,
        GroupProperty: QtQml.GroupProperty,
    }

    const out = {
        types: [],
        modules: new Set(),
        qtMembers: [],
    }

    for (const [name, rel] of ENGINE_MODULES) {
        const mod = require(path.join(abs, rel))
        walkEngineModule(mod, name, out, helpers, null, abs)
    }

    const componentMembers = [
        { name: 'completed', kind: KIND.signal, detail: 'signal completed()' },
        { name: 'destruction', kind: KIND.signal, detail: 'signal destruction()' },
        { name: 'onCompleted', kind: KIND.handler, detail: 'Component.onCompleted' },
        { name: 'onDestruction', kind: KIND.handler, detail: 'Component.onDestruction' },
    ]
    out.types.push(new TypeRecord({
        name: 'ComponentAttached',
        qualifiedName: 'Component',
        module: 'QtQml',
        kind: 'group',
        engine: true,
        filePath: pathExists(path.join(abs, 'QtQml', 'Component.js')) ? 'QtQml/Component.js' : '',
        members: componentMembers,
    }))

    return {
        types: out.types,
        modules: [...out.modules].sort(),
        qtMembers: out.qtMembers,
        helpers,
    }
}

function serializeEngine(engine) {
    const serializeMember = (member) => {
        const copy = Object.assign({}, member)
        if (copy.groupType && copy.groupType.name && !copy.groupTypeName) {
            copy.groupTypeName = copy.groupType.name
        }
        delete copy.groupType
        return copy
    }
    return {
        modules: engine.modules || [],
        qtMembers: engine.qtMembers || [],
        types: (engine.types || []).map((type) => ({
            name: type.name,
            qualifiedName: type.qualifiedName,
            module: type.module,
            kind: type.kind,
            baseName: type.baseName || '',
            singleton: !!type.singleton,
            engine: true,
            filePath: type.filePath || '',
            members: (type.members || []).map(serializeMember),
            statics: type.statics || [],
        })),
    }
}

function hydrateEngine(data) {
    return {
        types: (data.types || []).map((type) => new TypeRecord(type)),
        modules: data.modules || [],
        qtMembers: data.qtMembers || [],
        helpers: {},
    }
}

function loadEngineIndex(filePath) {
    return hydrateEngine(JSON.parse(fs.readFileSync(filePath, { encoding: 'utf8' })))
}

function engineLooksLive(enginePath) {
    if (!enginePath) return false
    return fs.existsSync(path.join(enginePath, 'QtQuick', 'QtQuick.js'))
}

function parseQmldir(dirPath, text) {
    const types = []
    let moduleName = ''
    for (const raw of String(text).split(/\r?\n/)) {
        const line = raw.trim().replace(/[ ]+/g, ' ')
        if (!line || line.startsWith('#')) continue
        const params = line.split(' ')
        if (params.length === 2 && params[0] === 'module') {
            moduleName = params[1]
            continue
        }
        if (params[0] === 'internal' || params[0] === 'plugin' || params[0] === 'classname' || params[0] === 'typeinfo' || params[0] === 'depends' || params[0] === 'prefer') {
            continue
        }
        if (params[0] === 'singleton' && params.length >= 4) {
            types.push({
                singleton: true,
                name: params[1],
                version: parseFloat(params[2]),
                filePath: normalizePath(path.resolve(dirPath, params[3])),
            })
            continue
        }
        if (params.length >= 3) {
            types.push({
                singleton: false,
                name: params[0],
                version: parseFloat(params[1]),
                filePath: normalizePath(path.resolve(dirPath, params[2])),
            })
        }
    }
    return { moduleName, types }
}

function extractImportsFromText(text) {
    const imports = []
    const re = /^\s*import\s+([A-Za-z_][\w.]*)(?:\s+(\d+(?:\.\d+)?))?(?:\s+as\s+([A-Za-z_]\w*))?/gm
    let match
    while ((match = re.exec(text))) {
        imports.push({
            path: match[1],
            version: match[2] ? parseFloat(match[2]) : undefined,
            as: match[3] || '',
            range: {
                start: { line: 0, character: 0 },
                end: { line: 0, character: match[1].length },
            },
        })
    }
    return imports
}

function extractImports(meta) {
    const imports = []
    if (!meta) return imports
    for (const item of meta) {
        if (item && item[0] === 'qmlimport') {
            imports.push({
                path: item[1],
                version: item[2],
                as: item[3] || '',
                range: rangeFromInfo(infoOf(item)),
            })
        }
    }
    return imports
}

function extractPragmas(meta) {
    const pragmas = []
    if (!meta) return pragmas
    for (const item of meta) {
        if (item && item[0] === 'qmlpragma') pragmas.push(item[1])
    }
    return pragmas
}

function walkQmlTree(node, parent, document, text) {
    if (!node || !Array.isArray(node)) return

    if (node[0] === 'qmldefaultprop') {
        walkQmlTree(node[1], parent, document, text)
        return
    }

    if (node[0] === 'qmlelem') {
        const typeName = typeNameOf(node[1])
        const info = infoOf(node)
        const start = info && info.pos != null ? info.pos : 0
        const braces = braceRangeAfter(text, start)
        const element = {
            typeName,
            id: '',
            parent,
            children: [],
            properties: [],
            signals: [],
            methods: [],
            assigns: [],
            objects: [],
            range: {
                start: offsetToPosition(text, braces.start),
                end: offsetToPosition(text, braces.end),
            },
            offsetStart: start,
            offsetEnd: braces.end,
            braceOffset: braces.start,
            nameRange: rangeFromName(info, typeName.split('.').shift()),
            info,
        }
        if (parent) parent.children.push(element)
        else document.root = element
        document.elements.push(element)

        const body = node[3] || []
        for (const child of body) walkQmlTree(child, element, document, text)
        return
    }

    if (!parent) return

    if (node[0] === 'qmlprop') {
        const name = propNameOf(node[1])
        const info = infoOf(node)
        if (name === 'id') {
            const valueNode = node[2] && node[2][1]
            const idName = valueNode && valueNode[0] === 'name' ? valueNode[1] : String(node[3] || '').trim().split(/\s+/)[0]
            parent.id = idName
            if (idName) {
                document.ids[idName] = parent
                parent.idRange = idDeclarationLocation(text, document.filePath, info, idName)
            }
        } else {
            const from = info && info.pos != null ? info.pos : parent.offsetStart
            const script = valueRangeAfterColon(text, from, parent.offsetEnd)
            parent.assigns.push({
                name,
                info,
                range: rangeFromName(info, String(name).split('.')[0]),
                handler: isHandlerName(name),
                scriptStart: script && script.start,
                scriptEnd: script && script.end,
            })
        }
        const inner = node[2] && node[2][1]
        if (inner && inner[0] === 'qmlelem') walkQmlTree(inner, parent, document, text)
        return
    }

    if (node[0] === 'qmlpropdef') {
        const name = node[2]
        const type = node[3]
        const modifiers = {
            default: !!(node[1] && node[1][0]),
            required: !!(node[1] && node[1][1]),
            readonly: !!(node[1] && node[1][2]),
        }
        const loc = locationFromInfo(text, document.filePath, infoOf(node), name)
            || locationInText(text, document.filePath, name)
        parent.properties.push({
            name,
            type,
            modifiers,
            kind: KIND.property,
            detail: 'property ' + type + ' ' + name,
            info: infoOf(node),
            typeName: type,
            filePath: document.filePath,
            range: loc && loc.range,
        })
        parent.signals.push({
            name: name + 'Changed',
            kind: KIND.signal,
            detail: 'signal ' + name + 'Changed()',
            filePath: document.filePath,
            range: loc && loc.range,
        })
        const inner = node[4] && node[4][1]
        if (inner && inner[0] === 'qmlelem') walkQmlTree(inner, parent, document, text)
        else {
            const from = (loc && loc.range && parent.offsetStart != null) ? (infoOf(node) && infoOf(node).pos) || parent.offsetStart : parent.offsetStart
            const script = valueRangeAfterColon(text, from, parent.offsetEnd)
            if (script) {
                parent.properties[parent.properties.length - 1].scriptStart = script.start
                parent.properties[parent.properties.length - 1].scriptEnd = script.end
            }
        }
        return
    }

    if (node[0] === 'qmlaliasdef') {
        const name = node[1]
        const loc = locationFromInfo(text, document.filePath, infoOf(node), name)
            || locationInText(text, document.filePath, name)
        parent.properties.push({
            name,
            type: 'alias',
            kind: KIND.property,
            detail: 'property alias ' + name,
            info: infoOf(node),
            typeName: 'alias',
            aliasId: node[2] || '',
            aliasPath: node[3] || '',
            filePath: document.filePath,
            range: loc && loc.range,
        })
        parent.signals.push({
            name: name + 'Changed',
            kind: KIND.signal,
            detail: 'signal ' + name + 'Changed()',
            filePath: document.filePath,
            range: loc && loc.range,
        })
        return
    }

    if (node[0] === 'qmlsignaldef') {
        const args = (node[2] || []).map(arg => (arg.type ? arg.type + ' ' : '') + arg.name).join(', ')
        const loc = locationFromInfo(text, document.filePath, infoOf(node), node[1])
            || locationInText(text, document.filePath, node[1])
        parent.signals.push({
            name: node[1],
            args: node[2] || [],
            kind: KIND.signal,
            detail: 'signal ' + node[1] + '(' + args + ')',
            filePath: document.filePath,
            range: loc && loc.range,
        })
        return
    }

    if (node[0] === 'qmlmethod') {
        const args = ((node[2] && node[2][2]) || []).join(', ')
        const loc = locationFromInfo(text, document.filePath, infoOf(node), node[1])
            || locationInText(text, document.filePath, node[1])
        const source = typeof node[3] === 'string' ? node[3] : ''
        let from = parent.offsetStart || 0
        if (source) {
            const idx = text.indexOf(source, parent.offsetStart || 0)
            if (idx >= 0) from = idx
        }
        const braces = braceRangeAfter(text, from)
        parent.methods.push({
            name: node[1],
            args: (node[2] && node[2][2]) || [],
            kind: KIND.method,
            detail: 'function ' + node[1] + '(' + args + ')',
            filePath: document.filePath,
            range: loc && loc.range,
            offsetStart: braces.start,
            offsetEnd: braces.end,
        })
        return
    }

    if (node[0] === 'qmlobj') {
        const name = typeNameOf(node[1])
        const group = {
            name,
            assigns: [],
            info: infoOf(node),
        }
        parent.objects.push(group)
        parent.assigns.push({
            name,
            info: infoOf(node),
            range: rangeFromName(infoOf(node), name),
            group: true,
        })
        for (const child of node[2] || []) {
            if (child && child[0] === 'qmlprop') {
                group.assigns.push({
                    name: propNameOf(child[1]),
                    info: infoOf(child),
                    range: rangeFromName(infoOf(child), propNameOf(child[1])),
                })
            }
        }
        return
    }
}

function parseQmlDocument(filePath, text) {
    const document = {
        filePath: normalizePath(filePath),
        text,
        ast: null,
        imports: [],
        pragmas: [],
        root: null,
        elements: [],
        ids: {},
        diagnostics: [],
        singleton: false,
        className: path.basename(filePath, '.qml'),
        directory: normalizePath(path.dirname(filePath)),
        moduleName: '',
    }

    try {
        parser.parse.nowParsingFile = filePath
        document.ast = parser.parse(text)
    } catch (error) {
        const line = Math.max(0, (error.line || 1) - 1)
        const character = Math.max(0, (error.col || 1) - 1)
        document.diagnostics.push({
            message: (error.message || String(error)).split('\n')[0],
            severity: 'error',
            range: {
                start: { line, character },
                end: { line, character: character + 1 },
            },
        })
        document.imports = extractImportsFromText(text)
        return document
    }

    document.imports = extractImports(document.ast[1])
    document.pragmas = extractPragmas(document.ast[3])
    document.singleton = document.pragmas.indexOf('Singleton') >= 0
    walkQmlTree(document.ast[2], null, document, text)
    return document
}

function stabilizeQmlText(text) {
    return String(text || '').replace(/([A-Za-z_]\w*)\.(?=\s*[;,)\]}\r]|$)/gm, '$1._')
}

function ownMembersOf(element) {
    const members = []
    for (const prop of element.properties || []) members.push(prop)
    for (const signal of element.signals || []) members.push(signal)
    for (const method of element.methods || []) members.push(method)
    members.push({ name: 'id', kind: KIND.property, detail: 'id' })
    if (element.id) {
        members.push({
            name: element.id,
            kind: KIND.variable,
            detail: 'id ' + element.id,
            filePath: element.idRange && element.idRange.filePath,
            range: element.idRange && element.idRange.range,
        })
    }
    return members
}

class LanguageService {
    constructor(options) {
        this.enginePath = options && options.enginePath ? options.enginePath : ''
        this.engineIndexPath = options && options.engineIndexPath ? options.engineIndexPath : ''
        this.hideInternal = options && options.hideInternal === false ? false : true
        this.engineLoadError = null
        this.engine = { types: [], modules: [], qtMembers: [], helpers: {} }
        this.engineTypes = new Map()
        this.moduleTypes = new Map()
        this.qmlDocuments = new Map()
        this.localTypes = new Map()
        this.moduleByPath = new Map()
        this.configDirs = []
        this.workspaceFolders = []
        this.pathEnv = Object.assign({}, process.env)
        this.extraEnv = (options && options.extraEnv) || {}
        this.engineSourceRoot = detectEngineRoot(this.enginePath)

        try {
            if (engineLooksLive(this.enginePath)) {
                this.engine = loadEngine(this.enginePath)
            } else {
                const bundledIndex = this.engineIndexPath || path.join(__dirname, 'engine-index.json')
                if (fs.existsSync(bundledIndex)) {
                    this.engine = loadEngineIndex(bundledIndex)
                } else if (this.enginePath) {
                    this.engine = loadEngine(this.enginePath)
                } else {
                    throw new Error('JQML engine types were not found')
                }
            }
            this.applyEngine(this.engine)
        } catch (error) {
            this.engineLoadError = error
        }
    }

    applyEngine(engine) {
        this.engine = engine
        this.engineTypes = new Map()
        this.moduleTypes = new Map()
        for (const type of engine.types || []) {
            this.engineTypes.set(type.qualifiedName, type)
            const list = this.moduleTypes.get(type.module) || []
            list.push(type)
            this.moduleTypes.set(type.module, list)
        }
    }

    setWorkspaceFolders(folders) {
        this.workspaceFolders = (folders || []).map(folder => normalizePath(folder))
    }

    setExtraEnv(extraEnv) {
        this.extraEnv = extraEnv || {}
    }

    rebuildPathEnv(seedPaths) {
        this.pathEnv = Object.assign({}, process.env)
        const inferred = inferProjectEnv(seedPaths)
        for (const key of Object.keys(inferred)) {
            if (!this.pathEnv[key]) this.pathEnv[key] = inferred[key]
        }
        Object.assign(this.pathEnv, this.extraEnv)
        if (!this.engineSourceRoot || !pathExists(path.join(this.engineSourceRoot, 'QtQuick', 'QtQuick.js'))) {
            const fromCore = this.pathEnv.IMTCOREDIR
                ? path.join(this.pathEnv.IMTCOREDIR, 'Tools', 'JQML', 'v3')
                : ''
            this.engineSourceRoot = detectEngineRoot(this.engineSourceRoot || this.enginePath || fromCore)
        }
        return this.pathEnv
    }

    loadConfig(configPath) {
        this.configDirs = []
        if (!configPath) return
        this.rebuildPathEnv([configPath].concat(this.workspaceFolders))
        const abs = resolveConfigRef(process.cwd(), envFillPath(configPath, this.pathEnv), this.pathEnv)
        if (!fs.existsSync(abs)) return
        const configDirPath = path.dirname(abs)
        const dirs = []
        const env = this.pathEnv

        const includeFiles = (sourceFile, baseDirPath) => {
            if (!sourceFile.includes) return
            for (const filePath of sourceFile.includes) {
                const absoluteConfigPath = resolveConfigRef(baseDirPath, filePath, env)
                if (!fs.existsSync(absoluteConfigPath)) continue
                const includeConfigDirPath = path.dirname(absoluteConfigPath)
                let file
                try {
                    file = JSON.parse(envFill(readFile(absoluteConfigPath), env))
                } catch {
                    continue
                }
                includeFiles(file, includeConfigDirPath)
                for (const dirPath of file.dirs || []) {
                    const resolved = resolveConfigRef(includeConfigDirPath, dirPath, env)
                    if (resolved.indexOf('${') >= 0) continue
                    dirs.unshift(resolved)
                }
            }
        }

        let config
        try {
            config = JSON.parse(envFill(readFile(abs), env))
        } catch {
            return
        }
        includeFiles(config, configDirPath)
        for (const dirPath of config.dirs || []) {
            const resolved = resolveConfigRef(configDirPath, dirPath, env)
            if (resolved.indexOf('${') >= 0) continue
            dirs.push(resolved)
        }
        if (fs.existsSync(path.join(configDirPath, 'qmldir'))) {
            dirs.push(configDirPath)
        }
        this.configDirs = dirs.map(normalizePath)

        for (const dir of this.configDirs) {
            const qmldirPath = path.join(dir, 'qmldir')
            if (fs.existsSync(qmldirPath)) this.indexQmldir(qmldirPath)
        }
    }

    indexQmldir(filePath) {
        const abs = normalizePath(path.resolve(filePath))
        if (!fs.existsSync(abs)) return
        const dirPath = path.dirname(abs)
        const parsed = parseQmldir(dirPath, readFile(abs))
        if (!parsed.moduleName) return
        this.moduleByPath.set(normalizePath(dirPath), parsed.moduleName)
        for (const item of parsed.types) {
            this.moduleByPath.set(item.filePath, parsed.moduleName)
            this.localTypes.set(parsed.moduleName + '.' + item.name, {
                name: item.name,
                module: parsed.moduleName,
                filePath: item.filePath,
                singleton: item.singleton,
                version: item.version,
            })
        }
    }

    indexQmlFile(filePath, text) {
        const abs = normalizePath(path.resolve(filePath))
        const source = text != null ? text : (fs.existsSync(abs) ? readFile(abs) : '')
        const document = parseQmlDocument(abs, source)
        const moduleName = this.moduleByPath.get(abs) || this.moduleByPath.get(document.directory) || ''
        document.moduleName = moduleName
        const local = moduleName ? this.localTypes.get(moduleName + '.' + document.className) : null
        if (local && local.singleton) document.singleton = true
        if (!document.diagnostics.length || !this.qmlDocuments.has(abs)) {
            this.qmlDocuments.set(abs, document)
        }
        return document
    }

    removeFile(filePath) {
        this.qmlDocuments.delete(normalizePath(path.resolve(filePath)))
    }

    getDocument(filePath, text) {
        const abs = normalizePath(path.resolve(filePath))
        if (text != null) return this.indexQmlFile(abs, text)
        if (this.qmlDocuments.has(abs)) return this.qmlDocuments.get(abs)
        if (fs.existsSync(abs)) return this.indexQmlFile(abs)
        return parseQmlDocument(abs, '')
    }

    analysisDocument(filePath, text) {
        const parsed = this.getDocument(filePath, text)
        if (!parsed.diagnostics.length) return parsed
        const source = text != null ? text : parsed.text || ''
        if (source) {
            const patched = stabilizeQmlText(source)
            if (patched !== source) {
                const retry = parseQmlDocument(parsed.filePath || filePath, patched)
                const moduleName = this.moduleByPath.get(retry.filePath) || this.moduleByPath.get(retry.directory) || parsed.moduleName || ''
                retry.moduleName = moduleName
                const local = moduleName ? this.localTypes.get(moduleName + '.' + retry.className) : null
                if (local && local.singleton) retry.singleton = true
                if (!retry.diagnostics.length) return retry
            }
        }
        const cached = this.qmlDocuments.get(normalizePath(path.resolve(filePath)))
        if (cached && cached.root && !cached.diagnostics.length) return cached
        return parsed
    }

    findInnermostElement(document, offset) {
        let found = document.root
        for (const element of document.elements) {
            if (offset >= element.offsetStart && offset <= element.offsetEnd) {
                if (!found || (element.offsetEnd - element.offsetStart) <= (found.offsetEnd - found.offsetStart)) {
                    found = element
                }
            }
        }
        return found
    }

    findType(qualifiedName) {
        if (!qualifiedName) return null
        if (this.engineTypes.has(qualifiedName)) return this.engineTypes.get(qualifiedName)
        const local = this.localTypes.get(qualifiedName)
        if (local) {
            const doc = this.getDocument(local.filePath)
            return this.documentToType(doc, local.name, local.module)
        }
        return null
    }

    documentToType(document, name, moduleName) {
        if (!document || !document.root) {
            return new TypeRecord({
                name: name || (document && document.className) || '',
                module: moduleName || '',
                members: [],
                filePath: document ? document.filePath : '',
            })
        }
        const baseName = document.root.typeName
        const qualifiedName = moduleName ? moduleName + '.' + (name || document.className) : (name || document.className)
        const local = this.localTypes.get(qualifiedName)
        return new TypeRecord({
            name: name || document.className,
            qualifiedName,
            module: moduleName || document.moduleName || '',
            baseName,
            filePath: document.filePath,
            singleton: document.singleton || !!(local && local.singleton),
            members: ownMembersOf(document.root),
            context: document,
        })
    }

    sameDirectoryTypes(document) {
        const result = []
        const dir = document.directory
        for (const other of this.qmlDocuments.values()) {
            if (other.directory === dir && other.className) {
                result.push(this.documentToType(other, other.className, other.moduleName))
            }
        }
        return result
    }

    importedModules(document) {
        const modules = ['QtQml', 'QtQml.Models']
        for (const item of document.imports) {
            if (item.path && item.path.indexOf('.js') < 0) modules.push(item.path)
        }
        if (document.moduleName) modules.push(document.moduleName)
        return modules
    }

    resolveImportedName(document, name) {
        for (let i = document.imports.length - 1; i >= 0; i--) {
            const item = document.imports[i]
            if (item.as && item.as === name) {
                return { module: item.path, alias: item.as }
            }
        }
        return null
    }

    lookupInModule(moduleName, typeName) {
        if (!moduleName || !typeName) return null
        const qualified = moduleName + '.' + typeName
        const found = this.findType(qualified)
        if (found) return found
        const list = this.moduleTypes.get(moduleName) || []
        return list.find(type => type.name === typeName) || null
    }

    resolveType(typeName, document) {
        if (!typeName) return null
        if (this.engineTypes.has(typeName)) return this.engineTypes.get(typeName)

        const parts = String(typeName).split('.')
        if (parts.length > 1) {
            const alias = this.resolveImportedName(document, parts[0])
            if (alias) {
                return this.lookupInModule(alias.module + (parts.length > 2 ? '.' + parts.slice(1, -1).join('.') : ''), parts[parts.length - 1])
                    || this.lookupInModule(alias.module, parts.slice(1).join('.'))
                    || this.findType(alias.module + '.' + parts.slice(1).join('.'))
            }
            const found = this.findType(typeName)
            if (found) return found
            return this.lookupInModule(parts.slice(0, -1).join('.'), parts[parts.length - 1])
        }

        for (const local of this.sameDirectoryTypes(document)) {
            if (local.name === typeName) return local
        }

        if (document.moduleName) {
            const sameModule = this.lookupInModule(document.moduleName, typeName)
            if (sameModule) return sameModule
        }

        const qtqml = this.lookupInModule('QtQml', typeName) || this.lookupInModule('QtQml.Models', typeName)
        if (qtqml) return qtqml

        for (let i = document.imports.length - 1; i >= 0; i--) {
            const item = document.imports[i]
            if (item.as) continue
            const found = this.lookupInModule(item.path, typeName) || this.findType(item.path + '.' + typeName)
            if (found) return found
        }

        return this.lookupTypeByName(typeName, document)
    }

    lookupTypeByName(typeName, document) {
        if (!typeName) return null
        const imported = document ? this.importedModules(document) : []
        const locals = [...this.localTypes.values()].filter(entry => entry.name === typeName)
        const preferred = locals.find(entry => imported.indexOf(entry.module) >= 0)
            || (document && document.moduleName && locals.find(entry => entry.module === document.moduleName))
            || (locals.length ? locals[0] : null)
        if (preferred) return this.findType(preferred.module + '.' + preferred.name)
        return [...this.engineTypes.values()].find(type => type.name === typeName) || null
    }

    isAliasMember(member) {
        return !!(member && (member.type === 'alias' || member.typeName === 'alias') && member.aliasId)
    }

    aliasDeclaringDocument(member, ownerType, document) {
        if (member && member.filePath) {
            const abs = normalizePath(path.resolve(member.filePath))
            if (this.qmlDocuments.has(abs)) return this.qmlDocuments.get(abs)
            if (fs.existsSync(abs)) return this.getDocument(abs)
        }
        if (ownerType && ownerType.context) return ownerType.context
        return document || null
    }

    typeOfMember(member, document, ownerType) {
        if (!member) return null
        const ctx = (ownerType && ownerType.context) || document
        if (member.group) return this.groupType(member, ctx)
        if (this.isAliasMember(member)) {
            return this.resolveAliasType(member, ctx, ownerType)
        }
        const typeName = member.typeName || member.type
        if (!typeName || typeName === 'alias') return null
        if (PROPERTY_TYPE_NAMES.indexOf(String(typeName).toLowerCase()) >= 0) return null
        return this.resolveType(typeName, ctx) || this.lookupTypeByName(typeName, ctx)
    }

    resolveAliasType(member, document, ownerType) {
        const ctx = this.aliasDeclaringDocument(member, ownerType, document)
        if (!ctx) return null
        const idName = member.aliasId
        const rest = String(member.aliasPath || '').split('.').filter(Boolean)
        const target = ctx.ids && ctx.ids[idName]
        let type = target ? this.resolveType(target.typeName, ctx) : this.lookupTypeByName(idName, ctx)
        let members = target ? memberMap(this.membersOfElement(target, ctx)) : memberMap(this.collectMembers(type, ctx))
        for (const part of rest) {
            const found = members.get(part)
            if (!found) return type
            type = this.typeOfMember(found, ctx, type) || this.groupType(found, ctx)
            members = memberMap(this.collectMembers(type, (type && type.context) || ctx))
        }
        return type
    }

    nestedMembers(head, document, ownerType) {
        const resolved = this.typeOfMember(head, document, ownerType) || this.groupType(head, document)
        if (!resolved) return new Map()
        const ctx = resolved.context || document
        return memberMap(this.collectMembers(resolved, ctx).concat(resolved.members || []))
    }

    aliasTargetLocations(member, ownerType, document) {
        const ctx = this.aliasDeclaringDocument(member, ownerType, document)
        if (!ctx || !this.isAliasMember(member)) return []
        const locs = []
        const target = ctx.ids && ctx.ids[member.aliasId]
        if (target && target.idRange) locs.push(target.idRange)
        const rest = String(member.aliasPath || '').split('.').filter(Boolean)
        let type = target ? this.resolveType(target.typeName, ctx) : null
        let members = target ? memberMap(this.membersOfElement(target, ctx)) : memberMap()
        for (const part of rest) {
            const found = members.get(part)
            if (!found) break
            if (found.filePath && found.range) locs.push({ filePath: found.filePath, range: found.range })
            else {
                const loc = this.memberOrigin(part, type, ctx)
                if (loc) locs.push(loc)
            }
            type = this.typeOfMember(found, ctx, type) || this.groupType(found, ctx)
            members = memberMap(this.collectMembers(type, (type && type.context) || ctx))
        }
        return locs
    }

    collectMembers(type, document, visited) {
        const seen = visited || new Set()
        if (!type || seen.has(type.qualifiedName || type.name)) return []
        seen.add(type.qualifiedName || type.name)
        const map = memberMap(type.members || [])
        if (type.baseName) {
            const base = this.resolveType(type.baseName, type.context || document)
            for (const member of this.collectMembers(base, type.context || document, seen)) {
                if (!map.has(member.name)) map.set(member.name, member)
            }
        }
        return [...map.values()]
    }

    membersOfElement(element, document) {
        if (!element) return []
        const map = memberMap(ownMembersOf(element))
        const type = this.resolveType(element.typeName, document) || this.lookupTypeByName(element.typeName, document)
        for (const member of this.collectMembers(type, (type && type.context) || document)) {
            if (!map.has(member.name)) map.set(member.name, member)
        }
        map.set('parent', { name: 'parent', kind: KIND.variable, detail: 'parent' })
        map.set('Component', { name: 'Component', kind: KIND.property, detail: 'attached Component', group: true, typeName: 'Component' })
        return [...map.values()]
    }

    findElementById(document, name) {
        if (!document || !name) return null
        if (document.ids && document.ids[name]) return document.ids[name]
        for (const element of document.elements || []) {
            if (element.id === name) return element
        }
        return null
    }

    membersOfTypeAccess(type, document) {
        if (!type) return []
        const ctx = type.context || document
        return this.collectMembers(type, ctx).concat(type.statics || [])
    }

    idLocation(element, document) {
        if (!element) return null
        if (element.idRange && element.idRange.filePath && element.idRange.range) return element.idRange
        const filePath = (document && document.filePath) || (element.idRange && element.idRange.filePath) || ''
        const text = document && document.text
        if (text && element.id && filePath) {
            const match = text.match(new RegExp('\\bid\\s*:\\s*' + escapeRegExp(element.id) + '\\b'))
            if (match) {
                const index = match.index + match[0].length - element.id.length
                const start = offsetToPosition(text, index)
                return {
                    filePath: normalizePath(filePath),
                    range: {
                        start,
                        end: { line: start.line, character: start.character + element.id.length },
                    },
                }
            }
        }
        if (element.nameRange && filePath) {
            return { filePath: normalizePath(filePath), range: element.nameRange }
        }
        return null
    }

    groupType(member, document) {
        if (!member) return null
        const groupName = (member.groupType && member.groupType.name) || member.groupTypeName || member.typeName
        if (groupName && String(groupName).toLowerCase() !== 'alias') {
            return this.engineTypes.get('QtQml.' + groupName)
                || this.engineTypes.get('QtQuick.Layouts.' + groupName)
                || this.engineTypes.get('QtQuick.' + groupName)
                || this.findType(groupName)
                || [...this.engineTypes.values()].find(type => type.name === groupName)
                || this.resolveType(groupName, document)
        }
        return this.resolveType(member.name, document)
    }

    availableTypes(document) {
        const result = new Map()
        for (const local of this.sameDirectoryTypes(document)) result.set(local.name, local)

        for (const moduleName of this.importedModules(document)) {
            const list = this.moduleTypes.get(moduleName) || []
            for (const type of list) {
                if (type.kind === 'element' || type.kind === 'group') result.set(type.name, type)
            }
            for (const meta of this.localTypes.values()) {
                if (meta.module === moduleName) {
                    const doc = this.getDocument(meta.filePath)
                    result.set(meta.name, this.documentToType(doc, meta.name, moduleName))
                }
            }
        }

        for (const type of this.engine.types) {
            if (type.module === 'QtQml' || type.module === 'QtQml.Models') {
                if (type.kind === 'element' || type.kind === 'type') result.set(type.name, type)
            }
        }
        return [...result.values()]
    }

    getDiagnostics(filePath, text) {
        const document = this.getDocument(filePath, text)
        const diagnostics = document.diagnostics.slice()
        if (diagnostics.length) return diagnostics

        for (const item of document.imports) {
            if (!item.path || item.path.indexOf('.js') >= 0) continue
            const known = this.engine.modules.indexOf(item.path) >= 0
                || KNOWN_CPP_MODULES.has(item.path)
                || [...this.localTypes.values()].some(entry => entry.module === item.path)
                || this.moduleTypes.has(item.path)
            if (!known) {
                diagnostics.push({
                    message: item.path + ' is not found',
                    severity: 'error',
                    range: item.range,
                })
            }
        }

        for (const element of document.elements) {
            const type = this.resolveType(element.typeName, document)
            if (!type) {
                diagnostics.push({
                    message: element.typeName + ' is not found',
                    severity: 'error',
                    range: element.nameRange,
                })
                continue
            }
            if (element.typeName === 'ListElement') continue

            const members = memberMap(this.membersOfElement(element, document))
            for (const assign of element.assigns) {
                const parts = String(assign.name).split('.')
                if (assign.handler) {
                    const signalName = handlerToSignal(assign.name)
                    const attached = parts.length > 1 ? parts[0] : ''
                    if (attached === 'Component' && (signalName === 'completed' || signalName === 'destruction')) continue
                    if (parts.length > 1) {
                        const group = members.get(parts[0]) || this.resolveType(parts[0], document)
                        const groupMembers = group ? this.nestedMembers(group, document, type) : new Map()
                        const ok = [...groupMembers.values()].some(member => member.name === signalName || member.name === parts[parts.length - 1] || handlerToSignal(parts[parts.length - 1]) === member.name)
                        if (!ok && group && !this.isAliasMember(group)) {
                            diagnostics.push({
                                message: assign.name + ' is not found',
                                severity: 'warning',
                                range: assign.range,
                            })
                        }
                        continue
                    }
                    if (!members.has(signalName) && !members.has(assign.name)) {
                        diagnostics.push({
                            message: assign.name + ' is not found',
                            severity: 'warning',
                            range: assign.range,
                        })
                    }
                    continue
                }

                if (parts.length === 1) {
                    if (!members.has(parts[0]) && parts[0] !== 'id') {
                        diagnostics.push({
                            message: parts[0] + ' is not found',
                            severity: 'warning',
                            range: assign.range,
                        })
                    }
                    continue
                }

                const head = members.get(parts[0]) || this.resolveType(parts[0], document)
                if (!head) {
                    diagnostics.push({
                        message: parts[0] + ' is not found',
                        severity: 'warning',
                        range: assign.range,
                    })
                    continue
                }
                const groupMembers = this.nestedMembers(head, document, type)
                const tail = parts[1]
                if (tail && !groupMembers.has(tail) && !isHandlerName(assign.name)) {
                    if (this.isAliasMember(head) && !groupMembers.size) continue
                    diagnostics.push({
                        message: assign.name + ' is not found',
                        severity: 'warning',
                        range: assign.range,
                    })
                }
            }

            for (const group of element.objects) {
                const head = members.get(group.name) || this.resolveType(group.name, document)
                if (!head) {
                    diagnostics.push({
                        message: group.name + ' is not found',
                        severity: 'warning',
                        range: rangeFromName(group.info, group.name),
                    })
                    continue
                }
                const groupMembers = this.nestedMembers(head, document, type)
                if (!groupMembers.size && this.isAliasMember(head)) continue
                for (const assign of group.assigns) {
                    if (!groupMembers.has(assign.name)) {
                        diagnostics.push({
                            message: group.name + '.' + assign.name + ' is not found',
                            severity: 'warning',
                            range: assign.range,
                        })
                    }
                }
            }
        }

        return diagnostics
    }

    filterMembers(members, prefix, includeInternal) {
        const lower = (prefix || '').toLowerCase()
        const result = []
        const seen = new Set()
        for (const member of members) {
            if (!member || !member.name) continue
            if (isCompletionNoise(member)) continue
            if (!includeInternal && this.hideInternal && isInternalMember(member.name)) continue
            if (lower && String(member.name).toLowerCase().indexOf(lower) !== 0) continue
            if (seen.has(member.name)) continue
            seen.add(member.name)
            result.push(member)
        }
        return result
    }

    completionItem(member, extra) {
        const item = Object.assign({
            label: member.name,
            kind: member.kind || KIND.property,
            detail: member.detail || '',
            insertText: member.insertText || member.name,
            sortText: member.sortText || member.name,
        }, extra || {})
        if (!item.sortText) item.sortText = item.label
        return item
    }

    completionElement(document, offset) {
        let found = null
        for (const element of document.elements) {
            const start = element.braceOffset != null ? element.braceOffset : element.offsetStart
            const end = element.offsetEnd
            if (offset > start && offset <= end) {
                if (!found || (end - start) <= (found.offsetEnd - (found.braceOffset != null ? found.braceOffset : found.offsetStart))) {
                    found = element
                }
            }
        }
        return found
    }

    isHeaderPosition(document, offset) {
        if (!document.root) return true
        const brace = document.root.braceOffset != null ? document.root.braceOffset : document.root.offsetStart
        return offset <= brace
    }

    isScriptPosition(text, offset, element) {
        if (!element) return false
        for (const method of element.methods || []) {
            if (method.offsetStart != null && offset > method.offsetStart && offset <= method.offsetEnd) return true
        }
        for (const assign of element.assigns || []) {
            if (assign.scriptStart != null && offset >= assign.scriptStart && offset <= assign.scriptEnd) {
                if (assign.handler || assign.name && isHandlerName(assign.name)) return true
                if (assign.group) continue
                return true
            }
        }
        for (const prop of element.properties || []) {
            if (prop.scriptStart != null && offset >= prop.scriptStart && offset <= prop.scriptEnd) return true
        }
        if (isBindingValueOnLine(text, offset)) return true
        const brace = innermostBraceStart(text, offset, element.braceOffset, element.offsetEnd)
        if (brace == null || brace === element.braceOffset) return false
        for (const child of element.children || []) {
            if (child.braceOffset === brace) return false
        }
        const from = Math.max(element.braceOffset || 0, brace - 120)
        const header = text.slice(from, brace)
        const lastLine = header.split('\n').pop() || ''
        if (/^\s*[a-z_]\w*\s*$/.test(lastLine)) return false
        return true
    }

    groupNameAtBrace(text, brace, element) {
        if (!element || brace == null || brace === element.braceOffset) return ''
        const from = Math.max(element.braceOffset || 0, brace - 80)
        const lastLine = text.slice(from, brace).split('\n').pop() || ''
        const match = lastLine.match(/^\s*([a-z_]\w*)\s*$/)
        return match ? match[1] : ''
    }

    detectCompletion(document, text, offset, accessPath) {
        if (accessPath && accessPath.length) {
            return { kind: 'member', element: this.completionElement(document, offset) || this.findInnermostElement(document, offset), path: accessPath }
        }
        const element = this.completionElement(document, offset)
        if (!element) return { kind: 'header', element: null, path: [] }
        const group = this.groupNameAtBrace(text, innermostBraceStart(text, offset, element.braceOffset, element.offsetEnd), element)
        if (group) return { kind: 'member', element, path: [group] }
        if (this.isScriptPosition(text, offset, element)) return { kind: 'script', element, path: [] }
        return { kind: 'element', element, path: [] }
    }

    typesForAlias(document, aliasName) {
        const alias = this.resolveImportedName(document, aliasName)
        if (!alias) return []
        const result = []
        const list = this.moduleTypes.get(alias.module) || []
        for (const type of list) result.push(type)
        for (const meta of this.localTypes.values()) {
            if (meta.module === alias.module) {
                result.push(this.documentToType(this.getDocument(meta.filePath), meta.name, meta.module))
            }
        }
        return result
    }

    resolveAccess(pathParts, document, element) {
        if (!pathParts.length) return { members: this.membersOfElement(element, document), type: this.resolveType(element && element.typeName, document) }
        let currentMembers = memberMap(this.membersOfElement(element, document))
        let currentType = this.resolveType(element && element.typeName, document)

        for (let i = 0; i < pathParts.length; i++) {
            const part = pathParts[i]
            if (part === 'Qt' && i === 0) {
                return {
                    members: this.engine.qtMembers.map(item => ({
                        name: item.name.split('.')[0],
                        kind: item.kind,
                        detail: item.detail,
                    })),
                }
            }
            if (i === 0 && this.resolveImportedName(document, part)) {
                return { members: this.typesForAlias(document, part).map(type => ({
                    name: type.name,
                    kind: KIND.class,
                    detail: type.qualifiedName,
                })) }
            }
            if (i === 0 && (part === 'this' || part === 'self')) {
                continue
            }
            if (i === 0 && this.findElementById(document, part)) {
                const target = this.findElementById(document, part)
                currentType = this.resolveType(target.typeName, document) || this.lookupTypeByName(target.typeName, document)
                currentMembers = memberMap(this.membersOfElement(target, document))
                continue
            }
            if (i === 0 && part === 'parent' && element && element.parent) {
                currentType = this.resolveType(element.parent.typeName, document)
                currentMembers = memberMap(this.membersOfElement(element.parent, document))
                continue
            }
            if (i === 0 && !currentMembers.has(part)) {
                let ancestor = element && element.parent
                while (ancestor && !currentMembers.has(part)) {
                    const ancestorMembers = memberMap(this.membersOfElement(ancestor, document))
                    if (ancestorMembers.has(part)) {
                        currentMembers = ancestorMembers
                        currentType = this.resolveType(ancestor.typeName, document)
                        break
                    }
                    ancestor = ancestor.parent
                }
                if (!currentMembers.has(part) && document.root) {
                    const rootMembers = memberMap(this.membersOfElement(document.root, document))
                    if (rootMembers.has(part)) {
                        currentMembers = rootMembers
                        currentType = this.resolveType(document.root.typeName, document)
                    }
                }
            }
            if (i === 0 && !currentMembers.has(part)) {
                const asType = this.resolveType(part, document) || this.lookupTypeByName(part, document)
                if (asType) {
                    const typeMembers = this.membersOfTypeAccess(asType, document)
                    if (typeMembers.length) {
                        currentMembers = memberMap(typeMembers)
                        currentType = asType
                        continue
                    }
                }
            }
            const member = currentMembers.get(part)
            if (!member) return { members: [], type: currentType }
            const nextType = this.typeOfMember(member, document, currentType) || this.groupType(member, document)
            if (nextType) {
                currentType = nextType
                const ctx = nextType.context || document
                currentMembers = memberMap(this.collectMembers(nextType, ctx).concat(nextType.members || []))
                continue
            }
            if (member.typeName || member.type) {
                currentType = this.resolveType(member.typeName || member.type, (currentType && currentType.context) || document)
                currentMembers = memberMap(this.collectMembers(currentType, document))
                continue
            }
            return { members: [], type: currentType }
        }
        return { members: [...currentMembers.values()], type: currentType }
    }

    getCompletions(filePath, text, offset) {
        const document = this.analysisDocument(filePath, text)
        const before = text.slice(0, offset)
        const lineStart = before.lastIndexOf('\n') + 1
        const lineBefore = before.slice(lineStart)
        const { path: accessPath, prefix } = wordPrefix(lineBefore)
        const items = []
        const add = (member, extra) => {
            for (const item of this.filterMembers([member], extra && extra.forcePrefix != null ? extra.forcePrefix : prefix)) {
                items.push(this.completionItem(item, extra))
            }
        }
        const addMembers = (element, withHandlers) => {
            if (!element) return
            for (const member of this.membersOfElement(element, document)) {
                if (isCompletionNoise(member)) continue
                if (member.kind === KIND.variable) continue
                add(member, { sortText: '0_' + member.name })
                if (withHandlers && member.kind === KIND.signal) {
                    const handler = 'on' + member.name[0].toUpperCase() + member.name.slice(1)
                    add({ name: handler, kind: KIND.handler, detail: 'handler for ' + member.name, sortText: '0_' + handler }, { forcePrefix: prefix })
                }
            }
        }
        const addTypes = () => {
            for (const type of this.availableTypes(document)) {
                add({ name: type.name, kind: KIND.class, detail: type.qualifiedName, sortText: '1_' + type.name })
            }
        }

        if (/^\s*import\s+[\w.]*$/.test(lineBefore)) {
            const modules = new Set(this.engine.modules.concat([...this.localTypes.values()].map(item => item.module)))
            for (const name of modules) add({ name, kind: KIND.module, detail: 'module' })
            return uniqueCompletions(items)
        }

        if (/^\s*(?:(?:readonly|required|default)\s+)*property\s+[\w.]*$/.test(lineBefore)) {
            for (const name of PROPERTY_TYPE_NAMES) add({ name, kind: KIND.class, detail: 'type' })
            for (const type of this.availableTypes(document)) add({ name: type.name, kind: KIND.class, detail: type.qualifiedName })
            return uniqueCompletions(items)
        }

        if (/^\s*pragma\s+\w*$/.test(lineBefore)) {
            add({ name: 'Singleton', kind: KIND.keyword, detail: 'pragma Singleton' })
            return items
        }

        const ctx = this.detectCompletion(document, text, offset, accessPath)

        if (ctx.kind === 'member') {
            const element = ctx.element || this.findInnermostElement(document, offset)
            const resolved = this.resolveAccess(ctx.path, document, element)
            const wantHandlers = prefix.slice(0, 2).toLowerCase() === 'on'
            for (const member of resolved.members || []) {
                if (member.name === 'id' || member.kind === KIND.variable) continue
                if (isCompletionNoise(member)) continue
                add(member, { sortText: '0_' + member.name })
                if (wantHandlers && member.kind === KIND.signal) {
                    const handler = 'on' + member.name[0].toUpperCase() + member.name.slice(1)
                    items.push(this.completionItem({
                        name: handler,
                        kind: KIND.handler,
                        detail: 'handler for ' + member.name,
                        sortText: '0_' + handler,
                    }))
                }
            }
            return uniqueCompletions(items)
        }

        if (ctx.kind === 'header') {
            for (const word of HEADER_KEYWORDS) add({ name: word, kind: KIND.keyword, detail: 'keyword', sortText: '0_' + word })
            addTypes()
            return uniqueCompletions(items)
        }

        if (ctx.kind === 'script') {
            addMembers(ctx.element, false)
            for (const idName of Object.keys(document.ids)) {
                add({ name: idName, kind: KIND.variable, detail: 'id', sortText: '0_' + idName })
            }
            for (const word of JS_GLOBALS) add({ name: word, kind: KIND.variable, detail: 'builtin', sortText: '2_' + word })
            return uniqueCompletions(items)
        }

        addMembers(ctx.element, true)
        for (const word of ELEMENT_KEYWORDS) add({ name: word, kind: KIND.keyword, detail: 'keyword', sortText: '2_' + word })
        addTypes()
        return uniqueCompletions(items)
    }

    engineRoots() {
        const roots = []
        const add = (dir) => {
            if (dir && pathExists(path.join(dir, 'QtQuick', 'QtQuick.js')) && roots.indexOf(dir) < 0) roots.push(dir)
        }
        add(this.enginePath)
        add(this.engineSourceRoot)
        add(detectEngineRoot(''))
        if (this.pathEnv && this.pathEnv.IMTCOREDIR) {
            add(path.join(this.pathEnv.IMTCOREDIR, 'Tools', 'JQML', 'v3'))
        }
        return roots
    }

    resolveEngineFile(type) {
        if (!type) return ''
        const rel = type.filePath || ''
        if (rel && pathExists(rel)) return path.resolve(rel)
        const roots = this.engineRoots()
        if (rel) {
            for (const root of roots) {
                const abs = path.join(root, rel)
                if (pathExists(abs)) return abs
            }
        }
        const moduleParts = (type.module || '').split('.').filter(Boolean)
        const name = type.name
        if (!name) return ''
        for (const root of roots) {
            const candidates = [
                path.join(root, ...moduleParts, name + '.js'),
                path.join(root, ...moduleParts, name, name + '.js'),
                path.join(root, ...moduleParts, name, 'index.js'),
            ]
            for (const candidate of candidates) {
                if (pathExists(candidate)) return candidate
            }
        }
        return ''
    }

    typeLocation(type) {
        if (!type) return null
        if (type.filePath && !type.engine && pathExists(type.filePath)) {
            const doc = this.getDocument(type.filePath)
            if (doc && doc.root && doc.root.nameRange) {
                return { filePath: normalizePath(type.filePath), range: doc.root.nameRange }
            }
            const named = locationInFile(type.filePath, type.name)
            if (named) return named
            return {
                filePath: normalizePath(type.filePath),
                range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
            }
        }
        const file = this.resolveEngineFile(type)
        if (!file) return null
        return locationInFile(file, type.name) || {
            filePath: normalizePath(file),
            range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
        }
    }

    moduleLocation(moduleName) {
        if (!moduleName) return null
        for (const meta of this.localTypes.values()) {
            if (meta.module === moduleName && meta.filePath && pathExists(meta.filePath)) {
                const qmldir = path.join(path.dirname(meta.filePath), 'qmldir')
                if (pathExists(qmldir)) {
                    return {
                        filePath: normalizePath(qmldir),
                        range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
                    }
                }
                return {
                    filePath: normalizePath(meta.filePath),
                    range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
                }
            }
        }
        return null
    }

    inheritanceChain(type, document) {
        const chain = []
        const seen = new Set()
        let current = type
        while (current) {
            const key = current.qualifiedName || current.name || current.filePath
            if (!key || seen.has(key)) break
            seen.add(key)
            chain.push(current)
            if (!current.baseName) break
            current = this.resolveType(current.baseName, current.context || document)
        }
        return chain
    }

    memberOrigin(name, type, document) {
        if (!name) return null
        const chain = this.inheritanceChain(type, document)
        for (const item of chain) {
            if (item.context && item.context.root) {
                const own = ownMembersOf(item.context.root)
                const found = own.find(member => member.name === name)
                if (found && found.filePath && found.range) {
                    return { filePath: found.filePath, range: found.range }
                }
                if (found && item.context.filePath) {
                    const loc = locationInText(item.context.text, item.context.filePath, name)
                    if (loc) return loc
                    return {
                        filePath: item.context.filePath,
                        range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
                    }
                }
            }
        }
        for (const item of chain) {
            const file = this.resolveEngineFile(item)
            if (!file) continue
            const loc = locationInFile(file, name)
            if (loc) return loc
        }
        if (type && type.filePath && !type.engine) {
            return this.typeLocation(type)
        }
        const engineFile = this.resolveEngineFile(type)
        if (engineFile) {
            return {
                filePath: normalizePath(engineFile),
                range: { start: { line: 0, character: 0 }, end: { line: 0, character: 1 } },
            }
        }
        return null
    }

    resolveSymbol(filePath, text, offset) {
        const document = this.analysisDocument(filePath, text)
        const source = text != null ? text : document.text || ''
        const token = tokenAtOffset(source, offset)
        if (!token) return null
        const pos = offsetToPosition(source, offset)
        const element = this.findInnermostElement(document, offset)

        for (const el of document.elements) {
            if (positionInRange(pos, el.nameRange) && el.typeName) {
                const type = this.resolveType(el.typeName, document) || this.lookupTypeByName(el.typeName, document)
                return { kind: 'type', name: el.typeName, type, document, token }
            }
        }

        if (!token.path.length) {
            for (const item of document.imports) {
                if (item.path === token.name || item.path.split('.').pop() === token.name) {
                    return { kind: 'module', name: item.path, document, token }
                }
            }
            if (document.ids[token.name]) {
                return { kind: 'id', name: token.name, element: document.ids[token.name], document, token }
            }
            const byId = this.findElementById(document, token.name)
            if (byId) return { kind: 'id', name: token.name, element: byId, document, token }
        }

        let memberName = token.name
        if (isHandlerName(memberName)) memberName = handlerToSignal(memberName)

        if (token.path.length) {
            const resolved = this.resolveAccess(token.path, document, element)
            const members = memberMap(resolved.members || [])
            const member = members.get(token.name) || members.get(memberName)
            if (member) return { kind: 'member', name: member.name, member, type: resolved.type, document, token }
        }

        if (element) {
            const members = memberMap(this.membersOfElement(element, document))
            const member = members.get(token.name) || members.get(memberName)
            if (member) {
                if (member.kind === KIND.variable) {
                    const byId = this.findElementById(document, token.name)
                    if (byId) return { kind: 'id', name: token.name, element: byId, document, token }
                }
                return {
                    kind: 'member',
                    name: member.name,
                    member,
                    type: this.resolveType(element.typeName, document),
                    document,
                    token,
                }
            }
        }

        const asId = this.findElementById(document, token.name)
        if (asId && !token.path.length) {
            return { kind: 'id', name: token.name, element: asId, document, token }
        }

        const asType = this.resolveType(token.name, document) || this.lookupTypeByName(token.name, document)
        if (asType) return { kind: 'type', name: token.name, type: asType, document, token }
        return null
    }

    locationOfSymbol(symbol) {
        if (!symbol) return []
        if (symbol.kind === 'type') {
            const loc = this.typeLocation(symbol.type)
            return loc ? [loc] : []
        }
        if (symbol.kind === 'module') {
            const loc = this.moduleLocation(symbol.name)
            return loc ? [loc] : []
        }
        if (symbol.kind === 'id') {
            const loc = this.idLocation(symbol.element, symbol.document)
            return loc ? [loc] : []
        }
        if (symbol.kind === 'member') {
            const member = symbol.member
            const locs = []
            if (member.filePath && member.range) locs.push({ filePath: member.filePath, range: member.range })
            if (this.isAliasMember(member)) {
                for (const loc of this.aliasTargetLocations(member, symbol.type, symbol.document)) locs.push(loc)
            }
            if (!locs.length) {
                const loc = this.memberOrigin(member.name, symbol.type, symbol.document)
                    || this.memberOrigin(member.name, this.typeOfMember(member, symbol.document, symbol.type), symbol.document)
                    || this.memberOrigin(member.name, this.groupType(member, symbol.document), symbol.document)
                if (loc) locs.push(loc)
            }
            return uniqueLocations(locs)
        }
        return []
    }

    getHover(filePath, text, offset) {
        const symbol = this.resolveSymbol(filePath, text, offset)
        if (!symbol) return null
        if (symbol.kind === 'member') {
            let detail = symbol.member.detail || symbol.member.name
            if (this.isAliasMember(symbol.member)) {
                const resolved = this.resolveAliasType(symbol.member, symbol.document, symbol.type)
                const path = [symbol.member.aliasId, symbol.member.aliasPath].filter(Boolean).join('.')
                detail = resolved
                    ? 'property alias ' + symbol.member.name + ' : ' + (resolved.qualifiedName || resolved.name)
                    : 'property alias ' + symbol.member.name + (path ? ' : ' + path : '')
            }
            return {
                detail,
                range: symbol.token.range,
            }
        }
        if (symbol.kind === 'type') {
            return {
                detail: (symbol.type && (symbol.type.qualifiedName || symbol.type.name)) || symbol.name,
                range: symbol.token.range,
            }
        }
        if (symbol.kind === 'id') {
            const typeName = symbol.element && symbol.element.typeName ? symbol.element.typeName : ''
            return {
                detail: typeName ? 'id ' + symbol.name + ' : ' + typeName : 'id ' + symbol.name,
                range: symbol.token.range,
            }
        }
        if (symbol.kind === 'module') {
            return { detail: 'module ' + symbol.name, range: symbol.token.range }
        }
        return null
    }

    getDefinition(filePath, text, offset) {
        return this.locationOfSymbol(this.resolveSymbol(filePath, text, offset))
    }
}

function uniqueCompletions(items) {
    const seen = new Set()
    const result = []
    for (const item of items) {
        const key = item.kind + ':' + item.label
        if (seen.has(key)) continue
        seen.add(key)
        result.push(item)
    }
    return result
}

function uniqueLocations(locs) {
    const seen = new Set()
    const result = []
    for (const loc of locs || []) {
        if (!loc || !loc.filePath || !loc.range || !loc.range.start) continue
        const key = normalizePath(loc.filePath) + ':' + loc.range.start.line + ':' + loc.range.start.character
        if (seen.has(key)) continue
        seen.add(key)
        result.push(loc)
    }
    return result
}

module.exports = {
    LanguageService,
    KIND,
    parseQmlDocument,
    loadEngine,
    loadEngineIndex,
    serializeEngine,
    parseQmldir,
    resolveConfigRef,
    inferProjectEnv,
    positionToOffset,
    offsetToPosition,
}
