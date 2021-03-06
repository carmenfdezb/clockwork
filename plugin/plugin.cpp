#include "appgridiconprovider.h"
#include "clockworkimageprovider.h"
#include "dynamiciconsmodel.h"
#include "iconpacksmodel.h"
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QTranslator>
#include <QtGlobal>
#include <QtQml>

#ifndef VERSION_STRING
#define VERSION_STRING "0.0.0"
#endif

namespace {

class ClockworkSettingsTranslator : public QTranslator
{
    Q_OBJECT

public:
    explicit ClockworkSettingsTranslator(QObject *parent)
        : QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    ~ClockworkSettingsTranslator() override
    {
        qApp->removeTranslator(this);
    }
};

} // namespace

class ClockworkSettingsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.dseight.clockwork")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("com.dseight.clockwork"));

        // Always load default translations first, otherwise we'll see
        // only string IDs when running jolla-settings from terminal
        auto *defaultTranslator = new ClockworkSettingsTranslator(engine);
        defaultTranslator->load("settings-clockwork", "/usr/share/clockwork/translations");

        auto *translator = new ClockworkSettingsTranslator(engine);
        translator->load(QLocale(), "settings-clockwork", "-", "/usr/share/clockwork/translations");

        engine->addImageProvider("clockwork", new ClockworkImageProvider);
        engine->rootContext()->setContextProperty("clockworkPluginVersion", VERSION_STRING);
    }

    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("com.dseight.clockwork"));

        qmlRegisterType<DynamicIconsModel>(uri, 1, 0, "DynamicIconsModel");
        qmlRegisterType<DynamicIconsSortModel>(uri, 1, 0, "DynamicIconsSortModel");
        qmlRegisterType<IconPacksModel>(uri, 1, 0, "IconPacksModel");
        qmlRegisterType<AppGridIconProvider>(uri, 1, 0, "AppGridIconProvider");
    }
};

#include "plugin.moc"
