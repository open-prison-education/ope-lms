#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

/**
 * @brief ConfigManager - Singleton class for managing application configuration
 * 
 * This class handles reading and validating the config.json file,
 * determining the data storage path, and providing error handling
 * for configuration issues.
 */
class ConfigManager
{
public:
    /**
     * @brief Get the singleton instance of ConfigManager
     * @return Reference to the ConfigManager instance
     */
    static ConfigManager& instance();
    
    /**
     * @brief Load and validate configuration from config.json
     * @param showErrors If true, shows error dialogs using showConfigError function.
     * @return true if configuration loaded successfully, false otherwise
     */
    bool loadConfig(bool showErrors = true);
    
    /**
     * @brief Get the configured data path
     * @return The data path (e.g., "D:/ProgramData/OPE")
     */
    QString getDataPath() const;
    
    /**
     * @brief Check if configuration is valid
     * @return true if configuration is loaded and valid
     */
    bool isValid() const;
    
    /**
     * @brief Get the configured drive letter
     * @return The drive letter (e.g., "D")
     */
    QString getDriveLetter() const;

private:
    // Private constructor for singleton pattern
    ConfigManager() = default; 
    
    // Disable copy constructor and assignment operator
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    /**
     * @brief Read the drive letter from config.json
     * @param showErrors If true, shows error dialogs using showConfigError function.
     * @return The drive letter, or empty string if error
     */
    QString readConfigDrive(bool showErrors = true);
    
    /**
     * @brief Validate that the specified drive exists and is writable
     * @param driveLetter The drive letter to validate
     * @param showErrors If true, shows error dialogs using showConfigError function.  
     * @return true if drive is valid and writable
     */
    bool validateDrive(const QString& driveLetter, bool showErrors = true);
    
    /**
     * @brief Show configuration error message and exit application
     * @param message The error message to display
     * @param showErrors If true, shows error dialogs.
     */
    void showConfigError(const QString& message, bool showErrors = true);
    
    // Member variables
    QString m_dataPath;
    QString m_driveLetter;
    bool m_configValid;
};

#endif // CONFIGMANAGER_H
