// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

const { tokenize, TT } = require('./lexer');

/**
 * Parse a QML source file into an AST.
 *
 * The returned AST has the shape:
 * ```
 * {
 *   imports:   ImportNode[],
 *   component: ComponentNode | null
 * }
 * ```
 *
 * ComponentNode:
 * ```
 * {
 *   type:       string,          // e.g. "Rectangle", "ViewBase"
 *   id:         string | null,
 *   properties: PropertyNode[],
 *   signals:    SignalNode[],
 *   handlers:   HandlerNode[],
 *   functions:  FunctionNode[],
 *   bindings:   BindingNode[],
 *   children:   ComponentNode[]
 * }
 * ```
 *
 * @param {string} src  Raw QML source text.
 * @returns {{ imports: string[], component: object|null }}
 */
function parse(src) {
    const toks = tokenize(src);
    let pos = 0;

    // ── Token stream helpers ──────────────────────────────────────────────────

    const peek  = (off = 0) => toks[Math.min(pos + off, toks.length - 1)];
    const eat   = ()        => toks[pos++];
    const check = (t, v)    => peek().t === t && (v === undefined || peek().v === v);
    const match = (t, v)    => { if (check(t, v)) { eat(); return true; } return false; };
    const expect = (t) => {
        if (peek().t !== t) {
            throw new Error(
                `Expected token ${t}, got ${peek().t} ('${peek().v}') at line ${peek().line}`
            );
        }
        return eat();
    };

    // ── Helpers ───────────────────────────────────────────────────────────────

    /** Read a possibly-dotted type name: QtQuick.Controls or Rectangle */
    function readTypeName() {
        let name = expect(TT.IDENTIFIER).v;
        while (check(TT.DOT) && peek(1).t === TT.IDENTIFIER) {
            eat();
            name += '.' + eat().v;
        }
        return name;
    }

    /**
     * Read a balanced braced block starting at the current '{'.
     * Returns the raw reconstructed source including outer braces.
     */
    function readBracedBlock() {
        expect(TT.LBRACE);
        let out = '{';
        let depth = 1;
        while (pos < toks.length && depth > 0) {
            const tok = eat();
            switch (tok.t) {
                case TT.LBRACE:    depth++; out += '{'; break;
                case TT.RBRACE:    depth--; if (depth > 0) out += '}'; break;
                case TT.STRING:    out += JSON.stringify(tok.v); break;
                case TT.SEMICOLON: out += ';'; break;
                default:           out += tok.v !== null ? tok.v : '';
            }
            // Re-insert spaces between tokens for readability
            if (depth > 0 && tok.t !== TT.OTHER) out += ' ';
        }
        out += '}';
        return out;
    }

    /**
     * Read the value/body after a ':' binding or handler.
     * - If the next token is '{', read the whole braced block.
     * - Otherwise, read a single-line expression until a clear statement
     *   boundary is detected (new-line-based heuristic).
     */
    function readValue() {
        if (check(TT.LBRACE)) {
            return readBracedBlock();
        }

        let expr = '';
        const startLine = peek().line;

        while (pos < toks.length) {
            const tok = peek();

            if (tok.t === TT.EOF)       break;
            if (tok.t === TT.RBRACE)    break;
            if (tok.t === TT.SEMICOLON) { eat(); break; }

            // Stop if we cross a line boundary and the next token looks like
            // the start of a new QML statement.
            if (tok.line > startLine) {
                const next1 = peek(1);
                if (tok.t === TT.PROPERTY ||
                    tok.t === TT.SIGNAL   ||
                    tok.t === TT.FUNCTION ||
                    tok.t === TT.READONLY ||
                    tok.t === TT.DEFAULT  ||
                    tok.t === TT.IMPORT   ||
                    // identifier followed by '{' → child component
                    (tok.t === TT.IDENTIFIER && next1.t === TT.LBRACE) ||
                    // identifier followed by ':' → property binding
                    (tok.t === TT.IDENTIFIER && next1.t === TT.COLON)  ||
                    // identifier followed by '.' and then ':' somewhere → dotted binding / handler
                    (tok.t === TT.IDENTIFIER && next1.t === TT.DOT)) {
                    break;
                }
            }

            if (tok.t === TT.LBRACE) {
                expr += ' ' + readBracedBlock();
            } else {
                const t = eat();
                expr += (t.t === TT.STRING ? JSON.stringify(t.v) : (t.v !== null ? t.v : ''));
            }
        }
        return expr.trim();
    }

    // ── Grammar ───────────────────────────────────────────────────────────────

    function parseImports() {
        const imports = [];
        while (check(TT.IMPORT)) {
            eat(); // consume 'import'
            let raw = '';
            const startLine = peek().line;
            // Collect tokens until we reach what looks like the component body
            // or another import statement.
            while (!check(TT.EOF)) {
                const tok = peek();
                // A '{' at the same or deeper nesting means we hit the component
                if (tok.t === TT.LBRACE) break;
                // Next import statement
                if (tok.t === TT.IMPORT) break;
                // First identifier-like token on a NEW line that's not a version
                // number → we've moved past this import
                if (tok.line > startLine &&
                    tok.t !== TT.DOT &&
                    tok.t !== TT.NUMBER &&
                    tok.t !== TT.IDENTIFIER &&
                    tok.t !== TT.STRING) {
                    break;
                }
                if (tok.line > startLine &&
                    (tok.t === TT.IDENTIFIER) &&
                    raw.trim().length > 0) {
                    // Already have some content and we're on a new line:
                    // check whether what follows looks like a new import path
                    // rather than a version/alias qualifier (which stay on same line).
                    const prevLine = toks[pos - 1].line;
                    if (tok.line > prevLine) break;
                }
                const t = eat();
                raw += (t.t === TT.STRING ? '"' + t.v + '"' : t.v) + ' ';
            }
            const trimmed = raw.trim();
            if (trimmed) imports.push(trimmed);
        }
        return imports;
    }

    function parseComponentBody(node) {
        while (!check(TT.RBRACE) && !check(TT.EOF)) {
            // ── id: identifier ───────────────────────────────────────────────
            if (check(TT.IDENTIFIER, 'id') && peek(1).t === TT.COLON) {
                eat(); eat();
                node.id = expect(TT.IDENTIFIER).v;
                match(TT.SEMICOLON);
                continue;
            }

            // ── readonly / default modifiers ─────────────────────────────────
            const isReadonly = match(TT.READONLY);
            const isDefault  = !isReadonly && match(TT.DEFAULT);

            // ── property [alias] <type> <name> [: <value>] ───────────────────
            if (check(TT.PROPERTY)) {
                eat();
                const isAlias = match(TT.ALIAS);
                // For aliases the format is: property alias <name>: <target>
                // There is no type token; for regular properties it is: property <type> <name>
                let typeName, name;
                if (isAlias) {
                    typeName = 'alias';
                    name     = expect(TT.IDENTIFIER).v;
                } else {
                    typeName = readTypeName();
                    name     = expect(TT.IDENTIFIER).v;
                }
                let value = null;
                if (match(TT.COLON)) {
                    value = readValue();
                }
                match(TT.SEMICOLON);
                node.properties.push({ typeName, name, value, isAlias, isReadonly, isDefault });
                continue;
            }

            // ── signal <name> [( <params> )] ──────────────────────────────────
            if (check(TT.SIGNAL)) {
                eat();
                const name = expect(TT.IDENTIFIER).v;
                const params = [];
                if (match(TT.LPAREN)) {
                    while (!check(TT.RPAREN) && !check(TT.EOF)) {
                        // param: [type] name
                        const first = readTypeName();
                        if (check(TT.IDENTIFIER)) {
                            params.push({ type: first, name: eat().v });
                        } else {
                            params.push({ type: 'var', name: first });
                        }
                        match(TT.COMMA);
                    }
                    expect(TT.RPAREN);
                }
                match(TT.SEMICOLON);
                node.signals.push({ name, params });
                continue;
            }

            // ── function <name> ( <params> ) { <body> } ───────────────────────
            if (check(TT.FUNCTION)) {
                eat();
                const name = expect(TT.IDENTIFIER).v;
                expect(TT.LPAREN);
                const params = [];
                while (!check(TT.RPAREN) && !check(TT.EOF)) {
                    params.push(eat().v);
                    match(TT.COMMA);
                }
                expect(TT.RPAREN);
                const body = readBracedBlock();
                node.functions.push({ name, params, body });
                continue;
            }

            // ── <Obj>.<onEvent>: handler  e.g. Component.onCompleted ──────────
            if (check(TT.IDENTIFIER) &&
                peek(1).t === TT.DOT &&
                peek(2).t === TT.IDENTIFIER && /^on[A-Z]/.test(peek(2).v) &&
                peek(3).t === TT.COLON) {
                const obj   = eat().v; eat();
                const event = eat().v; eat(); // ':'
                const body  = readValue();
                node.handlers.push({ event: obj + '.' + event, body });
                continue;
            }

            // ── onXxx: handler ────────────────────────────────────────────────
            if (check(TT.IDENTIFIER) && /^on[A-Z]/.test(peek().v) && peek(1).t === TT.COLON) {
                const event = eat().v; eat(); // ':'
                const body  = readValue();
                node.handlers.push({ event, body });
                continue;
            }

            // ── Child component: <TypeName> { ... } ───────────────────────────
            if (check(TT.IDENTIFIER) && peek(1).t === TT.LBRACE) {
                node.children.push(parseComponent());
                continue;
            }

            // ── Dotted child: <Type>.<Sub> { ... } ────────────────────────────
            if (check(TT.IDENTIFIER) &&
                peek(1).t === TT.DOT &&
                peek(2).t === TT.IDENTIFIER &&
                peek(3).t === TT.LBRACE) {
                node.children.push(parseComponent());
                continue;
            }

            // ── Dotted binding: anchors.left: ..., sourceSize.width: ... ──────
            if (check(TT.IDENTIFIER) && peek(1).t === TT.DOT) {
                let name = eat().v;
                while (check(TT.DOT) && peek(1).t === TT.IDENTIFIER) {
                    eat();
                    name += '.' + eat().v;
                }
                if (match(TT.COLON)) {
                    const value = readValue();
                    match(TT.SEMICOLON);
                    node.bindings.push({ name, value });
                } else {
                    match(TT.SEMICOLON);
                }
                continue;
            }

            // ── Plain property binding: <name>: <value> ───────────────────────
            if (check(TT.IDENTIFIER) && peek(1).t === TT.COLON) {
                const name = eat().v; eat(); // ':'
                const value = readValue();
                match(TT.SEMICOLON);
                node.bindings.push({ name, value });
                continue;
            }

            // ── Skip anything else (pragma, unknown constructs) ───────────────
            eat();
        }
    }

    function parseComponent() {
        const type = readTypeName();
        expect(TT.LBRACE);

        const node = {
            type,
            id:         null,
            properties: [],
            signals:    [],
            handlers:   [],
            functions:  [],
            bindings:   [],
            children:   [],
        };

        try {
            parseComponentBody(node);
        } catch (e) {
            // Best-effort: stop parsing this component on error
        }

        expect(TT.RBRACE);
        return node;
    }

    // ── Entry point ───────────────────────────────────────────────────────────

    const imports = parseImports();

    // Skip any remaining pragma/comment-style tokens before the root component
    while (!check(TT.IDENTIFIER) && !check(TT.EOF)) eat();

    let component = null;
    if (!check(TT.EOF)) {
        try {
            component = parseComponent();
        } catch (e) {
            component = null;
        }
    }

    return { imports, component };
}

module.exports = { parse };
