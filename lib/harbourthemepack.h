#ifndef HARBOURTHEMEPACK_H
#define HARBOURTHEMEPACK_H

#include "iconpack.h"
#include <QHash>

class HarbourThemePack : public IconPack
{
public:
    explicit HarbourThemePack(const QString &path, QObject *parent = nullptr);

    QString displayName() override;
    QStringList icons() override;
    QStringList previewIcons() override;
    QString iconByPackageName(const QString &packageName) override;
    QString iconByActivity(const QString &activity) override;
    QImage requestIcon(const QString &iconId, const QSize &requestedSize) override;

    static QList<IconPack *> loadAll();

private:
    QString findJollaIcon(const QString &iconName);
    QString findNativeIcon(const QString &iconName);
    QString findApkIcon(const QString &iconName);

private:
    QString m_path;
    QString m_displayName;
    QStringList m_previewIcons;

    // Icon name -> scale/size mappings (e.g. "icon-launcher-shell" -> "z1.75")
    QHash<QString, QString> m_jollaIcons;
    QHash<QString, QString> m_nativeIcons;
    QHash<QString, QString> m_apkIcons;
};

#endif // HARBOURTHEMEPACK_H
