# EU Cyber Resilience Act (CRA) Compliance

## Overview

This document describes how the ImtCore (ImagingTools Core Library) project complies with the EU Cyber Resilience Act (Regulation (EU) 2024/2847).

## Product Information

**Product Name**: ImtCore - ImagingTools Core Library  
**Product Type**: Software library/framework (open-source)  
**License**: GNU Lesser General Public License v2.1 (LGPL-2.1)  
**Manufacturer**: ImagingTools GmbH  
**Product Category**: Software development framework

## CRA Requirements Compliance

### 1. Security Requirements (Article 10)

#### 1.1 Secure by Design

ImtCore follows secure development practices:

- **Input Validation**: Components include parameter validation mechanisms
- **Type Safety**: Strongly typed parameter interfaces reduce runtime errors
- **Memory Safety**: Uses Qt framework's memory management features
- **Code Review**: Changes undergo review process before integration

#### 1.2 Security Updates

- Security updates are provided for supported versions
- Critical vulnerabilities are addressed with priority
- Security advisories are published through GitHub Security Advisories
- See [SECURITY.md](SECURITY.md) for our security update policy

#### 1.3 Vulnerability Handling

- Documented process for reporting vulnerabilities (see SECURITY.md)
- Response timeline: acknowledgment within 72 hours
- Resolution target: within 90 days based on severity
- Coordinated disclosure with security researchers

### 2. Vulnerability Management (Article 11)

#### 2.1 Identification

- Regular dependency monitoring
- Security scanning in development workflow
- Community-driven vulnerability reports
- Automated security checks via GitHub tools

#### 2.2 Documentation

- Known vulnerabilities documented in security advisories
- CVE identifiers assigned for critical issues
- Mitigation guidance provided in advisories
- Fixes tracked in release notes

#### 2.3 Remediation

- Security patches released as updates
- Hotfixes for critical vulnerabilities
- Backports to supported versions when feasible
- Migration guidance for deprecated features

### 3. Transparency Requirements (Article 13)

#### 3.1 Product Documentation

- Comprehensive API documentation: https://imagingtools.github.io/ImtCore/
- Component framework guide and usage examples
- Build and installation instructions
- Best practices documentation

#### 3.2 Security Information

- Security policy: [SECURITY.md](SECURITY.md)
- Supported versions clearly documented
- Known limitations and security considerations
- Safe usage guidelines

#### 3.3 Software Bill of Materials (SBOM)

ImtCore's main dependencies:

**Runtime Dependencies:**
- ACF Framework (version 1.0+) - ImagingTools GmbH (LGPL-2.1)
- Qt Framework (version 6.8) - Qt Project (LGPL/GPL)
- Standard C++ library (implementation-dependent)

**Build Dependencies:**
- CMake 3.x or later (BSD License)
- QMake (part of Qt)
- Doxygen (GPL) - for documentation generation

SBOM can be generated using standard tools:
- For CMake builds: Use `cmake --graphviz` or SBOM generation tools
- For package distributions: Use platform-specific SBOM tools

See [SBOM.md](SBOM.md) for detailed SBOM information.

#### 3.4 Open Source Compliance

- Full source code available: https://github.com/ImagingTools/ImtCore
- License: LGPL-2.1 (see [LICENSE](LICENSE))
- Dependencies are clearly documented
- Build from source instructions provided

### 4. Support and Maintenance (Article 14)

#### 4.1 Support Period

- **Current Release**: Full support including security updates
- **Previous Release**: Security updates for critical issues
- **Older Releases**: Community support, no official security updates

#### 4.2 End of Support

- When a version reaches end of support, it will be announced in:
  - GitHub releases
  - Repository README
  - Project documentation
  - Minimum 6 months notice for planned end of support

#### 4.3 Maintenance Activities

- Bug fixes and security patches
- Compatibility updates for dependencies
- Documentation improvements
- Performance optimizations

### 5. Conformity Assessment

#### 5.1 Self-Assessment

As an open-source software library under LGPL-2.1:

- ImtCore provides building blocks for applications
- End-product manufacturers are responsible for their own CRA compliance
- ImtCore maintains documentation to support downstream compliance
- Security practices align with CRA requirements

#### 5.2 Third-Party Dependencies

Users must ensure compliance for:

- ACF Framework (from ImagingTools GmbH)
- Qt Framework (from Qt Project)
- Platform-specific system libraries
- Any additional dependencies added by downstream users

#### 5.3 Risk Assessment

ImtCore is designed as a framework with:

- **Low inherent risk**: Framework provides tools, doesn't handle sensitive data directly
- **User responsibility**: Applications built with ImtCore must implement their own security controls
- **Extensibility considerations**: Users must review security of custom components

### 6. Incident Response

#### 6.1 Security Incident Handling

In case of a security incident:

1. **Detection**: Through reports or automated scanning
2. **Assessment**: Evaluate severity and impact
3. **Communication**: Notify affected users via GitHub advisories
4. **Remediation**: Develop and release fix
5. **Disclosure**: Publish advisory after fix is available

#### 6.2 Communication Channels

- GitHub Security Advisories
- Release notes
- Project documentation updates
- Direct notification to known affected users when possible

### 7. Documentation and Records

#### 7.1 Available Documentation

- Technical documentation: https://imagingtools.github.io/ImtCore/
- Security policy: [SECURITY.md](SECURITY.md)
- License information: [LICENSE](LICENSE)
- Build instructions in documentation
- API reference documentation

#### 7.2 Change Management

- Version control via Git
- Release notes for each version
- Change tracking in commit history
- Security-relevant changes highlighted

### 8. User Responsibilities

When using ImtCore, developers are responsible for:

#### 8.1 Application Security

- Implementing authentication and authorization
- Validating user inputs in their applications
- Securing data storage and transmission
- Following secure coding practices

#### 8.2 Dependency Management

- Keeping ImtCore, ACF, and Qt updated
- Monitoring security advisories
- Testing updates before deployment
- Managing their own SBOM

#### 8.3 Compliance

- Ensuring their products comply with CRA
- Documenting their use of ImtCore in their SBOM
- Implementing their own security measures
- Maintaining their own support processes

### 9. Limitations and Disclaimers

#### 9.1 No Warranty

As stated in the LGPL-2.1 license, ImtCore is provided "AS IS" without warranty. See [LICENSE](LICENSE) for full terms.

#### 9.2 Security Limitations

- ImtCore provides tools; security depends on correct usage
- No framework can prevent all security issues
- Users must implement application-specific security
- Regular updates are essential for security

#### 9.3 Liability

Liability limitations are defined in the LGPL-2.1 license.

### 10. Contact Information

**Security Issues**: See [SECURITY.md](SECURITY.md) for reporting procedures  
**General Questions**: GitHub Issues  
**Project Repository**: https://github.com/ImagingTools/ImtCore  
**Documentation**: https://imagingtools.github.io/ImtCore/

### 11. Updates to This Document

This CRA compliance document is reviewed and updated regularly to reflect:

- Changes in CRA requirements or guidance
- Updates to the project's security practices
- New features or capabilities
- Lessons learned from security incidents

**Version**: 1.0  
**Last Updated**: February 2026  
**Next Review**: August 2026

## References

- [EU Cyber Resilience Act - Regulation (EU) 2024/2847](https://eur-lex.europa.eu/)
- [ImtCore Security Policy](SECURITY.md)
- [ImtCore Documentation](https://imagingtools.github.io/ImtCore/)
- [GNU LGPL v2.1 License](LICENSE)
- [NIST Secure Software Development Framework (SSDF)](https://csrc.nist.gov/Projects/ssdf)

## Certification Status

**Note**: As an open-source library, ImtCore is designed to support CRA compliance but does not itself undergo certification. Products incorporating ImtCore must undergo their own conformity assessment as required by the CRA.
