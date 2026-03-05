// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

const fs   = require('fs');
const path = require('path');

const { parse }    = require('./parser');
const { generate } = require('./codegen');

// ── JQML v2 browser runtime ───────────────────────────────────────────────────
// Embedded verbatim into jqml.full.js so the output is self-contained.

const RUNTIME = `// ============================================================
// JQML v2 Runtime
// QML component registry and lifecycle helpers for web deployment
// ============================================================
(function(global) {
    'use strict';

    var _registry  = Object.create(null);
    var _instances = [];

    /**
     * Register a QML component definition.
     * Called by generated code for every compiled .qml file.
     *
     * @param {string} name        Component name (e.g. "MyButton").
     * @param {object} descriptor  Component descriptor object.
     */
    function define(name, descriptor) {
        _registry[name] = descriptor;
    }

    /**
     * Instantiate a registered component.
     *
     * @param {string} name      Component name.
     * @param {object} overrides Property overrides applied on top of defaults.
     * @returns {object} Live component instance.
     */
    function create(name, overrides) {
        var def = _registry[name];
        if (!def) {
            throw new Error('JQML: Unknown component "' + name + '"');
        }

        // Deep-clone descriptor properties so instances are independent
        var props = Object.create(null);
        var defProps = def.properties || {};
        for (var k in defProps) {
            var entry = defProps[k];
            var val   = (overrides && overrides.hasOwnProperty(k))
                ? overrides[k]
                : (typeof entry.value === 'function' ? entry.value() : entry.value);
            props[k] = { meta: entry.meta, value: val };
        }

        var handlers  = Object.assign(Object.create(null), def.handlers  || {});
        var bindings  = Object.assign(Object.create(null), def.bindings  || {});
        var functions = Object.assign(Object.create(null), def.functions || {});

        var instance = {
            _type:      name,
            _base:      def.base,
            _id:        def.id || null,
            properties: props,
            signals:    (def.signals  || []).slice(),
            handlers:   handlers,
            bindings:   bindings,
            functions:  functions,
            children:   (def.children || []).slice(),

            /** Get a property value, resolving binding functions. */
            get: function(propName) {
                var p = this.properties[propName];
                if (!p) return undefined;
                return (typeof p.value === 'function') ? p.value.call(this) : p.value;
            },

            /** Set a property value and notify change handlers. */
            set: function(propName, value) {
                var p = this.properties[propName];
                if (!p) { this.properties[propName] = { value: value }; return; }
                if (p.meta && p.meta.readonly) {
                    console.warn('JQML: Attempt to write readonly property "' + propName + '"');
                    return;
                }
                p.value = value;
                var handlerName = 'on' + propName.charAt(0).toUpperCase() + propName.slice(1) + 'Changed';
                if (typeof this.handlers[handlerName] === 'function') {
                    this.handlers[handlerName].call(this);
                }
            },

            /** Emit a signal — invokes the matching onXxx handler. */
            emit: function(signalName) {
                var handlerName = 'on' + signalName.charAt(0).toUpperCase() + signalName.slice(1);
                if (typeof this.handlers[handlerName] === 'function') {
                    this.handlers[handlerName].apply(this, Array.prototype.slice.call(arguments, 1));
                }
            },

            /** Call a QML function defined on this component. */
            call: function(fnName) {
                var fn = this.functions[fnName];
                if (typeof fn !== 'function') {
                    throw new Error('JQML: No function "' + fnName + '" on "' + name + '"');
                }
                return fn.apply(this, Array.prototype.slice.call(arguments, 1));
            },

            /** Trigger Component.onCompleted if defined. */
            complete: function() {
                var h = this.handlers['Component.onCompleted'];
                if (typeof h === 'function') h.call(this);
                return this;
            }
        };

        // Apply extra overrides that are not in properties (e.g. id overrides)
        if (overrides) {
            for (var key in overrides) {
                if (!instance.properties.hasOwnProperty(key)) {
                    instance[key] = overrides[key];
                }
            }
        }

        _instances.push(instance);
        return instance;
    }

    /** Look up a component descriptor by name. */
    function get(name) {
        return _registry[name] || null;
    }

    /** Return all registered component names. */
    function list() {
        return Object.keys(_registry);
    }

    global.JQML = {
        define:    define,
        create:    create,
        get:       get,
        list:      list,
        _registry: _registry,
    };

}(typeof window !== 'undefined' ? window : (typeof global !== 'undefined' ? global : this)));
`;

// ── Directory walker ──────────────────────────────────────────────────────────

function walkDir(dir, onFile) {
    const entries = fs.readdirSync(dir, { withFileTypes: true });
    for (const entry of entries) {
        const full = path.join(dir, entry.name);
        if (entry.isDirectory()) {
            walkDir(full, onFile);
        } else if (entry.isFile()) {
            onFile(full, entry.name);
        }
    }
}

// ── Main bundling logic ───────────────────────────────────────────────────────

/**
 * Compile all .qml and .js files in `srcDir` and write the bundle to
 * `outputFile` (typically `<srcDir>/jqml.full.js`).
 *
 * Processing order:
 *   1. JQML runtime header
 *   2. Plain .js helper files (included verbatim, sorted by path)
 *   3. Compiled .qml component definitions (sorted by path)
 *
 * @param {string} srcDir      Directory containing .qml / .js sources.
 * @param {string} outputFile  Destination for the bundled JavaScript.
 * @returns {{ files: number, errors: number }}
 */
function processDirectory(srcDir, outputFile) {
    const qmlFiles = [];
    const jsFiles  = [];

    walkDir(srcDir, (fullPath, name) => {
        if (name.endsWith('.full.js')) return; // skip previous output
        if (name.endsWith('.qml'))     qmlFiles.push(fullPath);
        else if (name.endsWith('.js')) jsFiles.push(fullPath);
    });

    qmlFiles.sort();
    jsFiles.sort();

    const parts  = [];
    const errors = [];

    parts.push(RUNTIME);

    // ── Include JS helpers ────────────────────────────────────────────────────
    for (const filePath of jsFiles) {
        const rel     = path.relative(srcDir, filePath);
        const content = fs.readFileSync(filePath, 'utf8');
        parts.push(`\n// ---- ${rel} ----\n${content}`);
    }

    // ── Compile QML components ────────────────────────────────────────────────
    for (const filePath of qmlFiles) {
        const rel           = path.relative(srcDir, filePath);
        const componentName = path.basename(filePath, '.qml');
        try {
            const src = fs.readFileSync(filePath, 'utf8');
            const ast = parse(src);
            const js  = generate(ast, componentName);
            parts.push(`\n// ---- ${rel} ----\n${js}`);
        } catch (err) {
            const msg = `// ERROR: Failed to compile ${rel}: ${err.message}`;
            errors.push(msg);
            parts.push('\n' + msg);
        }
    }

    // ── Write output ──────────────────────────────────────────────────────────
    fs.writeFileSync(outputFile, parts.join('\n'), 'utf8');

    if (errors.length > 0) {
        process.stderr.write(
            `[JQML v2] ${errors.length} file(s) had compilation errors:\n` +
            errors.join('\n') + '\n'
        );
    }

    return {
        files:  qmlFiles.length + jsFiles.length,
        qml:    qmlFiles.length,
        js:     jsFiles.length,
        errors: errors.length,
    };
}

module.exports = { processDirectory };
