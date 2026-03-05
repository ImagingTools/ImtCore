// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

/**
 * JavaScript code generator for JQML v2.
 *
 * Converts an AST produced by parser.js into a JavaScript string that calls
 * `JQML.define()` to register the component with the runtime.
 *
 * Generated output format:
 * ```js
 * // Generated from MyComponent.qml
 * JQML.define("MyComponent", {
 *   base:       "ParentType",
 *   id:         "root",
 *   properties: { propName: { type: "string", alias: false, readonly: false, value: "default" }, ... },
 *   signals:    [{ name: "clicked", params: ["x", "y"] }, ...],
 *   handlers:   { onClicked: function() { ... }, ... },
 *   bindings:   { width: function() { return parent.width; }, ... },
 *   functions:  { doThing: function(arg) { ... }, ... },
 *   children:   [ { type: "Text", bindings: { text: function() { return root.title; } }, ... }, ... ]
 * });
 * ```
 */

const INDENT = '    ';

// ── Helpers ───────────────────────────────────────────────────────────────────

function ind(level) {
    return INDENT.repeat(level);
}

/**
 * Wrap a raw value expression so that it is safe as a default.
 * Expressions that reference other components or use bindings become
 * factory functions; literals and `null`/`undefined` are kept as-is.
 */
function wrapValue(raw) {
    if (raw === null || raw === undefined || raw === '' || raw === 'undefined') {
        return 'undefined';
    }
    // Plain literals — keep as-is
    if (/^-?\d+(\.\d+)?$/.test(raw))           return raw;  // number
    if (/^(true|false|null|undefined)$/.test(raw)) return raw;
    if (/^".*"$/.test(raw) || /^'.*'$/.test(raw))  return raw; // string literal

    // Everything else is treated as a binding expression
    return `function() { return ${raw}; }`;
}

// ── Component code generation ──────────────────────────────────────────────────

function genProperties(props, level) {
    if (props.length === 0) return `${ind(level)}properties: {},`;
    const lines = [`${ind(level)}properties: {`];
    for (const p of props) {
        const meta = JSON.stringify({
            type:     p.typeName,
            alias:    !!p.isAlias,
            readonly: !!p.isReadonly,
        });
        const val = wrapValue(p.value);
        lines.push(`${ind(level + 1)}${JSON.stringify(p.name)}: { meta: ${meta}, value: ${val} },`);
    }
    lines.push(`${ind(level)}},`);
    return lines.join('\n');
}

function genSignals(signals, level) {
    if (signals.length === 0) return `${ind(level)}signals: [],`;
    const lines = [`${ind(level)}signals: [`];
    for (const s of signals) {
        const paramNames = s.params.map(p => JSON.stringify(p.name));
        lines.push(`${ind(level + 1)}{ name: ${JSON.stringify(s.name)}, params: [${paramNames.join(', ')}] },`);
    }
    lines.push(`${ind(level)}],`);
    return lines.join('\n');
}

function genHandlers(handlers, level) {
    if (handlers.length === 0) return `${ind(level)}handlers: {},`;
    const lines = [`${ind(level)}handlers: {`];
    for (const h of handlers) {
        // body is already a braced block string or a simple expression
        const fnBody = h.body.startsWith('{') ? h.body : `{ ${h.body}; }`;
        lines.push(`${ind(level + 1)}${JSON.stringify(h.event)}: function() ${fnBody},`);
    }
    lines.push(`${ind(level)}},`);
    return lines.join('\n');
}

function genBindings(bindings, level) {
    if (bindings.length === 0) return `${ind(level)}bindings: {},`;
    const lines = [`${ind(level)}bindings: {`];
    for (const b of bindings) {
        const expr = b.value || 'undefined';
        const fnBody = expr.startsWith('{')
            ? `function() ${expr}`
            : `function() { return ${expr}; }`;
        lines.push(`${ind(level + 1)}${JSON.stringify(b.name)}: ${fnBody},`);
    }
    lines.push(`${ind(level)}},`);
    return lines.join('\n');
}

function genFunctions(functions, level) {
    if (functions.length === 0) return `${ind(level)}functions: {},`;
    const lines = [`${ind(level)}functions: {`];
    for (const fn of functions) {
        const params = fn.params.join(', ');
        lines.push(`${ind(level + 1)}${fn.name}: function(${params}) ${fn.body},`);
    }
    lines.push(`${ind(level)}},`);
    return lines.join('\n');
}

function genChildren(children, level) {
    if (children.length === 0) return `${ind(level)}children: [],`;
    const lines = [`${ind(level)}children: [`];
    for (const child of children) {
        lines.push(genChildObject(child, level + 1) + ',');
    }
    lines.push(`${ind(level)}],`);
    return lines.join('\n');
}

function genChildObject(node, level) {
    const lines = [];
    lines.push(`${ind(level)}{`);
    lines.push(`${ind(level + 1)}type: ${JSON.stringify(node.type)},`);
    if (node.id) {
        lines.push(`${ind(level + 1)}id: ${JSON.stringify(node.id)},`);
    }
    lines.push(genProperties(node.properties, level + 1));
    lines.push(genSignals(node.signals,        level + 1));
    lines.push(genHandlers(node.handlers,      level + 1));
    lines.push(genBindings(node.bindings,      level + 1));
    lines.push(genFunctions(node.functions,    level + 1));
    lines.push(genChildren(node.children,      level + 1));
    lines.push(`${ind(level)}}`);
    return lines.join('\n');
}

// ── Public API ────────────────────────────────────────────────────────────────

/**
 * Generate JavaScript for a single QML AST.
 *
 * @param {{ imports: string[], component: object|null }} ast
 * @param {string} componentName  The component name (typically the filename without .qml).
 * @returns {string}  JavaScript source code.
 */
function generate(ast, componentName) {
    if (!ast || !ast.component) {
        return `// JQML: No component found in ${componentName}.qml\n`;
    }

    const node   = ast.component;
    const level  = 0;
    const lines  = [];

    lines.push(`// Generated from ${componentName}.qml`);
    lines.push(`JQML.define(${JSON.stringify(componentName)}, {`);
    lines.push(`${ind(1)}base: ${JSON.stringify(node.type)},`);

    if (node.id) {
        lines.push(`${ind(1)}id: ${JSON.stringify(node.id)},`);
    }

    lines.push(genProperties(node.properties, 1));
    lines.push(genSignals(node.signals,        1));
    lines.push(genHandlers(node.handlers,      1));
    lines.push(genBindings(node.bindings,      1));
    lines.push(genFunctions(node.functions,    1));
    lines.push(genChildren(node.children,      1));

    lines.push(`});`);

    return lines.join('\n');
}

module.exports = { generate };
