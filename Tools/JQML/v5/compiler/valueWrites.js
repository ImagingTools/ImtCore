// A component of a color written in JavaScript (c.a = 0.5, item.color.r = 1): QML colors are value types (QColor,
// QQuickColorValueType) whose components can be set; v5 keeps a color as its name, a string that cannot change. The
// assignment is made a call that, when the value is a color, makes the new color and stores it where the value came
// from - the variable, or the property it was read from (QV4::QQmlValueTypeWrapper writes a property reference back);
// any other value gets the member assigned as JavaScript does.
//   x.a = v          -> JQ.cset(x, "a", v, $n => x = $n)
//   o.p.a = v        -> JQ.csetm(o, "p", "a", v)
//   o[k].a = v       -> JQ.csetm(o, k, "a", v)
//   f().a = v        -> JQ.cset(f(), "a", v, null)
// A const variable written this way becomes let (the color it holds changes, as the QColor of Qt does).
const U = require('uglify-js')

const COLOR_COMPONENTS = new Set(['r', 'g', 'b', 'a', 'hsvHue', 'hsvSaturation', 'hsvValue', 'hslHue', 'hslSaturation', 'hslLightness'])

const ref = (source) => U.parse('(' + source + ')').body[0].body

function colorWrites(ast){
    let consts = new Set()
    let found = false
    ast = ast.transform(new U.TreeTransformer(null, function(node){
        if(!(node instanceof U.AST_Assign) || node.operator !== '=') return undefined
        let left = node.left
        if(!(left instanceof U.AST_Dot) || !COLOR_COMPONENTS.has(left.property)) return undefined
        found = true
        let target = left.expression
        let name = new U.AST_String({ value: left.property })
        if(target instanceof U.AST_SymbolRef && target.thedef && !target.thedef.undeclared){
            let def = target.thedef
            if(def.orig && def.orig[0] instanceof U.AST_SymbolConst) consts.add(def)
            let assign = new U.AST_Arrow({
                argnames: [new U.AST_SymbolFunarg({ name: '$n' })],
                body: [],
                value: new U.AST_Assign({ operator: '=', left: new U.AST_SymbolRef({ name: target.name, thedef: def }), right: new U.AST_SymbolRef({ name: '$n' }) }),
            })
            return new U.AST_Call({ start: node.start, end: node.end, expression: ref('JQ.cset'), args: [target, name, node.right, assign] })
        }
        if(target instanceof U.AST_Dot){
            return new U.AST_Call({ start: node.start, end: node.end, expression: ref('JQ.csetm'),
                args: [target.expression, new U.AST_String({ value: target.property }), name, node.right] })
        }
        if(target instanceof U.AST_Sub){
            return new U.AST_Call({ start: node.start, end: node.end, expression: ref('JQ.csetm'), args: [target.expression, target.property, name, node.right] })
        }
        return new U.AST_Call({ start: node.start, end: node.end, expression: ref('JQ.cset'), args: [target, name, node.right, new U.AST_Null()] })
    }))
    if(!found || !consts.size) return ast
    return ast.transform(new U.TreeTransformer(null, function(node){
        if(node instanceof U.AST_Const && node.definitions.some(d => d.name && d.name.thedef && consts.has(d.name.thedef))){
            return new U.AST_Let({ start: node.start, end: node.end, definitions: node.definitions })
        }
        return undefined
    }))
}

module.exports = { colorWrites, COLOR_COMPONENTS }
