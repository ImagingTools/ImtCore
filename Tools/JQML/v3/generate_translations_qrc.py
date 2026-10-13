import os
import sys

# rcc accepts absolute file paths, so translations can be embedded straight from their source
# location - no need to copy them into the web build dir first.

def main():
    if len(sys.argv) < 2:
        print("usage: generate_translations_qrc.py <output.qrc> [translations_dir ...]")
        return

    output_path = sys.argv[1]
    translation_dirs = sys.argv[2:]

    files = {}
    for translations_dir in translation_dirs:
        if not os.path.isdir(translations_dir):
            continue
        for name in os.listdir(translations_dir):
            if name.endswith('.ts'):
                files[name] = os.path.join(translations_dir, name)

    lines = ['<RCC>']
    if files:
        lines.append('    <qresource prefix="/Translations">')
        for name in sorted(files):
            lines.append('        <file alias="{0}">{1}</file>'.format(name, files[name].replace('\\', '/')))
        lines.append('    </qresource>')
    lines.append('</RCC>')

    output_dir = os.path.dirname(output_path)
    if output_dir and not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    with open(output_path, 'w', encoding='utf-8') as qrc_file:
        qrc_file.write('\n'.join(lines) + '\n')

    print("Generated {0} with {1} translation file(s)".format(output_path, len(files)))


if __name__ == '__main__':
    main()
