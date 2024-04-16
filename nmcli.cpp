#include "nmcli.h"

namespace nmcli
{

bool available()
{
  QProcess process;
  process.start("nmcli", QStringList() << "-version");
  process.waitForFinished();

  if(process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

QString exec(const QStringList &parameters)
{
  QProcess process;
  process.start("nmcli", parameters);
  process.waitForFinished();
  return QString::fromUtf8(process.readAllStandardOutput());
}

QStringList getFields(QString line)
{
  QStringList fields;
  QString currentField;
  bool escapeNextChar = false;

  for (QChar ch : line)
  {
    if(escapeNextChar)
    {
      // If the previous character was an escape character, add the current character to the field.
      currentField.append(ch);
      escapeNextChar = false;
    }
    else if(ch == '\\')
    {
      // If the current character is an escape character, set the flag to escape the next character.
      escapeNextChar = true;
    }
    else if(ch == ':')
    {
      // If the current character is a colon (and not escaped), it's a delimiter.
      fields.append(currentField);
      currentField.clear();
    }
    else
    {
      // Otherwise, just add the current character to the field.
      currentField.append(ch);
    }
  }

  // Add the last field to the result if it's not empty.
  if(!currentField.isEmpty())
  {
    fields.append(currentField);
  }

  return fields;
}

}
