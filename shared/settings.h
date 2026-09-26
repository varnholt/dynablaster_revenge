#pragma once

#include <QString>
#include <QStringList>

#include <map>
#include <memory>

class SettingsValue
{
public:
   using SettingsMap = std::map<QString, QString>;

   SettingsValue() = default;
   explicit SettingsValue(const QString& text);

   int toInt(bool* ok = nullptr) const;
   float toFloat(bool* ok = nullptr) const;
   bool toBool() const;
   QString toString() const;
   QStringList toStringList() const;
   SettingsMap toMap() const;

private:
   QString _text;
   bool _valid = false;
};

class Settings
{
public:
   using SettingsMap = SettingsValue::SettingsMap;

   enum Format
   {
      IniFormat
   };

   explicit Settings(const QString& filename, Format format = IniFormat);
   virtual ~Settings();

   void beginGroup(const QString& group);
   void endGroup();

   QStringList childKeys() const;

   SettingsValue value(const QString& key) const;
   SettingsValue value(const QString& key, const QString& default_value) const;
   SettingsValue value(const QString& key, const char* default_value) const;
   SettingsValue value(const QString& key, int default_value) const;
   SettingsValue value(const QString& key, float default_value) const;
   SettingsValue value(const QString& key, double default_value) const;
   SettingsValue value(const QString& key, bool default_value) const;

   void setValue(const QString& key, const QString& value);
   void setValue(const QString& key, const char* value);
   void setValue(const QString& key, int value);
   void setValue(const QString& key, float value);
   void setValue(const QString& key, double value);
   void setValue(const QString& key, bool value);
   void setValue(const QString& key, const SettingsMap& map);

   void sync();

private:
   struct SharedFile
   {
      QString filename;
      std::map<QString, QString> values;
   };

   QString qualifiedKey(const QString& key) const;
   void save() const;

   static std::shared_ptr<SharedFile> acquire(const QString& filename);

   std::shared_ptr<SharedFile> _file;
   QStringList _group_stack;
};
