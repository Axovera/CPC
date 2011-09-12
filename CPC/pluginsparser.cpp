#include "pluginsparser.h"

#include <QFile>
#include <QDebug>
#include <windows.h>

#include "constants.h"
#include "plugins.h"

PluginsParser::PluginsParser(Plugins* aPlugins, const QString& aOS)
{
    plugins = aPlugins;
    os = aOS ;
}

bool PluginsParser:: readFile (const QString& fileName){
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
         qDebug() << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString());
        return(false);
    }

    reader.setDevice(&file);

    reader.readNext();
    while (!reader.atEnd()) {

        if (reader.isStartElement() && reader.name() == COEIA_PLUGINS) {
            reader.readNext();
            reader.readNext();

            if ( reader.name() == METADATA) {
                reader.readNext();
                reader.readNext();

                if ( reader.name() == PLUGINS_NAME) {
                    QString pluginName = reader.readElementText();
                    reader.readNext();
                    reader.readNext();

                    QString pluginCategory = reader.readElementText();
                    reader.readNext();
                    reader.readNext();

                    QString pluginVersion = reader.readElementText();
                    reader.readNext();
                    reader.readNext();

                    plugins->metaData.pluginsName = pluginName;
                    plugins->metaData.category = pluginCategory;
                    plugins->metaData.pluginsVersion = pluginVersion;
                }
            }
        }
        else if ( reader.isStartElement() && reader.name() == ITEMS ) {
            readItemsElement();
        }

        reader.readNext();
    }

    file.close();

    if(reader.hasError()) {
        qDebug() << "Error: Failed to parse file"
                 << qPrintable(fileName) << " : "
                 << qPrintable(reader.errorString()) ;
        return (false);
    }
    else if (file.error() != QFile::NoError) {
        qDebug() << "Error: Cannot read file " << qPrintable(fileName)
                 << " : " << qPrintable(file.errorString());
        return (false);
    }

    return (true);
}

void PluginsParser:: readItemsElement(){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == ITEM) {
                PluginsItem item;
                readItemElement(item);
                plugins->pluginsItems.append(item);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readItemElement(PluginsItem& item){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == ITEM_NAME) {
                QString name = reader.readElementText();
                item.name = name;
                reader.readNext();
            }
            else if (reader.name() == OBJECTS) {
                readObjectsElement(item);
            }
            else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readObjectsElement(PluginsItem& item){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == OBJECT) {
                PluginsObject object;
                readObjectElement(object);
                item.pluginsObject.append(object);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readObjectElement(PluginsObject& object){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == OBJECT_TYPE) {
                QString type = reader.readElementText();
                object.type = type;
                reader.readNext();
            }
            else if (reader.name() == OBJECT_COMMAND) {
                QString command = reader.readElementText();
                object.command = command;
                reader.readNext();
            }
            else if (reader.name() == OBJECT_PATH) {
               readPathElement(object.path);
            }
            else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readPathElement(PluginsPath& path){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == OBJECT_OS && (reader.attributes().value(OS_NAME).toString() == os ||
                                               reader.attributes().value(OS_NAME).toString() == "ALL" ) ){
                readOSElement(path);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readOSElement(PluginsPath& path){
    path.os = os;

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == OBJECT_LOCATION) {
                QString location = reader.readElementText();
                path.location = location;
                reader.readNext();
            }
            else if (reader.name() == OBJECT_PATTERNS) {
                readPatternsElement(path);
            }
            else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: readPatternsElement(PluginsPath& path){
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == OBJECT_PATTERN) {
                QString pattern = reader.readElementText();
                path.patterns.append(pattern);
                reader.readNext();
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void PluginsParser:: skipUnknownElement() {
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}
