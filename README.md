# OPE LMS - Open Prison Education Learning Management System

OPE LMS is a desktop application designed to synchronize with Canvas LMS for incarcerated students, allowing them to view Canvas materials offline. This application provides a secure, offline-capable learning environment specifically tailored for correctional education settings.

## Overview

The Open Prison Education Learning Management System (OPE LMS) is built using Qt6 and provides:

- **Offline Canvas Integration**: Sync with Canvas LMS to download course materials for offline viewing
- **Secure Environment**: Designed for use in correctional facilities with appropriate security measures
- **Local Web Server**: Built-in HTTP server to serve cached content locally
- **Platform Support**: Runs on Windows 10/11
- **Modern UI**: Built with Qt Quick Controls 2 and QML for a responsive user interface

## Features

- **Course Management**: View and navigate through Canvas courses offline
- **Assignment Handling**: Download and submit assignments
- **File Management**: Access course files, documents, and media offline
- **Discussion Forums**: Participate in course discussions
- **Quiz Support**: Take quizzes offline with automatic sync when online (WIP)
- **Announcements**: View course announcements offline (WIP)
- **Web Content**: Embedded web browser for viewing HTML content

## System Requirements

### Minimum Requirements
- **Operating System**: Windows 10/11
- **RAM**: 4 GB minimum, 8 GB recommended
- **Storage**: 2 GB free space for application and cached content
- **Network**: Internet connection required for initial sync and periodic updates

### Development Requirements
- **Qt6**: Version 6.8.3
- **qmake**: Qt's build system (included with Qt6)
- **C++ Compiler**: C++17 compatible compiler (MSVC 2022 64-bit)
- **OpenSSL**: For secure network communications

## Dependencies

This application uses the following Qt6 modules:

### Core Modules
- **Qt6::Core**: Core functionality and event system
- **Qt6::Gui**: GUI framework and graphics
- **Qt6::Widgets**: Widget-based UI components
- **Qt6::Network**: Network communication and HTTP requests
- **Qt6::Sql**: SQLite database support
- **Qt6::Concurrent**: Multi-threading support

### UI Modules
- **Qt6::Quick**: QML and Qt Quick framework
- **Qt6::QuickControls2**: Modern UI controls

### Web Modules
- **Qt6::WebView**: Web content display
- **Qt6::WebChannel**: JavaScript-Qt communication
- **Qt6::WebEngineCore**: Web engine core functionality
- **Qt6::WebEngineQuick**: Qt Quick integration with web engine
- **Qt6::WebEngineWidgets**: Widget-based web engine components

### Authentication
- **Qt6::NetworkAuth**: OAuth2 authentication support

## Building from Source

### Prerequisites

1. **Install Qt6**: Download and install Qt6 Community Edition from [qt.io](https://www.qt.io/download-qt-installer)
   - Ensure you install Qt6 with the following components selecting MSVC 2022 64-bit:
     - Qt6 Core, Gui, Widgets
     - Qt6 Quick, QuickControls2
     - Qt6 Network, Sql, Concurrent
     - Qt6 WebView, WebChannel
     - Qt6 WebEngineCore, WebEngineQuick, WebEngineWidgets
     - Qt6 NetworkAuth
     - OpenSSL

2. **Install Git**: Download from [git-scm.com](https://git-scm.com/downloads)

3. **Visual Studio Community 2022**: Donwload from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/vs/community/)

### Optional: Build QtWebEngine with Proprietary Codecs

**Note**: The standard Qt6 Community Edition includes QtWebEngine without proprietary codec support (such as MP4 video playback). If you need full media codec support for educational content, you may need to build QtWebEngine from source with proprietary codecs enabled.

For detailed instructions on building QtWebEngine with proprietary codecs support, see [WebEngineMP4Build_6.8.3.md](WebEngineMP4Build_6.8.3.md).


### Build Instructions

#### Windows

1. **Clone the repository**:
   ```bash
   git clone https://github.com/open-prison-education/ope-lms.git
   cd ope-lms
   ```

2. **Open Qt Creator** or **Visual Studio 2022 Developer Command Prompt** and navigate to the project directory

3. **Using Qt Creator**:
   - Open `OPE_LMS.pro` in Qt Creator
   - Select the appropriate kit (Qt6 with MSVC)
   - Build Steps -> Add Build Step -> Custom Process Step -> Browse -> win_deploy_6.8.3.cmd
   - Click Build → Build Project

4. **Using Command Line**:
   > **Note:** Ensure that all required build tools (such as `qmake` and `jom`) are available in your system `PATH`. You may need to add the directory containing these tools to your `PATH` environment variable.

   > Usual path for qmake `C:\Qt\6.8.3\msvc2022_64\bin\qmake.exe`

   > Usual path for jom `C:\Qt\Tools\QtCreator\bin\jom\jom.exe`

   ```cmd
   REM Create build directory from project root directory and navigate to it
   mkdir build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release
   cd build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release

   REM Generate Makefile
   qmake <project_root_dir>\OPE_LMS.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
   jom qmake_all

   REM Build the project
   jom

   REM Run win_deploy to copy over Qt libraries and other files
   <project_root_dir>\win_deploy_6.8.3.cmd release
   ```

6. **Run the application**:
   ```bash
   .\release\OPE_LMS.exe
   ```

## Configuration

### Data Storage Configuration

The application uses a JSON configuration file to determine where to store data files, logs, and cached content. This allows IT staff to easily configure the application to use a specific drive without requiring application interface changes.

#### Configuration File

**File Location**: `config.json` (placed in the same directory as the application executable)

**Format**:
```json
{
  "appdata_drive": "D"
}
```

**Configuration Options**:
- `appdata_drive`: The drive letter where application data should be stored (e.g., "D", "E", "F")

#### How It Works

1. **Configuration Loading**: The `ConfigManager` singleton class loads and validates the configuration at startup
2. **Drive Validation**: The application checks if the specified drive exists and has write permissions
3. **Data Path**: Uses `{drive}:/ProgramData/OPE` as the base data directory
4. **Error Handling**: If configuration fails, the application shows an error message and exits


#### Example Configurations

**Use D: drive**:
```json
{
  "appdata_drive": "D"
}
```

**Use E: drive**:
```json
{
  "appdata_drive": "E"
}
```

#### Error Scenarios

The application will show error messages and exit in these cases:

1. **Missing config.json**: "Configuration file not found. Please ensure config.json exists in the application directory."
2. **Invalid JSON**: "Invalid configuration file format. Please check config.json syntax."
3. **Missing drive field**: "Configuration file missing 'appdata_drive' field."
4. **Drive not found**: "Drive D: not found. Please check the drive letter in config.json."
5. **No write permissions**: "No write access to drive D: Please check drive permissions or contact IT support."

### Application Settings

The application stores settings in the system registry

### Canvas Integration

1. **Canvas URL**: Configure the Canvas instance URL
2. **OAuth2 Setup**: The application uses OAuth2 for Canvas authentication
3. **API Access**: Requires Canvas API access token

### Data Storage

- **Database**: SQLite database for local data storage
- **Cache Directory**: Stores downloaded course content
- **Logs**: Application logs stored in the configured drive location

## Usage

### Initial Setup

1. **Launch the application**
2. **Authenticate with Canvas**: Use your Canvas credentials in the registry
3. **Initial Sync**: Download course materials for offline access
4. **Navigate Courses**: Browse and access course content offline

### Offline Mode

Once synced, the application can run completely offline:
- View course materials
- Complete assignments
- Participate in discussions
- Take quizzes

### Sync Process

When online, the application can:
- Download new course materials
- Upload completed assignments
- Sync quiz responses (WIP)
- Update messages and discussions

## Troubleshooting

### Common Issues

1. **qmake Not Found**: 
   ```
   'qmake' is not recognized as an internal or external command
   ```
   **Solution**: 
   - Ensure Qt6 is properly installed
   - Add Qt6 bin directory to your PATH: `C:\Qt\6.8.3\msvc2022_64\bin`
   - Or use Qt Creator which automatically sets up the environment

2. **Build Errors**: Ensure all Qt6 modules are installed
3. **Runtime Errors**: Check that OpenSSL libraries are available
4. **Canvas Connection**: Verify Canvas URL and credentials
5. **Permission Issues**: Run with appropriate user permissions

### Log Files

Check log files in:
- **Windows**: `{configured_drive}:\ProgramData\OPE\tmp\log\lms_app_debug.log`
  - The exact path depends on the drive specified in `config.json`
  - Example: If `config.json` specifies `"appdata_drive": "D"`, logs will be at `D:\ProgramData\OPE\tmp\log\ope-lms.log`

## Contributing

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Code Style

- Follow Qt coding conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Ensure all code compiles without warnings

## License

This project is licensed under the **GNU General Public License v3.0** (GPL-3.0).

### Open Source Components

This application uses Qt6 Community Edition, which is licensed under:

- **Qt6 Core Libraries**: GNU Lesser General Public License v3.0 (LGPL-3.0)
- **Qt6 Additional Libraries**: Various licenses (see Qt documentation)
- **OpenSSL**: Apache License 2.0 and SSLeay License

## Support

### Documentation

- **Qt6 Documentation**: [doc.qt.io](https://doc.qt.io/qt-6/)
- **qmake Documentation**: [doc.qt.io/qt-6/qmake-manual.html](https://doc.qt.io/qt-6/qmake-manual.html)

### Community

- **Issues**: Report bugs and feature requests via GitHub Issues
- **Discussions**: Use GitHub Discussions for questions and community support

---

**Note**: This application is specifically designed for correctional education environments. Please ensure compliance with all applicable laws and regulations when deploying in institutional settings.