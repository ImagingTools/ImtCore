// Rewrites QML JavaScript: every free identifier goes through resolve(name),
// which returns replacement JS source or null to keep the name (JS globals).
const U = require('uglify-js')

const replacementCache = new Map()

function replacementNode(source){
    let cached = replacementCache.get(source)
    if(cached) return cached.clone(true)
    let node = U.parse('(' + source + ')').body[0].body
    replacementCache.set(source, node)
    return node.clone(true)
}

// QML allows a line break inside a string literal, JavaScript does not
function escapeStringBreaks(source){
    let out = '', quote = null
    for(let i = 0; i < source.length; i++){
        let ch = source[i]
        if(quote){
            if(ch === '\\'){ out += ch + source[++i]; continue }
            if(ch === '\n'){ out += '\\n'; continue }
            if(ch === quote) quote = null
        } else if(ch === '"' || ch === "'") quote = ch
        else if(ch === '/' && source[i + 1] === '/'){
            let end = source.indexOf('\n', i)
            if(end < 0) end = source.length
            out += source.slice(i, end)
            i = end - 1
            continue
        } else if(ch === '/' && source[i + 1] === '*'){
            let end = source.indexOf('*/', i + 2)
            end = end < 0 ? source.length : end + 2
            out += source.slice(i, end)
            i = end - 1
            continue
        }
        out += ch
    }
    return out
}

function parseQml(source){
    try {
        return U.parse(source)
    } catch(error) {
        if(!/Unterminated string/.test(error && error.message)) throw error
        return U.parse(escapeStringBreaks(source))
    }
}

function parse(source, where){
    try {
        let ast = parseQml(source)
        ast.figure_out_scope()
        return ast
    } catch(error) {
        let message = error && error.message ? error.message : String(error)
        throw new Error(where + ': ' + message + '\n' + source)
    }
}

function transformFree(ast, resolve, free){
    return ast.transform(new U.TreeTransformer(null, function(node){
        if(node instanceof U.AST_SymbolRef && node.thedef && node.thedef.undeclared){
            let source = resolve(node.name)
            if(source === null || source === undefined) return undefined
            if(free) free.add(node.name)
            return replacementNode(source)
        }
        return undefined
    }))
}

const PRINT = { beautify: false, comments: false, ascii_only: false }

// The raw text of a property value runs up to the next member and can carry `;` and comments
// (`radius: radiusL; // sizes`). When it is not an expression as is, the parser extracts it.
function expressionText(raw){
    try {
        U.parse('(' + raw + '\n)')
        return raw
    } catch(error) {
        if(/Unterminated string/.test(error && error.message)) return expressionText(escapeStringBreaks(raw))
        // `return <raw>` drops a trailing `;` and comments; a leading string literal is not a directive there
        let fn = U.parse('(function(){return ' + raw + '\n})').body[0].body
        let statements = fn.body.filter(node => !(node instanceof U.AST_EmptyStatement))
        if(statements.length === 1 && statements[0] instanceof U.AST_Return && statements[0].value){
            return statements[0].value.print_to_string(PRINT)
        }
        return raw
    }
}

// Value of a binding or an assignment: `expr`, `{ statements }` (block with return) or `function(...){...}`
function rewriteValue(raw, isBlock, params, resolve, where){
    let source = isBlock
        ? '(function(' + params + ')' + raw + '\n)'
        : '(function(' + params + '){return(' + expressionText(raw) + '\n)})'
    let free = new Set()
    let ast = transformFree(parse(source, where), resolve, free)
    return { code: ast.print_to_string(PRINT).replace(/;$/, ''), free }
}

// Signal handler body: statement, block or a function expression called with the signal arguments
function rewriteHandler(raw, kind, params, args, resolve, where){
    let source
    if(kind === 'function'){
        source = '(function(' + params + ',...$a){return(' + raw + '\n).apply($self,$a)})'
    } else if(kind === 'block'){
        // the body is a block of its own: `let index` in `onItemRemoved: {...}` may shadow the argument
        source = '(function(' + params + (args.length ? ',' : '') + args.join(',') + '){' + raw + '\n})'
    } else {
        source = '(function(' + params + (args.length ? ',' : '') + args.join(',') + '){' + raw + '\n})'
    }
    let ast = transformFree(parse(source, where), resolve, null)
    return ast.print_to_string(PRINT).replace(/;$/, '')
}

// `function name(args) { body }` -> function expression with a prologue
function rewriteMethod(raw, prologue, resolve, where){
    let source = '(' + raw.replace(/^function\s+[\w$]+\s*\(/, 'function(') + '\n)'
    let ast = parse(source, where)
    let fn = ast.body[0].body
    fn.body.unshift(U.parse(prologue).body[0])
    ast.figure_out_scope()
    ast = transformFree(ast, resolve, null)
    return ast.print_to_string(PRINT).replace(/;$/, '')
}

module.exports = { rewriteValue, rewriteHandler, rewriteMethod, expressionText }
