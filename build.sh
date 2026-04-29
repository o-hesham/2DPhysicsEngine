#!/bin/bash
# 2D Physics Engine - Linux/macOS Build Script

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Project info
PROJECT_NAME="2DPhysicsEngine"
VERSION="1.0.0"

echo -e "${BLUE}========================================"
echo -e "2D Physics Engine - Linux/macOS Build"
echo -e "========================================${NC}"

# Function to print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_info "Checking build dependencies..."
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_error "'make' not found. Please install build tools."
        exit 1
    fi
    
    # Check for g++
    if ! command -v g++ &> /dev/null; then
        print_error "'g++' not found. Please install GCC."
        exit 1
    fi
    
    # Check for SDL2 (optional check)
    if ! pkg-config --exists sdl2 2>/dev/null; then
        print_warning "SDL2 development libraries may not be installed."
        print_info "On Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-image-dev"
        print_info "On Fedora: sudo dnf install SDL2-devel SDL2_image-devel"
        print_info "On macOS: brew install sdl2 sdl2_image"
    fi
    
    print_success "Dependencies check completed."
}

# Show help
show_help() {
    echo "Usage: $0 [OPTIONS] [TARGET]"
    echo ""
    echo "OPTIONS:"
    echo "  --debug       Build in debug mode"
    echo "  --release     Build in release mode (default)"
    echo "  --cmake       Use CMake instead of Make"
    echo "  --clean       Clean before building"
    echo "  --help        Show this help message"
    echo ""
    echo "TARGETS:"
    echo "  all           Build the project (default)"
    echo "  debug         Build in debug mode"
    echo "  release       Build in release mode"
    echo "  run           Build and run the application"
    echo "  clean         Clean build artifacts"
    echo "  install       Install the executable"
    echo "  help          Show make help"
    echo ""
    echo "EXAMPLES:"
    echo "  $0                    # Release build with Make"
    echo "  $0 --debug           # Debug build with Make"
    echo "  $0 --cmake release   # Release build with CMake"
    echo "  $0 run               # Build and run"
    echo "  $0 --clean all       # Clean and build"
}

# Parse command line arguments
BUILD_MODE="release"
BUILD_SYSTEM="make"
CLEAN_FIRST=false
TARGET="all"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_MODE="debug"
            shift
            ;;
        --release)
            BUILD_MODE="release"
            shift
            ;;
        --cmake)
            BUILD_SYSTEM="cmake"
            shift
            ;;
        --clean)
            CLEAN_FIRST=true
            shift
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            TARGET="$1"
            shift
            ;;
    esac
done

print_info "Build system: $BUILD_SYSTEM"
print_info "Build mode: $BUILD_MODE"
print_info "Target: $TARGET"

# Clean if requested
if [ "$CLEAN_FIRST" = true ]; then
    print_info "Cleaning previous build..."
    if [ "$BUILD_SYSTEM" = "cmake" ]; then
        rm -rf build/
        rm -rf bin/${PROJECT_NAME}*
    else
        make clean
    fi
    print_success "Clean completed."
fi

# Check dependencies
check_dependencies

# Build based on selected system
if [ "$BUILD_SYSTEM" = "cmake" ]; then
    print_info "Building with CMake..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure
    if [ "$BUILD_MODE" = "debug" ]; then
        cmake -DCMAKE_BUILD_TYPE=Debug ..
    else
        cmake -DCMAKE_BUILD_TYPE=Release ..
    fi
    
    # Build
    if [ "$TARGET" = "all" ] || [ "$TARGET" = "debug" ] || [ "$TARGET" = "release" ]; then
        make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    elif [ "$TARGET" = "clean" ]; then
        make clean
    elif [ "$TARGET" = "install" ]; then
        sudo make install
    elif [ "$TARGET" = "run" ]; then
        make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
        cd ..
        ./bin/${PROJECT_NAME}
    else
        make $TARGET
    fi
    
else
    print_info "Building with Make..."
    
    # Build with make
    if [ "$TARGET" = "run" ]; then
        make BUILD_MODE=$BUILD_MODE run
    elif [ "$TARGET" = "debug" ]; then
        make debug
    elif [ "$TARGET" = "release" ]; then
        make release
    else
        make $TARGET BUILD_MODE=$BUILD_MODE
    fi
fi

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
else
    print_error "Build failed!"
    exit 1
fi
