#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include <QSettings>
#include "ui/Theme.h"
#include "ui/MainWindow.h"

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

    // Register cross-thread queued metatypes
    qRegisterMetaType<Ui::ScanResult>("ScanResult");
    qRegisterMetaType<Ui::ScanResult>("Ui::ScanResult");
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

    Ui::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
