# Figma Design Tokens Integration

## Overview

The ImtCore design token system now supports importing design tokens from Figma. This allows seamless integration between Figma designs and the ImtCore styling system.

## Supported Formats

The Figma token processor supports two main design token formats:

1. **Figma Tokens Plugin Format** - Standard format used by the Figma Tokens plugin
2. **W3C Design Tokens Format** - Standard format with `$` prefixed properties

## Token Structure

### Colors

Figma color tokens can be defined in several ways:

```json
{
  "color": {
    "primary": {
      "value": "#2A82DA",
      "type": "color"
    },
    "secondary": {
      "$value": "#335777",
      "$type": "color"
    }
  }
}
```

### Typography

Font tokens follow this structure:

```json
{
  "typography": {
    "heading1": {
      "value": {
        "fontFamily": "Arial",
        "fontSize": "24px",
        "fontWeight": "bold",
        "fontStyle": "normal"
      }
    }
  }
}
```

### Token References

Tokens can reference other tokens using curly braces:

```json
{
  "color": {
    "primary": {
      "value": "#2A82DA"
    },
    "button": {
      "background": {
        "value": "{color.primary}"
      }
    }
  }
}
```

## Usage

### Command Line

To convert Figma tokens to ImtCore `.theme` format:

```bash
DesignTokenCreator --figma <path-to-figma-tokens.json> -OD <output-dir> -P <project-name>
```

Example:

```bash
DesignTokenCreator --figma my-design-tokens.json -OD ./output -P MyApp
```

This will create a file named `MyApp_figma.theme` in the output directory.

### CMake Integration

In your CMakeLists.txt:

```cmake
imtcore_design_token_creator(
    PROJECT_NAME MyApp
    FIGMA_TOKENS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tokens/figma-tokens.json"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
```

## Output Format

The Figma processor converts tokens into the standard ImtCore `.theme` format:

- **ColorPalette**: All color tokens are extracted and flattened
- **Fonts**: Typography tokens are converted to Qt font definitions
- **Style**: A complete style definition with color groups (Active, Inactive, Disabled)

## Example

### Input (Figma Tokens)

```json
{
  "color": {
    "primary": {
      "value": "#2A82DA"
    },
    "background": {
      "value": "#FFFFFF"
    },
    "text": {
      "value": "#333333"
    }
  },
  "typography": {
    "body": {
      "value": {
        "fontFamily": "Arial",
        "fontSize": "14px",
        "fontWeight": "400"
      }
    },
    "heading": {
      "value": {
        "fontFamily": "Arial",
        "fontSize": "24px",
        "fontWeight": "700"
      }
    }
  }
}
```

### Output (.theme format)

```json
{
  "ColorPalette": {
    "primary": "#2A82DA",
    "background": "#FFFFFF",
    "text": "#333333"
  },
  "Style": {
    "Name": "Figma",
    "Fonts": {
      "body": {
        "Family": "Arial",
        "PointSize": 14,
        "Bold": false
      },
      "heading": {
        "Family": "Arial",
        "PointSize": 24,
        "Bold": true
      }
    },
    "ActiveColors": {
      "Background": "background",
      "Text": "text",
      "Highlight": "primary"
    }
  }
}
```

## Best Practices

1. **Organize tokens hierarchically** - Use nested structures for better organization
2. **Use semantic naming** - Name tokens by purpose (e.g., `button.background`) not by value
3. **Leverage references** - Use token references to maintain consistency
4. **Test conversions** - Always verify the generated `.theme` file matches your expectations

## Troubleshooting

### Common Issues

**Issue**: "Figma token file does not exist"
- **Solution**: Verify the file path is correct and the file is readable

**Issue**: "Failed to parse Figma token JSON"
- **Solution**: Validate your JSON syntax using a JSON validator

**Issue**: "No colors extracted"
- **Solution**: Ensure your color tokens have a `value` or `$value` field

## Integration with Build Systems

The Figma processor integrates seamlessly with the existing ImtCore build system. After conversion, the generated `.theme` files can be used with all existing processors:

- Icon processor for SVG colorization
- StyleSheet processor for CSS variable replacement
- QRC generators for Qt resource compilation

## Further Reading

- [Design Tokens Community Group](https://designtokens.org/)
- [Figma Tokens Plugin Documentation](https://docs.tokens.studio/)
- [ImtCore Design System Overview](./DesignSystemOverview.md)
