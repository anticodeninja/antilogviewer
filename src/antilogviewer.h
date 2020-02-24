// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef ANTILOGVIEWER_H
#define ANTILOGVIEWER_H

#include <QJsonObject>
#include <QMainWindow>
#include <QList>
#include <QMap>

class ChainElement;
class TableModel;
class TableView;

class QLineEdit;
class QPushButton;
class QComboBox;
class QBoxLayout;
class QGridLayout;
class QFrame;

class AntiLogViewer : public QMainWindow
{
    Q_OBJECT

public:
    AntiLogViewer(QWidget *parent = nullptr);
    ~AntiLogViewer();

private:
    void addChainElement(ChainElement *element);
    void insertChainElement(ChainElement *element, int position);
    void moveChainElement(ChainElement *element, int delta);
    void removeChainElement(ChainElement *element, bool free);
    void updateChainSlots();

    void loadConfiguration();
    void saveConfiguration(bool silent);

    bool loadProfile(QString name);
    void saveProfile(QString name);
    void generateDefaultProfile();
    void deleteProfile();

    void configureProfileButton();

    TableModel* _logModel;
    QList<ChainElement*> _chain;
    QBoxLayout* _filtersLayout;
    QPushButton* _profileButton;
    QMap<QString, QJsonArray> _profiles;

    bool _autoScroll;
};

#endif // ANTILOGVIEWER_H
