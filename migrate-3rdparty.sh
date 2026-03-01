#!/bin/bash

# Migration script for converting 3rdParty libraries to separate git repositories
# This script helps prepare each library for pushing to its own GitHub repository

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
GITHUB_ORG="ImagingTools"
THIRD_PARTY_DIR="3rdParty"
TEMP_DIR="/tmp/3rdparty-migration"

# List of libraries to migrate
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

# Function to create repository for a library
prepare_library_repo() {
    local lib_name=$1
    local lib_path="${THIRD_PARTY_DIR}/${lib_name}"
    local temp_repo="${TEMP_DIR}/${lib_name}"
    local repo_name="${lib_name}"
    local repo_url="https://github.com/${GITHUB_ORG}/${repo_name}.git"

    print_info "Preparing ${lib_name}..."

    # Check if library exists
    if [ ! -d "${lib_path}" ]; then
        print_error "Library not found: ${lib_path}"
        return 1
    fi

    # Create temporary directory for the repository
    mkdir -p "${temp_repo}"
    
    # Copy library contents
    print_info "Copying ${lib_name} to temporary location..."
    cp -r "${lib_path}"/* "${temp_repo}/"
    
    # Initialize git repository
    cd "${temp_repo}"
    
    if [ ! -d ".git" ]; then
        print_info "Initializing git repository..."
        git init
        
        # Create README if it doesn't exist
        if [ ! -f "README.md" ]; then
            create_readme "${lib_name}" > README.md
        fi
        
        # Add all files
        git add .
        
        # Create initial commit
        git commit -m "Initial commit of ${lib_name} library from ImtCore/3rdParty

This library was extracted from the ImtCore repository to enable
independent version control and easier dependency management.

Source: ImagingTools/ImtCore/3rdParty/${lib_name}"
        
        print_info "Repository prepared at: ${temp_repo}"
        print_info "To push to GitHub, run:"
        echo "  cd ${temp_repo}"
        echo "  git remote add origin ${repo_url}"
        echo "  git branch -M main"
        echo "  git push -u origin main"
        echo ""
    else
        print_warning "Git repository already exists in ${temp_repo}"
    fi
    
    cd - > /dev/null
}

# Function to create a basic README for a library
create_readme() {
    local lib_name=$1
    cat << EOF
# ${lib_name}

This repository contains the **${lib_name}** library used by ImtCore.

## Origin

This library was extracted from the ImtCore repository (\`3rdParty/${lib_name}\`) 
to enable independent version control and easier dependency management.

## Usage

This repository is integrated into ImtCore as a git submodule:

\`\`\`bash
git submodule add https://github.com/${GITHUB_ORG}/${lib_name}.git 3rdParty/${lib_name}
\`\`\`

## Updating

To update this library in ImtCore:

1. Make changes in this repository
2. Commit and push changes
3. In ImtCore repository:
   \`\`\`bash
   cd 3rdParty/${lib_name}
   git pull origin main
   cd ../..
   git add 3rdParty/${lib_name}
   git commit -m "Update ${lib_name} submodule"
   git push
   \`\`\`

## License

See LICENSE file if present, or refer to the original library's licensing terms.
EOF
}

# Function to create all library repositories
prepare_all_libraries() {
    print_info "Starting migration of ${#LIBRARIES[@]} libraries..."
    
    # Create temp directory
    mkdir -p "${TEMP_DIR}"
    
    for lib in "${LIBRARIES[@]}"; do
        prepare_library_repo "${lib}"
    done
    
    print_info "All libraries prepared!"
    print_info "Temporary repositories created in: ${TEMP_DIR}"
    echo ""
    print_warning "Next steps:"
    echo "1. Create GitHub repositories for each library under ${GITHUB_ORG}"
    echo "2. Push each temporary repository to its corresponding GitHub repository"
    echo "3. Run the add-submodules script to integrate them back into ImtCore"
}

# Function to generate submodule addition script
generate_add_submodules_script() {
    local script_file="add-submodules.sh"
    
    print_info "Generating submodule addition script..."
    
    cat > "${script_file}" << 'EOF'
#!/bin/bash

# Script to add all 3rdParty libraries as git submodules
# Run this after creating and populating all GitHub repositories

set -e

GITHUB_ORG="ImagingTools"

# Libraries to add as submodules
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

echo "Adding submodules..."

for lib in "${LIBRARIES[@]}"; do
    repo_url="https://github.com/${GITHUB_ORG}/${lib}.git"
    submodule_path="3rdParty/${lib}"
    
    echo "Adding ${lib}..."
    git submodule add "${repo_url}" "${submodule_path}"
done

echo "Initializing and updating submodules..."
git submodule init
git submodule update

echo "Done! All submodules added."
echo "Don't forget to commit the changes:"
echo "  git commit -m 'Convert 3rdParty libraries to git submodules'"
EOF

    chmod +x "${script_file}"
    print_info "Created ${script_file}"
}

# Function to show usage
show_usage() {
    cat << EOF
Usage: $0 [COMMAND]

Commands:
    prepare-all     Prepare all libraries (create temp git repos)
    prepare LIB     Prepare a specific library
    generate-script Generate the add-submodules.sh script
    list            List all libraries to be migrated
    help            Show this help message

Examples:
    $0 prepare-all              # Prepare all libraries
    $0 prepare boost            # Prepare only boost library
    $0 generate-script          # Generate add-submodules.sh script
    $0 list                     # List all libraries

EOF
}

# Main script logic
case "${1:-}" in
    prepare-all)
        prepare_all_libraries
        generate_add_submodules_script
        ;;
    prepare)
        if [ -z "${2:-}" ]; then
            print_error "Please specify a library name"
            show_usage
            exit 1
        fi
        prepare_library_repo "$2"
        ;;
    generate-script)
        generate_add_submodules_script
        ;;
    list)
        print_info "Libraries to migrate:"
        for lib in "${LIBRARIES[@]}"; do
            echo "  - ${lib}"
        done
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        print_error "Unknown command: ${1:-}"
        show_usage
        exit 1
        ;;
esac
