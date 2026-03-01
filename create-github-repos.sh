#!/bin/bash

# Script to create GitHub repositories for all 3rdParty libraries
# Requires GitHub CLI (gh) to be installed and authenticated

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
GITHUB_ORG="ImagingTools"
REPO_PREFIX=""
VISIBILITY="public"  # Change to "private" if needed

# List of libraries
LIBRARIES=(
    "EJ"
    "JQ"
    "JQML"
    "JQML2"
    "QXlsx"
    "WebCompiler"
    "boost"
    "icu"
    "libmdbx"
    "mongoc"
    "mongocxx"
    "nodejs"
    "openssl"
    "pybind11"
    "quazip"
    "zlib"
    "zstd"
)

# Library descriptions
declare -A DESCRIPTIONS=(
    ["EJ"]="Export library for ImtCore"
    ["JQ"]="Qt-based library for ImtCore"
    ["JQML"]="QML compiler for ImtCore"
    ["JQML2"]="QML compiler v2 for ImtCore"
    ["QXlsx"]="Excel file library for ImtCore"
    ["WebCompiler"]="Python-based web compiler for ImtCore"
    ["boost"]="Boost C++ libraries (v1.75.0) for ImtCore"
    ["icu"]="International Components for Unicode (v72.1) for ImtCore"
    ["libmdbx"]="Memory-mapped database library for ImtCore"
    ["mongoc"]="MongoDB C driver (v1.23.2) for ImtCore"
    ["mongocxx"]="MongoDB C++ driver (v3.7.0) for ImtCore"
    ["nodejs"]="Node.js runtime and npm for ImtCore"
    ["openssl"]="OpenSSL cryptography library (v1.1) for ImtCore"
    ["pybind11"]="Python/C++ binding library for ImtCore"
    ["quazip"]="Qt/C++ wrapper for ZIP/UNZIP for ImtCore"
    ["zlib"]="Compression library (v1.2.13) for ImtCore"
    ["zstd"]="Zstandard compression library (v1.5.4) for ImtCore"
)

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if gh is installed
check_gh_installed() {
    if ! command -v gh &> /dev/null; then
        print_error "GitHub CLI (gh) is not installed"
        echo "Please install it from: https://cli.github.com/"
        exit 1
    fi
}

# Check if gh is authenticated
check_gh_authenticated() {
    if ! gh auth status &> /dev/null; then
        print_error "GitHub CLI is not authenticated"
        echo "Please run: gh auth login"
        exit 1
    fi
}

# Create a single repository
create_repository() {
    local lib_name=$1
    local repo_name="${lib_name}"
    local description="${DESCRIPTIONS[$lib_name]}"
    
    print_info "Creating repository: ${GITHUB_ORG}/${repo_name}"
    
    # Check if repository already exists
    if gh repo view "${GITHUB_ORG}/${repo_name}" &> /dev/null; then
        print_warning "Repository ${GITHUB_ORG}/${repo_name} already exists, skipping"
        return 0
    fi
    
    # Create repository
    if gh repo create "${GITHUB_ORG}/${repo_name}" \
        --${VISIBILITY} \
        --description "${description}" \
        --confirm; then
        print_info "Successfully created ${GITHUB_ORG}/${repo_name}"
    else
        print_error "Failed to create ${GITHUB_ORG}/${repo_name}"
        return 1
    fi
}

# Create all repositories
create_all_repositories() {
    print_info "Creating ${#LIBRARIES[@]} repositories in ${GITHUB_ORG} organization..."
    echo ""
    
    local success_count=0
    local fail_count=0
    local skip_count=0
    
    for lib in "${LIBRARIES[@]}"; do
        if create_repository "${lib}"; then
            if gh repo view "${GITHUB_ORG}/${lib}" &> /dev/null; then
                ((success_count++))
            else
                ((skip_count++))
            fi
        else
            ((fail_count++))
        fi
        echo ""
    done
    
    echo ""
    print_info "Summary:"
    echo "  Created: ${success_count}"
    echo "  Skipped: ${skip_count}"
    echo "  Failed: ${fail_count}"
    
    if [ $fail_count -gt 0 ]; then
        print_warning "Some repositories failed to create. Please check errors above."
        return 1
    fi
    
    print_info "All repositories created successfully!"
}

# List what would be created (dry run)
dry_run() {
    print_info "The following repositories would be created in ${GITHUB_ORG}:"
    echo ""
    
    for lib in "${LIBRARIES[@]}"; do
        repo_name="${lib}"
        description="${DESCRIPTIONS[$lib]}"
        echo "  ${GITHUB_ORG}/${repo_name}"
        echo "    Description: ${description}"
        echo "    Visibility: ${VISIBILITY}"
        echo ""
    done
}

# Show usage
show_usage() {
    cat << EOF
Usage: $0 [COMMAND]

Commands:
    create      Create all GitHub repositories
    dry-run     Show what would be created without creating
    help        Show this help message

Examples:
    $0 dry-run      # Preview what will be created
    $0 create       # Create all repositories

Prerequisites:
    - GitHub CLI (gh) must be installed
    - You must be authenticated: gh auth login
    - You must have permission to create repos in ${GITHUB_ORG}

EOF
}

# Main script logic
main() {
    case "${1:-}" in
        create)
            check_gh_installed
            check_gh_authenticated
            create_all_repositories
            ;;
        dry-run)
            dry_run
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            if [ -n "${1:-}" ]; then
                print_error "Unknown command: $1"
                echo ""
            fi
            show_usage
            exit 1
            ;;
    esac
}

main "$@"
