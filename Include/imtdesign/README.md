# ImtCore Design Token System

## Overview

The ImtCore design token system provides a comprehensive framework for managing design tokens, enabling consistent styling across applications. The system supports multiple input formats and generates optimized resources for Qt-based applications.

## Features

- **Multiple Input Formats**: Support for custom `.theme` format and **Figma design tokens**
- **Automatic Resource Generation**: Creates Qt resource files (.qrc) for icons, stylesheets, and themes
- **Icon Processing**: SVG colorization with multiple icon states (normal, active, disabled, selected)
- **StyleSheet Processing**: CSS variable replacement with theme-specific values
- **Font Management**: Complete font definition support with Qt font properties
- **Split Mode**: Separate theme files per style for optimized loading

## Architecture

### Core Modules

#### imtdesign
Build-time processing module for design tokens:
- `CDesignTokenFileParserComp`: Parses `.theme` JSON files
- `CFigmaTokenProcessorComp`: **NEW** - Converts Figma tokens to `.theme` format
- `CDesignTokenIconProcessorComp`: Processes SVG icons with color replacement
- `CDesignTokenStyleSheetProcessorComp`: Processes CSS files with token variables
- `CDesignTokenQrcUpdaterComp`: Generates/updates Qt resource files
- `CDesignTokenCommonQrcGeneratorComp`: Creates common resource files
- `CDesignTokenWebQrcGeneratorComp`: Creates web-specific resource files

#### imtstyle
Runtime module for style management:
- `CDesignTokenBasedResourceProviderComp`: Provides design tokens at runtime
- `CImtStyle`: Qt proxy style implementation
- `CDesignManagerComp`: Central design system manager

## Theme File Format

Theme files use JSON format with the following structure:

```json
{
  "ColorPalette": {
    "ColorName": "#HexValue"
  },
  "Style": {
    "Name": "StyleName",
    "IconTemplateList": [...],
    "Fonts": {...},
    "ActiveColors": {...},
    "InactiveColors": {...},
    "DisabledActiveColors": {...},
    "DisabledInActiveColors": {...}
  }
}
```

See `Include/imtstyle/light.theme` and `dark.theme` for complete examples.

## Figma Integration (NEW)

The system now supports importing design tokens directly from Figma!

### Supported Formats
- **Figma Tokens Plugin format**: Standard format with `value` and `type` fields
- **W3C Design Tokens format**: Standard format with `$value` and `$type` fields

### Usage

#### Command Line
```bash
DesignTokenCreator --figma tokens.json -OD output/ -P MyApp
```

#### CMake
```cmake
imtcore_design_token_creator(
    PROJECT_NAME MyApp
    FIGMA_TOKENS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tokens.json"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
```

### Example Figma Tokens

See `Include/imtstyle/Resources/figma-tokens-example.json` for a complete example.

### Token Features
- **Color tokens**: Automatic extraction from hierarchical structure
- **Typography tokens**: Font family, size, weight, and style
- **Token references**: Supports `{color.primary}` style references
- **Nested structures**: Automatically flattened to color palette

For detailed Figma integration documentation, see [Docs/FigmaDesignTokensIntegration.md](../Docs/FigmaDesignTokensIntegration.md).

## Build System Integration

### Command Line Arguments

```
DesignTokenCreator [options]

Options:
  -F, --file <path>           Theme file path(s)
  --figma <path>              Figma tokens JSON file (NEW)
  -I <path>                   Images input directory
  -S <path>                   Style sheets input directory
  -OD <path>                  Output directory
  -OF <path>                  Output file path
  -P <name>                   Project name
  --split                     Split theme into separate files
  -C, --copy                  Copy theme file to output
  -W                          Generate web resources
  --common                    Generate common resources
  --qml                       Generate QML icon definitions
  --ignore-errors             Continue on errors
  -h, --help, /?              Show help
```

### CMake Integration

```cmake
include(${IMTCOREDIR}/Config/CMake/DesignTokenCreator.cmake)

imtcore_design_token_creator(
    PROJECT_NAME MyApp
    THEME_FILES "${CMAKE_CURRENT_SOURCE_DIR}/theme/light.theme"
                "${CMAKE_CURRENT_SOURCE_DIR}/theme/dark.theme"
    FIGMA_TOKENS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tokens/figma.json"
    IMAGES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/icons"
    STYLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/styles"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated"
    GENERATE_WEB ON
    GENERATE_COMMON ON
)
```

## Workflow

### Standard Workflow
1. Create/maintain `.theme` files with color palettes and styles
2. Add SVG icons to images directory
3. Add CSS stylesheets with variable placeholders
4. Run `DesignTokenCreator` during build
5. Qt resource files (.qrc) are generated
6. Resources are compiled into application via `rcc`

### Figma Workflow (NEW)
1. Export design tokens from Figma (using Figma Tokens plugin)
2. Run `DesignTokenCreator --figma tokens.json`
3. Generated `.theme` file can be used with standard workflow
4. Optionally: Use generated theme with icon/stylesheet processors

## Icon Processing

Icons support 8 states with automatic color replacement:
- Normal
- OffNormal
- OffDisabled
- OffActive
- OffSelected
- OnNormal
- OnDisabled
- OnActive
- OnSelected

Icon template colors (default: `#D9D9D9`) are replaced with theme-specific colors.

## StyleSheet Processing

CSS files can use variable placeholders that are replaced with theme values:

```css
.button {
  background-color: @Background@;
  color: @Text@;
  font: @font.Default@;
}
```

Variables are replaced with actual hex colors and font definitions from the theme.

## Testing

### Validate Figma Tokens
```bash
cd Tests
bash test_figma_tokens.sh
```

This script:
- Validates JSON format
- Shows token structure
- Tests conversion (if executable is built)
- Verifies output theme file

## File Locations

- **Design token processor**: `Include/imtdesign/`
- **Figma processor**: `Include/imtdesign/CFigmaTokenProcessorComp.*`
- **Runtime style system**: `Include/imtstyle/`
- **Theme examples**: `Include/imtstyle/*.theme`
- **Figma token example**: `Include/imtstyle/Resources/figma-tokens-example.json`
- **Executable**: `Impl/DesignTokenCreatorExe/`
- **Build configuration**: `Config/CMake/DesignTokenCreator.cmake`
- **Package definition**: `Impl/ImtDesignPck/`

## Best Practices

1. **Organize by semantic meaning**: Use names like `Primary`, `Background`, not `Blue`, `Gray`
2. **Use token references**: In Figma tokens, reference base tokens to maintain consistency
3. **Version control themes**: Keep theme files in version control
4. **Test theme changes**: Always build and test after modifying themes
5. **Document custom colors**: Add comments in theme files for custom palette colors
6. **Leverage Figma**: Use Figma as the source of truth for design tokens

## Troubleshooting

### Issue: Colors not updating in application
- **Solution**: Clean build directory and rebuild to regenerate resources

### Issue: Figma tokens not converting
- **Solution**: Validate JSON format and ensure proper structure (see examples)

### Issue: Icons not colorized
- **Solution**: Ensure SVG icons use the template color (#D9D9D9)

### Issue: Font not loading
- **Solution**: Verify font family is installed on the system

## Future Enhancements

- Support for more Figma token types (spacing, shadows, etc.)
- Direct Figma API integration
- Real-time theme preview
- Theme validation tools
- Additional output formats (CSS variables, SCSS, etc.)

## References

- [Figma Design Tokens Integration Guide](../Docs/FigmaDesignTokensIntegration.md)
- [Design Tokens Community Group](https://designtokens.org/)
- [Figma Tokens Plugin](https://docs.tokens.studio/)
- ImtCore Library Documentation
