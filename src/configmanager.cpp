#include "configmanager.h"
#include <QDebug>

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(bool showErrors)
{
    // Reset state
    m_configValid = false;
    m_dataPath.clear();
    m_driveLetter.clear();
    
    // Read drive from config file
    QString configDrive = readConfigDrive(showErrors);
    if (configDrive.isEmpty()) {
        // Error already handled by readConfigDrive
        return false;
    }
    
    // Validate the configured drive
    if (!validateDrive(configDrive, showErrors)) {
        // Error already handled by validateDrive
        return false;
    }
    
    // Build the data path using the configured drive
    m_driveLetter = configDrive;
    m_dataPath = configDrive + ":/ProgramData/OPE";
    m_configValid = true;
    
    return true;
}

QString ConfigManager::getDataPath() const
{
    return m_dataPath;
}

bool ConfigManager::isValid() const
{
    return m_configValid;
}

QString ConfigManager::getDriveLetter() const
{
    return m_driveLetter;
}

QString ConfigManager::readConfigDrive(bool showErrors)
{
    // Get the application directory path using QDir
    QDir appDir = QDir::current();
    QString configPath = appDir.absoluteFilePath("config.json");
    
    // Check if config file exists
    QFileInfo configFile(configPath);
    if (!configFile.exists()) {
        showConfigError("Configuration file not found. Please ensure config.json exists in the application directory.\n\n" + configPath, showErrors);
        return "";
    }
    
    // Read the config file
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        showConfigError("Cannot open configuration file. Please check file permissions.\n\n" + configPath, showErrors);
        return "";
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    // Parse JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        showConfigError("Invalid configuration file format. Please check config.json syntax.\n\n" + configPath, showErrors);
        return "";
    }
    
    QJsonObject obj = doc.object();
    if (!obj.contains("appdata_drive")) {
        showConfigError("Configuration file missing 'appdata_drive' field.\n\n" + configPath, showErrors);
        return "";
    }
    
    QString driveLetter = obj["appdata_drive"].toString();
    if (driveLetter.isEmpty()) {
        showConfigError("Configuration file 'appdata_drive' field is empty. It should contain drive letter such C, and D\n\n" + configPath, showErrors);
        return "";
    }
    
    // Clean up drive letter (remove colon if present)
    driveLetter = driveLetter.toUpper().replace(":", "");
    
    return driveLetter;
}

bool ConfigManager::validateDrive(const QString& driveLetter, bool showErrors)
{
    if (driveLetter.isEmpty()) {
        return false;
    }
    
    // Check if drive exists
    QString drivePath = driveLetter + ":/";
    QFileInfo driveInfo(drivePath);
    if (!driveInfo.exists()) {
        showConfigError("Drive " + drivePath + ": not found. Please check the drive letter in config.json.", showErrors);
        return false;
    }
    
    // Test write permissions by trying to create a test directory
    QString testPath = drivePath + "ProgramData/OPE/.write_test";
    QDir testDir;
    if (!testDir.mkpath(QFileInfo(testPath).absolutePath())) {
        showConfigError("Cannot create directories on drive " + drivePath + ": Please check drive permissions or contact IT support.", showErrors);
        return false;
    }
    
    // Test write permissions by creating a temporary file
    QFile testFile(testPath);
    if (!testFile.open(QIODevice::WriteOnly)) {
        showConfigError("No write access to drive " + drivePath + ": Please check drive permissions or contact IT support.", showErrors);
        return false;
    }
    testFile.close();
    testFile.remove(); // Clean up test file
    
    return true;
}

void ConfigManager::showConfigError(const QString& message, bool showErrors)
{
    if (showErrors) {
#if defined( Q_OS_WIN )
    MessageBoxW(NULL,
                reinterpret_cast<LPCWSTR>(message.utf16()),
                    L"Configuration Error",
                    MB_OK | MB_ICONERROR);
#endif
    }
    // Exit the application
    QCoreApplication::quit();
}
