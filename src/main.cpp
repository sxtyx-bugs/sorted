#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QScreen>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextStream>
#include "ui/Theme.h"
#include "ui/MainWindow.h"

static QString getLogDirectory() {
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appData.isEmpty()) {
        appData = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    }
    if (!appData.endsWith("SORTED")) {
        appData += "/SORTED";
    }
    QString logDir = appData + "/logs";
    QDir().mkpath(logDir);
    return logDir;
}

static void logDiagnosticHeader() {
    QString logFilePath = getLogDirectory() + "/sorted_runtime.log";

    QFile file(logFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << "============================================================\n";
        out << "SORTED C++ Runtime Session: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
        out << "Executable Path: " << QCoreApplication::applicationFilePath() << "\n";
        out << "Working Dir:     " << QDir::currentPath() << "\n";
        out << "Qt Version:      " << qVersion() << "\n";
        out << "OS:              " << QSysInfo::prettyProductName() << " (" << QSysInfo::kernelType() << " " << QSysInfo::kernelVersion() << ")\n";
        out << "CPU Arch:        " << QSysInfo::currentCpuArchitecture() << " (Build: " << QSysInfo::buildCpuArchitecture() << ")\n";
        if (auto screen = QGuiApplication::primaryScreen()) {
            out << "Primary Screen:  " << screen->geometry().width() << "x" << screen->geometry().height()
                << " (DPI: " << screen->logicalDotsPerInch() << ", DPR: " << screen->devicePixelRatio() << ")\n";
        }
        out << "Library Paths:   " << QCoreApplication::libraryPaths().join("; ") << "\n";
        out << "Icon Resource:   " << (QFile::exists(":/icons/SORTED.png") ? "Found" : "MISSING") << "\n";
        out << "============================================================\n";
    }
}

int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);

    QApplication::setApplicationName("SORTED");
    QApplication::setApplicationDisplayName("SORTED");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Satydev");
    QApplication::setOrganizationDomain("satydev.com");

    // Initialize release runtime diagnostics log
    logDiagnosticHeader();

    // Register cross-thread queued metatypes
    qRegisterMetaType<ScanResult>("ScanResult");
    qRegisterMetaType<Operations::ExecutionSummary>("Operations::ExecutionSummary");
    qRegisterMetaType<std::vector<MoveOperation>>("std::vector<MoveOperation>");

    // Set window icon from bundled Qt resource
    app.setWindowIcon(QIcon(":/icons/SORTED.png"));

    // Set modern clean UI style
    app.setStyle(QStyleFactory::create("Fusion"));

    // Restore saved theme from QSettings or default to Dark mode
    QSettings settings("SORTED", "SORTED");
    QString savedTheme = settings.value("theme", "Dark").toString();
    Ui::Theme::applyTheme(savedTheme == "Light" ? Ui::ColorTheme::Light : Ui::ColorTheme::Dark);

    int exitCode = 0;
    try {
        Ui::MainWindow mainWindow;
        mainWindow.show();
        exitCode = app.exec();
    } catch (const std::exception& e) {
        QFile file(getLogDirectory() + "/sorted_runtime.log");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << "[FATAL EXCEPTION] " << QDateTime::currentDateTime().toString(Qt::ISODate) << ": " << e.what() << "\n";
        }
        exitCode = 1;
    } catch (...) {
        QFile file(getLogDirectory() + "/sorted_runtime.log");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << "[FATAL EXCEPTION] " << QDateTime::currentDateTime().toString(Qt::ISODate) << ": Unknown exception\n";
        }
        exitCode = 2;
    }

    return exitCode;
}
