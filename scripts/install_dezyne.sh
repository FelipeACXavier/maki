#!/bin/bash
# verum-dezyne-installer.sh

set -e

# Version configuration - automatically generated
VERUM_DEZYNE_VERSION="2.19.2"
ARCHITECTURE="x86_64-linux"

# Derived paths based on version
DEFAULT_INSTALL_DIR="$HOME/verum-dezyne-$VERUM_DEZYNE_VERSION"
INSTALL_DIR=""
DEZYNE_INSTALLER_NAME="verum-dezyne-$VERUM_DEZYNE_VERSION-$ARCHITECTURE-installer"
TAR_FILE="$HOME/$DEZYNE_INSTALLER_NAME/verum-dezyne-$VERUM_DEZYNE_VERSION-$ARCHITECTURE.tar.gz"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}Verum Dezyne $VERUM_DEZYNE_VERSION Installer${NC}"
echo "================================="

# Function to ask user a yes/no question (same as Debian postinst)
ask_user() {
    local question="$1"
    local default="${2:-n}"

    # Check for explicit non-interactive mode
    if [ -n "$DEBIAN_FRONTEND" ] && [ "$DEBIAN_FRONTEND" = "noninteractive" ]; then
        echo "$default"
        return
    fi

    # Try to use the controlling terminal directly
    if [ -c /dev/tty ]; then
        while true; do
            if [ "$default" = "y" ]; then
                printf "%s [Y/n]: " "$question" >/dev/tty
            else
                printf "%s [y/N]: " "$question" >/dev/tty
            fi

            read -r answer </dev/tty

            # Use default if empty
            if [ -z "$answer" ]; then
                answer="$default"
            fi

            case "$answer" in
                [Yy]|[Yy][Ee][Ss])
                    echo "y"
                    return
                    ;;
                [Nn]|[Nn][Oo])
                    echo "n"
                    return
                    ;;
                *)
                    echo "Please answer yes or no." >/dev/tty
                    ;;
            esac
        done
    else
        # No terminal available, use default
        echo "$default"
        return
    fi
}

# Get installation directory from user
get_install_directory() {
    if [ -z "$DEBIAN_FRONTEND" ] || [ "$DEBIAN_FRONTEND" != "noninteractive" ]; then
        echo ""
        echo -e "${YELLOW}Installation Directory${NC}"
        echo "Please specify the parent directory where you want to install Verum Dezyne."
        echo "A 'verum-dezyne-$VERUM_DEZYNE_VERSION' subdirectory will be created inside this path."
        echo "Press Enter for default location: $HOME"
        echo ""
        read -p "Install directory: " PARENT_DIR
    fi

    # Use default if empty
    if [ -z "$PARENT_DIR" ]; then
        PARENT_DIR="$HOME"
    fi

    # Expand ~ to home directory
    PARENT_DIR="${PARENT_DIR/#\~/$HOME}"

    # Create the full installation path
    INSTALL_DIR="$PARENT_DIR/verum-dezyne-$VERUM_DEZYNE_VERSION"

    echo ""
    echo "Installing to: $INSTALL_DIR"
    echo ""
}

# Create AppArmor profile (same as Debian postinst - simple unconfined profile)
create_apparmor_profile() {
    local install_path="$1"

    # Convert install path to AppArmor profile name (replace / with .)
    # Remove leading / and replace remaining / with .
    local profile_name=$(echo "$install_path" | sed 's|^/||' | sed 's|/|.|g')
    local profile_file="/etc/apparmor.d/$profile_name"

    # Create a temporary file with the AppArmor profile using the ACTUAL installation directory
    cat > /tmp/verum-dezyne-apparmor << EOF
abi <abi/4.0>,
include <tunables/global>

profile $install_path $install_path/{dzn,dzn-env,guile,ide} flags=(unconfined) {
  userns,
}
EOF

    # Install the profile with path-based name
    sudo cp /tmp/verum-dezyne-apparmor "$profile_file"
    sudo apparmor_parser -r "$profile_file" || true

    # Store profile file name for uninstaller
    echo "$profile_file" > "$install_path/.apparmor_profile"

    # Clean up
    rm -f /tmp/verum-dezyne-apparmor
}

# Main installation function
install_verum_dezyne() {
    local install_path="$1"

    echo "Creating installation directory..."
    mkdir -p "$install_path"

    echo "Extracting tar archive..."
    # Extract tar file directly to installation path
    # Use --strip-components=1 to remove the top-level directory from the archive
    tar -xzf "$TAR_FILE" -C "$install_path" --strip-components=1

    echo "Setting permissions..."
    chmod +x "$install_path"/dzn 2>/dev/null || true
    chmod +x "$install_path"/ide 2>/dev/null || true
    chmod +x "$install_path"/dzn-env 2>/dev/null || true
    chmod +x "$install_path"/guile 2>/dev/null || true

    echo -e "${GREEN}✓ Verum Dezyne $VERUM_DEZYNE_VERSION installed at: $install_path${NC}"
}

# Function to add to PATH via .bashrc (same logic as Debian postinst)
add_to_path_for_user() {
    local install_path="$1"
    local bashrc="$HOME/.bashrc"
    local path_line="export PATH=\"$install_path:\$PATH\"  # Verum Dezyne $VERUM_DEZYNE_VERSION"

    # Check if already in PATH
    if [ -f "$bashrc" ] && grep -q "Verum Dezyne" "$bashrc"; then
        # Remove old PATH entries
        sed -i '/# Verum Dezyne/d' "$bashrc"
    fi

    # Add new PATH entry
    echo "" >> "$bashrc"
    echo "$path_line" >> "$bashrc"

    echo -e "${GREEN}✓ Added to PATH. Run 'source ~/.bashrc' or start a new terminal to use 'dzn' and 'ide' commands${NC}"

    source "$bashrc"
}

# Function to setup PATH for user (same logic as Debian postinst)
setup_path_for_user() {
    local install_path="$1"

    # Check if executables exist (they're in the root directory)
    local dzn_path="$install_path/dzn"
    local ide_path="$install_path/ide"

    if [ ! -x "$dzn_path" ] && [ ! -x "$ide_path" ]; then
        echo "Note: dzn and ide executables not found in $install_path, skipping PATH setup"
        return
    fi

    if [ ! -x "$dzn_path" ]; then
        echo "Warning: dzn executable not found at $dzn_path"
    fi

    if [ ! -x "$ide_path" ]; then
        echo "Warning: ide executable not found at $ide_path"
    fi

    echo ""  # Add spacing for better readability
    local response=$(ask_user "Add Verum Dezyne to PATH for easy access to dzn and ide commands?" "y")

    if [ "$response" = "y" ]; then
        add_to_path_for_user "$install_path"
    else
        echo "Skipping PATH setup. You can manually run commands using full paths:"
        echo "  $install_path/dzn"
        echo "  $install_path/ide"
    fi
}

# Uninstaller function (enhanced to handle AppArmor removal)
create_uninstaller() {
    local install_path="$1"

    cat > "$install_path/uninstall.sh" << 'UNINSTALL_EOF'
#!/bin/bash

# Function to ask user a yes/no question
ask_user() {
    local question="$1"
    local default="${2:-n}"

    # Try to use the controlling terminal directly
    if [ -c /dev/tty ]; then
        while true; do
            if [ "$default" = "y" ]; then
                printf "%s [Y/n]: " "$question" >/dev/tty
            else
                printf "%s [y/N]: " "$question" >/dev/tty
            fi

            read -r answer </dev/tty

            # Use default if empty
            if [ -z "$answer" ]; then
                answer="$default"
            fi

            case "$answer" in
                [Yy]|[Yy][Ee][Ss])
                    echo "y"
                    return
                    ;;
                [Nn]|[Nn][Oo])
                    echo "n"
                    return
                    ;;
                *)
                    echo "Please answer yes or no." >/dev/tty
                    ;;
            esac
        done
    else
        # No terminal available, use default
        echo "$default"
        return
    fi
}

echo "Verum Dezyne Uninstaller"
echo "========================"

# Get the installation directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "Installation directory: $SCRIPT_DIR"
echo ""

# Ask user for confirmation
response=$(ask_user "Are you sure you want to completely remove Verum Dezyne and all its files?" "n")

if [ "$response" != "y" ]; then
    echo "Uninstallation cancelled."
    exit 0
fi

echo ""
echo "Uninstalling Verum Dezyne..."

# Function to check if AppArmor was installed during setup
had_apparmor_profile() {
    # Check if AppArmor is enabled in the kernel
    if [ -f /sys/module/apparmor/parameters/enabled ]; then
        local apparmor_enabled=$(cat /sys/module/apparmor/parameters/enabled 2>/dev/null)
        if [ "$apparmor_enabled" = "Y" ]; then
            return 0
        fi
    fi

    # Alternative check: see if AppArmor is in the active LSM list
    if [ -f /sys/kernel/security/lsm ]; then
        local active_lsms=$(cat /sys/kernel/security/lsm 2>/dev/null)
        if [[ "$active_lsms" == *"apparmor"* ]]; then
            return 0
        fi
    fi

    return 1
}

# Remove AppArmor profile if it was installed
if had_apparmor_profile; then
    # Check for stored profile file path
    if [ -f "$SCRIPT_DIR/.apparmor_profile" ]; then
        profile_file=$(cat "$SCRIPT_DIR/.apparmor_profile")
        if [ -f "$profile_file" ]; then
            echo "Removing AppArmor profile: $profile_file"
            if command -v apparmor_parser &> /dev/null; then
                sudo apparmor_parser -R "$profile_file" 2>/dev/null || true
            fi
            sudo rm -f "$profile_file"
            echo "AppArmor profile removed."
        else
            echo "AppArmor profile reference found but file doesn't exist."
        fi
    else
        echo "No AppArmor profile reference found."
    fi
else
    echo "AppArmor profile not applicable for this system."
fi

# Remove PATH entries from shell config files
for config_file in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
    if [ -f "$config_file" ] && grep -q "Verum Dezyne" "$config_file"; then
        echo "Cleaning PATH entries from: $config_file"
        sed -i '/# Verum Dezyne/d' "$config_file"
    fi
done

# Remove installation directory
echo "Removing installation directory: $SCRIPT_DIR"
cd /
rm -rf "$SCRIPT_DIR"

echo "Verum Dezyne uninstalled successfully."
echo ""
echo "Note: You may need to restart your terminal or run 'source ~/.bashrc'"
echo "to update your PATH if Verum Dezyne was added to it."
UNINSTALL_EOF

    chmod +x "$install_path/uninstall.sh"
}

# Main execution
main() {
    # Check if running as root
    if [ "$EUID" -eq 0 ]; then
        echo -e "${RED}Please don't run this installer as root.${NC}"
        echo "The installer will ask for sudo permissions when needed."
        exit 1
    fi

    # Check if tar file exists
    if [ ! -f "$TAR_FILE" ]; then
        echo -e "${RED}Error: Verum Dezyne tar file not found: $TAR_FILE${NC}"
        echo "Expected: $TAR_FILE"
        exit 1
    fi

    # Get installation directory
    get_install_directory

    # Create parent directory if it doesn't exist
    if [ ! -d "$(dirname "$INSTALL_DIR")" ]; then
        echo "Creating parent directory: $(dirname "$INSTALL_DIR")"
        mkdir -p "$(dirname "$INSTALL_DIR")"
    fi

    # Check if installation directory already exists
    if [ -d "$INSTALL_DIR" ]; then
        echo -e "${YELLOW}Warning: Installation directory $INSTALL_DIR already exists.${NC}"
        local response=$(ask_user "Replace existing Verum Dezyne installation?" "y")
        if [ "$response" != "y" ]; then
            echo "Installation cancelled."
            exit 0
        fi
        echo "Removing existing installation..."
        rm -rf "$INSTALL_DIR"
    fi

    # Show installation summary
    echo ""
    echo -e "${BLUE}Installation Summary:${NC}"
    echo "Source archive: $TAR_FILE"
    echo "Installation path: $INSTALL_DIR"
    echo ""

    local response=$(ask_user "Continue with installation?" "y")
    if [ "$response" != "y" ]; then
        echo "Installation cancelled."
        exit 0
    fi

    # Install Verum Dezyne
    echo ""
    echo -e "${GREEN}Starting installation...${NC}"
    install_verum_dezyne "$INSTALL_DIR"

    # Install AppArmor profile if needed
    # install_apparmor_profile "$INSTALL_DIR"

    # Setup PATH
    setup_path_for_user "$INSTALL_DIR"

    # Create uninstaller
    create_uninstaller "$INSTALL_DIR"

    echo ""
    echo -e "${GREEN}Installation completed successfully!${NC}"
    echo ""
    echo "Verum Dezyne is installed in: $INSTALL_DIR"
    echo "To run Verum Dezyne:"
    echo "  Command line: $INSTALL_DIR/dzn"
    echo "  IDE: $INSTALL_DIR/ide"
    echo "To uninstall, run: $INSTALL_DIR/uninstall.sh"
    echo ""

    exit 0;
}

# Get the installer
wget https://downloads.verum.com/download/verum-dezyne/$DEZYNE_INSTALLER_NAME.tar.gz -O $DEZYNE_INSTALLER_NAME.tar.gz

# Extract the archive
tar xf $DEZYNE_INSTALLER_NAME.tar.gz -C "$HOME"

# Run main function
main "$@"
