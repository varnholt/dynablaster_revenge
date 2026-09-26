#include "settings.h"

#include <QFile>
#include <QTextStream>

#include <map>

namespace
{
constexpr QChar map_field_separator(0x1f);
constexpr QChar map_record_separator(0x1e);
}  // namespace

SettingsValue::SettingsValue(const QString& text) : _text(text), _valid(true)
{
}

int SettingsValue::toInt(bool* ok) const
{
   return _text.toInt(ok);
}

float SettingsValue::toFloat(bool* ok) const
{
   return _text.toFloat(ok);
}

bool SettingsValue::toBool() const
{
   return _text.compare("true", Qt::CaseInsensitive) == 0 || _text == "1";
}

QString SettingsValue::toString() const
{
   return _text;
}

QStringList SettingsValue::toStringList() const
{
   if (_text.isEmpty())
   {
      return {};
   }

   return _text.split(',');
}

SettingsValue::SettingsMap SettingsValue::toMap() const
{
   SettingsMap map;

   const auto records = _text.split(map_record_separator);
   for (const auto& record : records)
   {
      const auto fields = record.split(map_field_separator);
      if (fields.size() == 2)
      {
         map[fields[0]] = fields[1];
      }
   }

   return map;
}

std::shared_ptr<Settings::SharedFile> Settings::acquire(const QString& filename)
{
   // process-wide registry so every Settings instance pointed at the same
   // file shares one in-memory state, matching QSettings' own per-path cache -
   // otherwise the last instance destroyed would clobber every other
   // instance's writes with its own load-time snapshot.
   static std::map<QString, std::weak_ptr<SharedFile>> reg;

   auto it = reg.find(filename);
   if (it != reg.end())
   {
      if (auto existing = it->second.lock())
      {
         return existing;
      }
   }

   auto file = std::make_shared<SharedFile>();
   file->filename = filename;

   QFile io(filename);
   if (io.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream stream(&io);

      QString section;

      while (!stream.atEnd())
      {
         QString line = stream.readLine().trimmed();

         if (line.isEmpty() || line.startsWith('#') || line.startsWith(';'))
         {
            continue;
         }

         if (line.startsWith('[') && line.endsWith(']'))
         {
            section = line.mid(1, line.size() - 2);
            continue;
         }

         const auto separator_index = line.indexOf('=');
         if (separator_index < 0)
         {
            continue;
         }

         const auto key = line.left(separator_index).trimmed();
         const auto value = line.mid(separator_index + 1).trimmed();

         const auto qualified = section.isEmpty() ? key : section + "/" + key;
         file->values[qualified] = value;
      }
   }

   reg[filename] = file;

   return file;
}

Settings::Settings(const QString& filename, Format /*format*/) : _file(acquire(filename))
{
}

Settings::~Settings()
{
   save();
}

void Settings::beginGroup(const QString& group)
{
   _group_stack.push_back(group);
}

void Settings::endGroup()
{
   if (!_group_stack.isEmpty())
   {
      _group_stack.removeLast();
   }
}

QString Settings::qualifiedKey(const QString& key) const
{
   if (_group_stack.isEmpty())
   {
      return key;
   }

   return _group_stack.join('/') + "/" + key;
}

QStringList Settings::childKeys() const
{
   const auto prefix = qualifiedKey(QString());
   QStringList keys;

   for (const auto& entry : _file->values)
   {
      if (!entry.first.startsWith(prefix))
      {
         continue;
      }

      const auto remainder = entry.first.mid(prefix.size());
      if (!remainder.contains('/'))
      {
         keys << remainder;
      }
   }

   return keys;
}

SettingsValue Settings::value(const QString& key) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   if (it == values.end())
   {
      return SettingsValue();
   }

   return SettingsValue(it->second);
}

SettingsValue Settings::value(const QString& key, const QString& default_value) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   return SettingsValue(it == values.end() ? default_value : it->second);
}

SettingsValue Settings::value(const QString& key, const char* default_value) const
{
   return value(key, QString(default_value));
}

SettingsValue Settings::value(const QString& key, int default_value) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   return SettingsValue(it == values.end() ? QString::number(default_value) : it->second);
}

SettingsValue Settings::value(const QString& key, float default_value) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   return SettingsValue(it == values.end() ? QString::number(default_value) : it->second);
}

SettingsValue Settings::value(const QString& key, double default_value) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   return SettingsValue(it == values.end() ? QString::number(default_value) : it->second);
}

SettingsValue Settings::value(const QString& key, bool default_value) const
{
   const auto& values = _file->values;
   const auto it = values.find(qualifiedKey(key));
   return SettingsValue(it == values.end() ? QString(default_value ? "true" : "false") : it->second);
}

void Settings::setValue(const QString& key, const QString& value)
{
   _file->values[qualifiedKey(key)] = value;
   save();
}

void Settings::setValue(const QString& key, const char* value)
{
   setValue(key, QString(value));
}

void Settings::setValue(const QString& key, int value)
{
   setValue(key, QString::number(value));
}

void Settings::setValue(const QString& key, float value)
{
   setValue(key, QString::number(value));
}

void Settings::setValue(const QString& key, double value)
{
   setValue(key, QString::number(value));
}

void Settings::setValue(const QString& key, bool value)
{
   setValue(key, QString(value ? "true" : "false"));
}

void Settings::setValue(const QString& key, const SettingsMap& map)
{
   QString text;

   for (const auto& entry : map)
   {
      if (!text.isEmpty())
      {
         text += map_record_separator;
      }

      text += entry.first;
      text += map_field_separator;
      text += entry.second;
   }

   setValue(key, text);
}

void Settings::sync()
{
   save();
}

void Settings::save() const
{
   QFile io(_file->filename);
   if (!io.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream stream(&io);

   std::map<QString, std::vector<std::pair<QString, QString>>> sections;

   for (const auto& entry : _file->values)
   {
      const auto separator_index = entry.first.lastIndexOf('/');

      QString section;
      QString key = entry.first;

      if (separator_index >= 0)
      {
         section = entry.first.left(separator_index);
         key = entry.first.mid(separator_index + 1);
      }

      sections[section].emplace_back(key, entry.second);
   }

   for (const auto& section_entry : sections)
   {
      if (!section_entry.first.isEmpty())
      {
         stream << "[" << section_entry.first << "]\n";
      }

      for (const auto& key_value : section_entry.second)
      {
         stream << key_value.first << "=" << key_value.second << "\n";
      }

      stream << "\n";
   }
}
