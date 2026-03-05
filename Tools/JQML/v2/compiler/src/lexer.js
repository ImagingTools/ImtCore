// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

/**
 * Token types emitted by the QML lexer.
 */
const TT = {
    // Keywords
    IMPORT:     'IMPORT',
    PROPERTY:   'PROPERTY',
    SIGNAL:     'SIGNAL',
    FUNCTION:   'FUNCTION',
    ALIAS:      'ALIAS',
    READONLY:   'READONLY',
    DEFAULT:    'DEFAULT',
    // Atoms
    IDENTIFIER: 'IDENTIFIER',
    STRING:     'STRING',
    NUMBER:     'NUMBER',
    // Punctuation
    LBRACE:     'LBRACE',
    RBRACE:     'RBRACE',
    LPAREN:     'LPAREN',
    RPAREN:     'RPAREN',
    LBRACKET:   'LBRACKET',
    RBRACKET:   'RBRACKET',
    COLON:      'COLON',
    SEMICOLON:  'SEMICOLON',
    DOT:        'DOT',
    COMMA:      'COMMA',
    // Catch-all for operators (+, -, *, /, =, <, >, !, &, |, ?, etc.)
    OTHER:      'OTHER',
    EOF:        'EOF',
};

const PUNC_MAP = {
    '{': TT.LBRACE,  '}': TT.RBRACE,
    '(': TT.LPAREN,  ')': TT.RPAREN,
    '[': TT.LBRACKET, ']': TT.RBRACKET,
    ':': TT.COLON,   ';': TT.SEMICOLON,
    '.': TT.DOT,     ',': TT.COMMA,
};

/**
 * Tokenize a QML source string.
 *
 * @param {string} src  Raw QML source text.
 * @returns {Array<{t: string, v: string|null, line: number}>}  Token list.
 */
function tokenize(src) {
    const toks = [];
    let i = 0;
    let line = 1;
    const n = src.length;

    const ch  = (off = 0) => (i + off < n ? src[i + off] : '');
    const eat = () => {
        const c = src[i++];
        if (c === '\n') line++;
        return c;
    };
    const push = (t, v) => toks.push({ t, v, line });

    while (i < n) {
        const c = ch();

        // ── Whitespace ────────────────────────────────────────────────────────
        if (c === ' ' || c === '\t' || c === '\r' || c === '\n') { eat(); continue; }

        // ── Line comment ──────────────────────────────────────────────────────
        if (c === '/' && ch(1) === '/') {
            while (i < n && ch() !== '\n') eat();
            continue;
        }

        // ── Block comment ─────────────────────────────────────────────────────
        if (c === '/' && ch(1) === '*') {
            eat(); eat();
            while (i < n && !(ch() === '*' && ch(1) === '/')) eat();
            if (i < n) { eat(); eat(); }
            continue;
        }

        // ── String literal ────────────────────────────────────────────────────
        if (c === '"' || c === "'") {
            const q = eat();
            let s = '';
            while (i < n && ch() !== q) {
                const x = eat();
                s += (x === '\\' && i < n) ? x + eat() : x;
            }
            if (i < n) eat(); // closing quote
            push(TT.STRING, s);
            continue;
        }

        // ── Template literal (pass through as OTHER) ──────────────────────────
        if (c === '`') {
            let s = eat(); // opening backtick
            while (i < n && ch() !== '`') {
                const x = eat();
                s += (x === '\\' && i < n) ? x + eat() : x;
            }
            if (i < n) s += eat(); // closing backtick
            push(TT.OTHER, s);
            continue;
        }

        // ── Number literal ────────────────────────────────────────────────────
        if (c >= '0' && c <= '9') {
            let s = '';
            while (i < n && (ch() >= '0' && ch() <= '9' || ch() === '.')) s += eat();
            // Handle hex (0x...) and scientific notation (1e5)
            if (i < n && (ch() === 'x' || ch() === 'X')) s += eat();
            while (i < n && (ch() >= '0' && ch() <= '9' ||
                    ch() >= 'a' && ch() <= 'f' || ch() >= 'A' && ch() <= 'F')) s += eat();
            if (i < n && (ch() === 'e' || ch() === 'E')) {
                s += eat();
                if (i < n && (ch() === '+' || ch() === '-')) s += eat();
                while (i < n && ch() >= '0' && ch() <= '9') s += eat();
            }
            push(TT.NUMBER, s);
            continue;
        }

        // ── Identifier / keyword ──────────────────────────────────────────────
        if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c === '_' || c === '$') {
            let s = '';
            while (i < n && (ch() >= 'a' && ch() <= 'z' || ch() >= 'A' && ch() <= 'Z' ||
                    ch() >= '0' && ch() <= '9' || ch() === '_' || ch() === '$')) {
                s += eat();
            }
            let type;
            switch (s) {
                case 'import':   type = TT.IMPORT;    break;
                case 'property': type = TT.PROPERTY;  break;
                case 'signal':   type = TT.SIGNAL;    break;
                case 'function': type = TT.FUNCTION;  break;
                case 'alias':    type = TT.ALIAS;     break;
                case 'readonly': type = TT.READONLY;  break;
                case 'default':  type = TT.DEFAULT;   break;
                default:         type = TT.IDENTIFIER;
            }
            push(type, s);
            continue;
        }

        // ── Punctuation ───────────────────────────────────────────────────────
        if (PUNC_MAP[c]) {
            push(PUNC_MAP[c], c);
            eat();
            continue;
        }

        // ── Everything else (operators, etc.) ─────────────────────────────────
        push(TT.OTHER, eat());
    }

    push(TT.EOF, null);
    return toks;
}

module.exports = { tokenize, TT };
