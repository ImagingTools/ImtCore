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

// `value as Type` (QML type assertion, a RelationalExpression like instanceof): written `value instanceof __qmlas__(Type)`
// for the parser of JavaScript, then a call JQ.as(value, Type) (transformFree). `as` is the operator only after the end
// of an operand (a name, a number, a string, ) or ]) and before a type name
function rewriteAs(source){
    if(source.indexOf(' as ') < 0) return source
    let out = '', quote = null
    for(let i = 0; i < source.length; i++){
        let ch = source[i]
        if(quote){
            out += ch
            if(ch === '\\'){ out += source[++i]; continue }
            if(ch === quote) quote = null
            continue
        }
        if(ch === '"' || ch === "'" || ch === '`'){ quote = ch; out += ch; continue }
        if(ch === '/' && (source[i + 1] === '/' || source[i + 1] === '*')){
            let end = source[i + 1] === '/' ? source.indexOf('\n', i) : source.indexOf('*/', i + 2) + 2
            if(end <= 0) end = source.length
            out += source.slice(i, end)
            i = end - 1
            continue
        }
        if(ch === 'a' && source[i + 1] === 's' && /\s/.test(source[i - 1] || '') && /\s/.test(source[i + 2] || '')){
            let before = out.replace(/\s+$/, '')
            let last = before[before.length - 1] || ''
            let m = /^\s+([A-Za-z_$][\w$]*(?:\.[A-Za-z_$][\w$]*)*)/.exec(source.slice(i + 2))
            if(m && /[\w$)\]"'`]/.test(last) && !/\b(?:import|export|return|typeof|case|new|in|of)$/.test(before)){
                out += 'instanceof __qmlas__(' + m[1] + ')'
                i += 1 + m[0].length
                continue
            }
        }
        out += ch
    }
    return out
}

function parses(source){
    try { U.parse(source); return true } catch(error) { return false }
}

function parseQml(source){
    source = rewriteAs(source)
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

// resolve.callingContext(), when the resolver has it: the calling context of the code for a call x.createQmlObject(...)
// (QV4 callingQmlContext), as JS sources { qt, scope, ids }
function transformFree(ast, resolve, free){
    // Singleton.EnumValue: a value of the type (resolve.typeMember); T.Overlay: the attached object of a type of a
    // namespace (resolve.qualifiedMember); before the name in it is resolved
    const before = (node) => {
        if(resolve.typeMember && node instanceof U.AST_Dot && typeof node.property === 'string' && node.expression instanceof U.AST_SymbolRef
            && node.expression.thedef && node.expression.thedef.undeclared){
            let source = resolve.typeMember(node.expression.name, node.property)
                || (resolve.qualifiedMember && resolve.qualifiedMember(node.expression.name, node.property))
            if(source){
                if(free) free.add(node.expression.name)
                return replacementNode(source)
            }
        }
        return undefined
    }
    return ast.transform(new U.TreeTransformer(before, function(node){
        // value instanceof __qmlas__(Type): JQ.as(value, Type) (rewriteAs)
        if(node instanceof U.AST_Binary && node.operator === 'instanceof' && node.right instanceof U.AST_Call
            && node.right.expression instanceof U.AST_SymbolRef && node.right.expression.name === '__qmlas__'){
            return new U.AST_Call({ expression: replacementNode('JQ.as'), args: [node.left, node.right.args[0]] })
        }
        if(node instanceof U.AST_SymbolRef && node.name === '__qmlas__') return undefined
        if(node instanceof U.AST_SymbolRef && node.thedef && node.thedef.undeclared){
            let source = resolve(node.name)
            if(source === null || source === undefined) return undefined
            if(free) free.add(node.name)
            return replacementNode(source)
        }
        // `typeof name` of a name looked up at run time is "undefined" when it is not found, like JavaScript
        if(node instanceof U.AST_UnaryPrefix && node.operator === 'typeof' && node.expression instanceof U.AST_Call
            && node.expression.expression instanceof U.AST_Dot && node.expression.expression.property === 'ctx'
            && node.expression.expression.expression instanceof U.AST_SymbolRef && node.expression.expression.expression.name === 'JQ'){
            node.expression.expression.property = 'ctxValue'
            return node
        }
        if(resolve.callingContext && node instanceof U.AST_Call && node.expression instanceof U.AST_Dot && node.expression.property === 'createComponent'){
            return new U.AST_Call({
                start: node.start,
                end: node.end,
                expression: replacementNode('JQ.callCreateComponent'),
                args: [node.expression.expression, replacementNode(resolve.callingContext().scope)].concat(node.args),
            })
        }
        if(resolve.callingContext && node instanceof U.AST_Call && node.expression instanceof U.AST_Dot && node.expression.property === 'createQmlObject'){
            let context = resolve.callingContext()
            return new U.AST_Call({
                start: node.start,
                end: node.end,
                expression: replacementNode('JQ.callCreateQmlObject'),
                args: [node.expression.expression, replacementNode(context.qt), replacementNode(context.scope), replacementNode(context.ids)].concat(node.args),
            })
        }
        return undefined
    }))
}

const PRINT = { beautify: false, comments: false, ascii_only: false }

// The raw text of a property value runs up to the next member and can carry `;` and comments
// (`radius: radiusL; // sizes`). When it is not an expression as is, the parser extracts it.
function expressionText(raw){
    raw = rewriteAs(raw)
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
    // `map: { "a": x, "b": y }`: no block of statements, an object literal (the QML parser of Qt takes the expression)
    if(isBlock && !parses('(function()' + rewriteAs(raw) + '\n)') && parses('(' + rewriteAs(raw) + '\n)')) isBlock = false
    let source = isBlock
        ? '(function(' + params + ')' + raw + '\n)'
        : '(function(' + params + '){return(' + expressionText(raw) + '\n)})'
    let free = new Set()
    let parsed = parse(source, where)
    // a block is a binding by its completion value: `{ n * 10 }`, `{ let k = 1; k + n }` (the last statement)
    if(isBlock) completionReturn(parsed)
    let ast = transformFree(parsed, resolve, free)
    return { code: ast.print_to_string(PRINT).replace(/;$/, ''), free }
}

// the last statement of the function of a block binding, an expression, returns its value; an if / else does it in its
// branches (the completion value of the block, as QV4 evaluates a binding)
function completionReturn(ast){
    let fn = ast.body[0] && ast.body[0].body
    if(!(fn instanceof U.AST_Lambda)) return
    const last = (body) => {
        let i = body.length - 1
        while(i >= 0 && body[i] instanceof U.AST_EmptyStatement) i--
        if(i < 0) return
        let st = body[i]
        if(st instanceof U.AST_SimpleStatement) body[i] = new U.AST_Return({ value: st.body })
        else if(st instanceof U.AST_If){
            for(let branch of ['body', 'alternative']){
                let b = st[branch]
                if(b instanceof U.AST_BlockStatement) last(b.body)
                else if(b instanceof U.AST_SimpleStatement) st[branch] = new U.AST_Return({ value: b.body })
            }
        } else if(st instanceof U.AST_BlockStatement) last(st.body)
    }
    last(fn.body)
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

// QtTest: wait(), tryCompare(), tryVerify() and SignalSpy.wait() block a test function in Qt while the events go on;
// here they return promises. The calls of a method that wait: free calls of the names (the TestCase functions and its
// methods that wait themselves) and calls of x.wait(), in the method itself (not in the functions it makes)
function waitingCalls(fn, names){
    let calls = []
    fn.walk(new U.TreeWalker(function(node){
        if(node !== fn && node instanceof U.AST_Lambda) return true
        if(node instanceof U.AST_Call){
            let callee = node.expression
            if(callee instanceof U.AST_SymbolRef && names.has(callee.name) && (!callee.thedef || callee.thedef.undeclared)) calls.push(node)
            else if(callee instanceof U.AST_Dot && callee.property === 'wait') calls.push(node)
        }
    }))
    return calls
}

function methodAst(raw, where){
    let source = '(' + raw.replace(/^function\s+[\w$]+\s*\(/, 'function(') + '\n)'
    return parse(source, where)
}

// does the method wait (waitingCalls)
function methodWaits(raw, names, where){
    let ast = methodAst(raw, where)
    return waitingCalls(ast.body[0].body, names).length > 0
}

// `function name(args) { body }` -> function expression with a prologue; with waitNames, an async function that
// awaits its waiting calls (when it has any)
function rewriteMethod(raw, prologue, resolve, where, waitNames = null){
    let ast = methodAst(raw, where)
    let fn = ast.body[0].body
    let waits = waitNames ? new Set(waitingCalls(fn, waitNames)) : null
    if(waits && waits.size){
        ast = ast.transform(new U.TreeTransformer(null, function(node){
            if(waits.has(node)) return new U.AST_Await({ start: node.start, end: node.end, expression: node })
            return undefined
        }))
        fn = ast.body[0].body
    }
    fn.body.unshift(U.parse(prologue).body[0])
    ast.figure_out_scope()
    ast = transformFree(ast, resolve, null)
    let code = ast.print_to_string(PRINT).replace(/;$/, '')
    if(waits && waits.size) code = code.replace(/^(\(?)function\(/, '$1async function(')
    return code
}

module.exports = { rewriteValue, rewriteHandler, rewriteMethod, methodWaits, expressionText }
