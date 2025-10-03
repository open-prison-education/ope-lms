// Make sure to include windows.h first so it doesn't cause conflicts
#include <windows.h>
#include <QGuiApplication>
#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QtWebView/QtWebView>
#include <QtWebEngineCore>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QIcon>
#include <QtGlobal>
#include <QtDebug>
#include <QTextStream>
#include <QLocale>
#include <QTime>
#include <QFile>
#include <QOperatingSystemVersion>
//#include <QTranslator>

//#include "openetworkaccessmanagerfactory.h"
#include "appmodule.h"
#include "customlogger.h"
#include "configmanager.h"

// Needed to pull in windows functions
#pragma comment(lib,"user32.lib")

QT_REQUIRE_CONFIG(ssl);

QString pgdata_path = "";

int main(int argc, char *argv[])
{
    // Dummy variable to force rebuild
#define rebuilding 19;

    // Hide the console window
#if defined( Q_OS_WIN )
    ShowWindow( GetConsoleWindow(), SW_HIDE ); //hide console window
#endif

    // Set global app parameters - used by settings later
    QCoreApplication::setOrganizationName("OPE");
    QCoreApplication::setOrganizationDomain("openprisoneducation.com");
    QCoreApplication::setApplicationName("OPELMS");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#if defined( Q_OS_WIN )
    // Load configuration FIRST (before any other initialization)
    ConfigManager& config = ConfigManager::instance();
    if (!config.loadConfig()) {
        return -1;
    }
    // Set up logging with the determined path
    pgdata_path = config.getDataPath();
    log_file_path = pgdata_path + "/tmp/log/ope-lms.log";
    
    // Create the log directory if it doesn't exist
    QDir logDir(pgdata_path + "/tmp/log");
    if (!logDir.exists()) {
        logDir.mkpath(logDir.path());
    }
#else
    log_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "ope/tmp/log/ope-lms.log";
#endif

    // NOTE: Need before now?  - - Need this right after GUI App creation
    //QtWebEngine::initialize();
    //QtWebView::initialize();

    // Are we running in the Qt Creator IDE?
    QByteArray envVar = qgetenv("QTDIR");
    if (envVar.isEmpty()) {
        //qDebug() << "Running outside IDE";
        is_in_IDE = false;
        log_to_file = true;

        // Install custom log handler
        qInstallMessageHandler(customLogOutput);
        qDebug() << "Logging to: " << log_file_path;
    } else {
        qDebug() << "Running within IDE";
        is_in_IDE = true;
        log_to_file = false;
    }

    QString last_arg = "";
    last_arg = argv[argc-1];

    // Do we need to run headless (quiet_sync?)
    if (last_arg == "quiet_sync") {
        quiet_mode = true;
        // Show the console window
#if defined( Q_OS_WIN )
        ShowWindow( GetConsoleWindow(), SW_NORMAL ); //hide console window
        // SW_NORMAL - to show console
#endif
        qDebug() << "Running quiet_sync - headless mode...";
        QCoreApplication cmd_app(argc, argv);

        QQmlApplicationEngine cmd_engine;

        // -- Setup our app module which deals with QML/C++ integration
        AppModule *cmd_appModule = new AppModule(&cmd_engine, pgdata_path);

        // Sync from command line, then exit.
        if (cmd_appModule->isAppCredentialed() != true) {
            // Can't sync if app not credentialed.
            qDebug() << "ERROR - Can't sync app when not credentialed!";
            return -1;
        }

        qDebug() << "Launching syncLMSQuiet...";
        // Run sync in quiet mode then exit.
        QTimer::singleShot(1, cmd_appModule,  SLOT(syncLMSQuiet()));
        return cmd_app.exec();
    }

    QApplication app(argc, argv);
    qDebug() << "Running GUI app...";
//    QTranslator translator;
//    bool translator_ret = translator.load(":/translations_en.qm");
//    app.installTranslator(&translator);

    // Put our local folders as first path to look at for dlls
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/lib");
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    qDebug() << "Library Paths: " << QCoreApplication::libraryPaths();

    // NOTE: Need this right after GUI App creation? QtWebengine init earlier?
    QtWebView::initialize();

    app.setWindowIcon(QIcon(":/resources/images/logo_icon.ico"));

    QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.binding.removal.info=true"));


    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);


    QQmlApplicationEngine engine;

    // -- Setup our app module which deals with QML/C++ integration
    AppModule *appModule = new AppModule(&engine, pgdata_path);
    if (appModule->exitEarly()) {
        // Exit early - already running? Should quit now.
        qDebug() << "Exiting early.";
        MessageBoxW(NULL, L"Another instance of the LMS is running! It could be either LMS desktop or LMS background service. Closing this instance.", L"LMS app is already running", MB_OK | MB_ICONSTOP);

        delete appModule;
        return -1;
    }

    QString loadPage = "qrc:/lms.qml";

    bool is_app_credentialed = appModule->isAppCredentialed();
    if (is_app_credentialed != true) {
        // Load the error page for non credentialed apps
        loadPage = "qrc:/not_credentialed.qml";
    }

    bool need_sync = false;
    if (last_arg == "sync" || appModule->hasAppSycnedWithCanvas() != true)
    {
        need_sync = true;
    }

    // Set the need_sync attribute
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("need_sync"), need_sync);

    //engine.load(QUrl(QLatin1String("qrc:/dropTest.qml")));
    engine.load(QUrl(loadPage));

    int e = app.exec();
    // Needed to exit when a console window exists but is hidden
    // Don't kill parent console - it kills process too?
    // exit(e);
    return e;
}
