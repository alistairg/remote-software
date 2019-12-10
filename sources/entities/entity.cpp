#include "entity.h"
#include "../config.h"

#include <QtDebug>
#include <QTimer>

EntityInterface::~EntityInterface()
{
}

Entity::Entity(const QString& type, const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent) :
    QObject(parent),
    m_integrationObj(integrationObj),
    m_type(type),
    m_area(config.value("area").toString()),
    m_friendly_name(config.value("friendly_name").toString()),
    m_integration(config.value("integration").toString()),
    m_favorite(false),
    m_connected(false),
    m_state(0),
    m_enumState(nullptr),
    m_enumAttr(nullptr),
    m_enumFeatures(nullptr),
    m_enumCommands(nullptr),
    m_specificInterface(nullptr)
{
    QString entityId = config.value("entity_id").toString();
    setObjectName(entityId);

    QStringList f = Config::getInstance()->getProfileFavorites();
    m_favorite = f.contains(entityId);
}

Entity::~Entity()
{
}
void Entity::command(int command, const QVariant& param)
{
    m_integrationObj->sendCommand(m_type, entity_id(), command, param);
}

QVariantMap Entity::getDataToSave()
{
    QVariantMap map;

    map["type"] = m_type;
    map["entity_id"] = entity_id();
    map["favorite"] = m_favorite;
    map["friendly_name"] = m_friendly_name;
    map["integration"] = m_integration;
    map["area"] = m_area;
    map["supported_features"] = supported_features();

    return map;
}

bool Entity::update(const QVariantMap &attributes)
{
    bool chg = false;
    for (QVariantMap::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
        if (updateAttrByName (iter.key(), iter.value()))
            chg = true;
    }
    return chg;
}
bool Entity::updateAttrByName(const QString& name, const QVariant& value)
{
    int attrIndex = getAttrIndex(name);
    return updateAttrByIndex (attrIndex, value);
}
QString Entity::getAttrName (int attrIndex)
{
    Q_ASSERT(m_enumAttr != nullptr);
    return m_enumAttr->valueToKey(attrIndex);
}
int Entity::getAttrIndex (const QString& attrName)
{
    Q_ASSERT(m_enumAttr != nullptr);
    return m_enumAttr->keyToValue(attrName.toUpper().toUtf8());
}
QString  Entity::getFeatureName (int featureIndex)
{
    Q_ASSERT(m_enumFeatures != nullptr);
    return QString(m_enumFeatures->valueToKey(featureIndex)).mid(2);
}
int Entity::getFeatureIndex (const QString& featureName)
{
    Q_ASSERT(m_enumFeatures != nullptr);
    QString name = featureName;
    return m_enumFeatures->keyToValue(name.prepend("F_").toUpper().toUtf8());
}

QString Entity::getCommandName (int commandIndex)
{
    Q_ASSERT(m_enumCommands != nullptr);
    return QString(m_enumCommands->valueToKey(commandIndex)).mid(2);
}
int Entity::getCommandIndex     (const QString& commandName)
{
    Q_ASSERT(m_enumCommands != nullptr);
    QString name = commandName;
    return m_enumCommands->keyToValue(name.prepend("C_").toUpper().toUtf8());
}

QStringList Entity::allAttributes ()
{
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumAttr->keyCount(); i++)
        list.append(m_enumAttr->key(i));
    return list;
}
QStringList Entity::allCommands ()
{
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumCommands->keyCount(); i++) {
        QString name(m_enumCommands->key(i));
        list.append(name.mid(2));
    }
    return list;
}
bool Entity::isSupported (int feature)
{
    return m_supported_features.contains(feature);
}

QStringList Entity::supported_features() {
    QStringList list;
    foreach (const int &feature, m_supported_features) {
        list.append (getFeatureName(feature));
    }
    return list;
}

QStringList Entity::allFeatures ()
{
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumFeatures->keyCount(); i++) {
        QString name(m_enumFeatures->key(i));
        list.append(name.mid(2));
    }
    return list;
}
QStringList Entity::allStates ()
{
    Q_ASSERT(m_enumState != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumState->keyCount(); i++)
        list.append(m_enumState->key(i));
    return list;
}

bool Entity::setState (int state) {
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
        emit onChanged();
        emit stateTextChanged();
        return true;
    }
    return false;
}
QString Entity::stateText()
{
    Q_ASSERT(m_enumState != nullptr);
    return m_enumState->valueToKey(m_state);
}
bool Entity::setStateText(const QString& stateText)
{
    Q_ASSERT(m_enumState != nullptr);
    return setState (m_enumState->keyToValue(stateText.toUpper().toUtf8()));
}

bool Entity::updateAttrByIndex(int idx, const QVariant& value)
{
    Q_UNUSED(idx)
    Q_UNUSED(value)
    Q_ASSERT(false);                        // Must be overriden in specific entity
    return false;
}
bool Entity::isOn() {
    return m_state != 0;                    // Otherwise : Override in specific entity
}
bool Entity::supportsOn() {
    return true;                            // Otherwise : Override in specific entity
}
void Entity::setConnected(bool value)
{
    if (m_connected != value) {
        m_connected = value;
        emit connectedChanged();
    }
}

void Entity::setFavorite(bool value)
{
    QTimer::singleShot(1000, this, [=]() {
        // Set Favorite
        Config::getInstance()->setFavorite(entity_id(), value);
        m_favorite = value;
        emit favoriteChanged();
    });
}

void Entity::initializeSupportedFeatures(const QVariantMap &config)
{
    QStringList features = config.value("supported_features").toStringList();
    for (int i = 0; i < features.length(); i++) {
        m_supported_features.insert(getFeatureIndex(features[i]));
    }
}
