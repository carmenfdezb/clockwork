#include "dynamicicon.h"
#include "clockworkglobal_p.h"
#include "dynamicicon_p.h"
#include "iconupdater.h"
#include <MDesktopEntry>
#include <MGConfItem>
#include <QDebug>
#include <QDir>
#include <QLibrary>

namespace {

QList<QMetaObject> dynamicIconsMeta;

void loadDynamicIconPlugins()
{
    QDir pluginsDir("/usr/share/clockwork/dynamic-icons");
    const auto pluginsInfo = pluginsDir.entryInfoList({"*.so"}, QDir::Files);

    for (const auto &pluginInfo : pluginsInfo) {
        QLibrary library(pluginInfo.absoluteFilePath());
        if (library.load()) {
            qDebug() << "Loaded plugin:" << pluginInfo.fileName();
        } else {
            qWarning() << "Could not load" << pluginInfo.fileName()
                       << "plugin:" << library.errorString();
        }
    }
}

} // namespace

DynamicIconPrivate::DynamicIconPrivate(const QString &packageName,
                                       const QString &name,
                                       DynamicIcon *parent)
    : QObject(parent)
    , name(name)
    , packageName(packageName)
{
    desktopPath = QStringLiteral("/usr/share/applications/%1.desktop").arg(packageName);

    MDesktopEntry desktop(desktopPath);
    displayName = desktop.name();

    const auto dconfPath = QStringLiteral("/com/dseight/clockwork/applications/%1/provider")
                               .arg(packageName);

    applicationProvider = new MGConfItem(dconfPath, this);

    connect(applicationProvider, &MGConfItem::valueChanged, parent, &DynamicIcon::enabledChanged);
}

DynamicIcon::DynamicIcon(const QString &packageName, const QString &name, QObject *parent)
    : QObject(parent)
    , d_ptr(new DynamicIconPrivate(packageName, name, this))
{
}

QString DynamicIcon::name()
{
    return d_ptr->name;
}

QString DynamicIcon::packageName()
{
    return d_ptr->packageName;
}

QString DynamicIcon::displayName()
{
    return d_ptr->displayName;
}

bool DynamicIcon::available()
{
    return QFile::exists(d_ptr->desktopPath);
}

bool DynamicIcon::enabled()
{
    return d_ptr->applicationProvider->value().toString() == name();
}

void DynamicIcon::setEnabled(bool enabled)
{
    d_ptr->applicationProvider->set(enabled ? name() : QString());
}

IconProvider *DynamicIcon::iconProvider()
{
    return iconProvider(this);
}

IconUpdater *DynamicIcon::iconUpdater()
{
    return iconUpdater(iconProvider(), d_ptr->desktopPath, this);
}

IconUpdater *DynamicIcon::iconUpdater(IconProvider *provider,
                                      const QString &desktopPath,
                                      QObject *parent)
{
    return new IconUpdater(provider, desktopPath, parent);
}

void registerDynamicIconMeta(const QMetaObject &meta)
{
    dynamicIconsMeta.append(meta);
}

const QList<DynamicIcon *> loadDynamicIcons()
{
    static QList<DynamicIcon *> dynamicIcons;

    if (!dynamicIcons.isEmpty())
        return dynamicIcons;

    loadDynamicIconPlugins();

    for (const auto &meta : qAsConst(dynamicIconsMeta)) {
        auto dynamicIcon = qobject_cast<DynamicIcon *>(meta.newInstance());

        if (!dynamicIcon) {
            qWarning() << "Could not create instance of" << meta.className()
                       << ". Did you forget to add Q_INVOKABLE to constructor?";
            continue;
        }

        dynamicIcons.append(dynamicIcon);
        qDebug() << "Loaded dynamic icon:" << meta.className();
    }

    return dynamicIcons;
}
