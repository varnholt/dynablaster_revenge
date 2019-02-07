#ifndef KEYBOARDMAPPER_H
#define KEYBOARDMAPPER_H

#include <QObject>
#include <QMap>
#include <QSet>

class KeyboardMapper : public QObject
{
   Q_OBJECT

public:
   
   static KeyboardMapper* getInstance();

   QString getKeyString(Qt::Key) const;

   Qt::Key getFromString(const QString&, bool* valid = 0) const;

   bool isKeyAllowed(Qt::Key key);

   bool isModifiedKeyAllowed(const QString &key);

   bool isModifiedKeyForbidden(const QString& key);


signals:


public slots:

   
protected:

   KeyboardMapper(QObject *parent = 0);

   void initMap();
   void initAllowedKeys();


protected:


   QMap<Qt::Key, QString> mKeyMap;
   QSet<Qt::Key> mAllowedKeys;
   static KeyboardMapper* sInstance;
};

#endif // KEYBOARDMAPPER_H
