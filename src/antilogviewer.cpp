// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "antilogviewer.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QWidget>
#include <QSplitter>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QGroupBox>
#include <QTableView>
#include <QScrollArea>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTextDocumentFragment>
#include <QPushButton>
#include <QMenu>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>

#include "constants.h"
#include "table_model.h"
#include "chain_elements/udp_socket.h"
#include "chain_elements/text_input.h"
#include "chain_elements/level_filter.h"
#include "chain_elements/keyword_filter.h"
#include "chain_elements/memory_storage.h"
#include "chain_elements/source_filter.h"
#include "chain_elements/table_view.h"

AntiLogViewer::AntiLogViewer(QWidget *parent)
    : QMainWindow(parent)
    , _logModel(new TableModel())
    , _autoScroll(true)
{
    setWindowIcon(QIcon(":/logo.ico"));
    resize(QDesktopWidget().availableGeometry(this).size());
    setWindowState(Qt::WindowMaximized);

    auto logTable = new QTableView();
    logTable->setCornerButtonEnabled(false);
    logTable->setWordWrap(false);
    logTable->setSelectionMode(QAbstractItemView::SingleSelection);
    logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    logTable->setModel(_logModel);
    logTable->horizontalHeader()->setStretchLastSection(true);
    logTable->verticalHeader()->setDefaultSectionSize(logTable->verticalHeader()->minimumSectionSize());

    auto details = new QPlainTextEdit();
    details->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    details->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    auto filters = new QWidget();
    filters->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    _filtersLayout = new QVBoxLayout(filters);
    _filtersLayout->setMargin(0);
    _filtersLayout->setSpacing(0);

    auto filtersScroll = new QScrollArea();
    filtersScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    filtersScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    filtersScroll->setMinimumWidth(250);
    filtersScroll->setWidget(filters);
    filtersScroll->setWidgetResizable(true);

    _profileButton = new QPushButton("&Profile");

    auto ctrAutoScroll = new QPushButton("AutoScroll");
    ctrAutoScroll->setCheckable(true);
    ctrAutoScroll->setChecked(_autoScroll);

    auto auxPanel = new QWidget();
    auto auxLayout = new QGridLayout(auxPanel);
    auxLayout->addWidget(filtersScroll, 0, 0, 1, 2);
    auxLayout->addWidget(ctrAutoScroll, 1, 0);
    auxLayout->addWidget(_profileButton, 1, 1);

    auto leftSplitter = new QSplitter(Qt::Vertical);
    leftSplitter->addWidget(logTable);
    leftSplitter->addWidget(details);
    leftSplitter->setStretchFactor(0, 6);
    leftSplitter->setStretchFactor(1, 1);

    auto mainSplitter = new QSplitter();
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(auxPanel);
    mainSplitter->setStretchFactor(0, 8);
    mainSplitter->setStretchFactor(1, 1);
    setCentralWidget(mainSplitter);

    connect(_logModel, &QAbstractTableModel::rowsInserted, [this, logTable] {
        if (_autoScroll)
            logTable->scrollToBottom();
    });

    connect(logTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            [details](const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous);
        auto palette = details->palette();
        auto messageIndex = current.siblingAtColumn(2);
        palette.setColor(QPalette::Window, messageIndex.data(Qt::BackgroundColorRole).value<QColor>());
        palette.setColor(QPalette::Text, messageIndex.data(Qt::TextColorRole).value<QColor>());
        // It can look strange, but is tradeoff to get good table perfomance
        details->setPlainText(messageIndex.data(Qt::DisplayRole).toString().replace(NEWLINE_CHAR, '\n'));
        details->setPalette(palette);
    });
    connect(logTable->verticalScrollBar(), &QScrollBar::valueChanged, [logTable, ctrAutoScroll](int value) {
        if (logTable->verticalScrollBar()->maximum() != value)
            ctrAutoScroll->setChecked(false);
    });

    connect(ctrAutoScroll, &QPushButton::toggled, [this, logTable](bool state) {
        _autoScroll = state;
        if (_autoScroll)
            logTable->scrollToBottom();
    });

    loadConfiguration();
}

AntiLogViewer::~AntiLogViewer()
{

}

void AntiLogViewer::addChainElement(ChainElement *element)
{
    auto index = 0;
    for (; index < _chain.length(); ++index) {
        if (_chain[index]->type() > element->type()) {
            break;
        }
    }

    insertChainElement(element, index);
}

void AntiLogViewer::insertChainElement(ChainElement *element, int position)
{
    _chain.insert(position, element);

    if (position < _chain.length() - 1) {
        element->setNext(_chain[position + 1]);
    }
    if (position > 0) {
        _chain[position - 1]->setNext(element);
    }

    if (element->widget() == nullptr) {
        auto frame = new QFrame();
        frame->setFrameShape(QFrame::StyledPanel);

        auto layout = new QGridLayout(frame);
        element->createUI(layout);

        auto configMenu = new QMenu();
        configMenu->addAction("Up", [this, element] { moveChainElement(element, -1); });
        configMenu->addAction("Down", [this, element] { moveChainElement(element, 1); });
        configMenu->addAction("Remove", [this, element] { removeChainElement(element, true); });

        auto ctrConfig = new QPushButton(element->name());
        ctrConfig->setMenu(configMenu);
        ctrConfig->setFlat(true);

        layout->addWidget(ctrConfig, 0, 0, 1, layout->columnCount());
        element->setWidget(frame);
    }

    _filtersLayout->insertWidget(position, element->widget());
}

void AntiLogViewer::moveChainElement(ChainElement *element, int delta)
{
    auto index = _chain.indexOf(element);
    auto newIndex = qBound(0, index + delta, _chain.count() - 1);
    if (index == newIndex)
        return;

    removeChainElement(element, false);
    insertChainElement(element, newIndex);
}

void AntiLogViewer::removeChainElement(ChainElement *element, bool free)
{
    auto index = _chain.indexOf(element);

    if (index > 0)
        _chain[index - 1]->setNext(element->next());

    element->setNext(nullptr);
    _chain.removeAt(index);

    if (free) {
        delete element;
    }
}

void AntiLogViewer::loadConfiguration()
{
    QFile loadFile("antilogviewer.config");

    QJsonObject configuration;
    if (loadFile.open(QIODevice::ReadOnly)) {
        configuration = QJsonDocument::fromJson(loadFile.readAll()).object();
    } else {
        qWarning() << "Couldn't open save file, generate default";
        configuration = QJsonObject();
    }

    // Profiles
    if (configuration[PROFILES_SECTION].isObject()) {
        auto profilesSection = configuration[PROFILES_SECTION].toObject();
        for (auto i = profilesSection.constBegin(); i != profilesSection.constEnd(); ++i)
            _profiles[i.key()] = i.value().toArray();
    }

    if (!loadProfile(DEFAULT_PROFILE))
        generateDefaultProfile();
    configureProfileButton();

    // Colors
    // TODO Unhardcode
    QPalette palette(QColor(7, 54, 66));
    palette.setColor(QPalette::Base, QColor(7, 54, 66));
    palette.setColor(QPalette::Highlight, QColor(88, 110, 117));
    QApplication::setPalette(palette);
}

void AntiLogViewer::saveConfiguration(bool silent)
{
    QFile saveFile("antilogviewer.config");
    if (!saveFile.open(QIODevice::WriteOnly)) {
        if (silent) {
            qWarning() << "Couldn't save configuration";
        } else {
            QMessageBox::critical(this, "Something bad happens...", "Couldn't save configuration");
        }
        return;
    }

    QJsonObject configuration;

    QJsonObject profilesSection;
    for (auto i = _profiles.constBegin(); i != _profiles.constEnd(); ++i)
        profilesSection[i.key()] = i.value();
    configuration[PROFILES_SECTION] = profilesSection;

    saveFile.write(QJsonDocument(configuration).toJson());
}

bool AntiLogViewer::loadProfile(QString name)
{
    if (!_profiles.contains(name))
    {
        qWarning() << "Could not load profile" << name;
        return false;
    }

    while (!_chain.empty())
        removeChainElement(_chain.last(), true);

    auto chain = _profiles[name];
    for (auto i = 0; i < chain.size(); ++i) {
        auto elementSection = chain[i].toObject();
        if (!elementSection["name"].isString())
            continue;

        auto name = elementSection["name"].toString();
        ChainElement* element = nullptr;
        if (name == "Udp Socket") {
            element = new UdpSocket();
        } else if (name == "Text Input") {
            element = new TextInput();
        } else if (name == "Level Filter") {
            element = new LevelFilter();
        } else if (name == "Source Filter") {
            element = new SourceFilter();
        } else if (name == "Keyword Filter") {
            element = new KeywordFilter();
        } else if (name == "Memory Storage") {
            element = new MemoryStorage();
        } else if (name == "Table View") {
            if (!_logModel->linked())
                element = new TableView(_logModel);
        }

        if (element == nullptr)
            continue;

        if (elementSection["data"].isObject())
            element->load(elementSection["data"].toObject());
        insertChainElement(element, _chain.size());
    }

    return true;
}

void AntiLogViewer::saveProfile(QString name)
{
    bool ok = true;
    if (name.isEmpty()) {
        QStringList profilesNames;
        for (auto i = _profiles.constBegin(); i != _profiles.constEnd(); ++i)
            profilesNames.append(i.key());

        name = QInputDialog::getItem(this, "Save Profile", "Profile Name",
                                          profilesNames, 0, true, &ok);
    }

    if (!ok || name.isEmpty())
        return;

    QJsonArray chain;
    foreach (auto element, _chain) {
        QJsonObject elementSection;
        elementSection["name"] = element->name();

        QJsonObject elementData;
        element->save(elementData);
        if (!elementData.empty())
            elementSection["data"] = elementData;

        chain.append(elementSection);
    }

    _profiles[name] = chain;
    configureProfileButton();
    saveConfiguration(false);
}

void AntiLogViewer::generateDefaultProfile()
{
    addChainElement(new UdpSocket());
    addChainElement(new MemoryStorage());
    addChainElement(new LevelFilter());
    addChainElement(new KeywordFilter());
    addChainElement(new SourceFilter());
    addChainElement(new TableView(_logModel));
    saveProfile(DEFAULT_PROFILE);
}

void AntiLogViewer::deleteProfile()
{
    if (_profiles.count() < 2)
        return;

    QStringList profilesNames;
    for (auto i = _profiles.constBegin(); i != _profiles.constEnd(); ++i)
        if (i.key() != DEFAULT_PROFILE)
            profilesNames.append(i.key());

    bool ok = true;
    auto name = QInputDialog::getItem(this, "Delete Profile", "Profile Name",
                                      profilesNames, 0, false, &ok);

    _profiles.remove(name);
    configureProfileButton();
    saveConfiguration(false);
}

void AntiLogViewer::configureProfileButton()
{
    auto profileMenu = new QMenu();

    for (auto i = _profiles.constBegin(); i != _profiles.constEnd(); ++i) {
        auto profileName = i.key();
        profileMenu->addAction(profileName, [this, profileName] { loadProfile(profileName); });
    }
    profileMenu->addSeparator();

    profileMenu->addAction("Save Profile", [this] { saveProfile(QString()); });
    profileMenu->addAction("Delete Profile", [this] { deleteProfile(); });
    profileMenu->addSeparator();

    auto addMenu = profileMenu->addMenu("Add");

    addMenu->addSection("Sources");
    addMenu->addAction("UDP Socket", [this] { addChainElement(new UdpSocket()); });
    addMenu->addAction("Text Input", [this] { addChainElement(new TextInput()); });

    addMenu->addSection("Filters");
    addMenu->addAction("Level", [this] { addChainElement(new LevelFilter()); });
    addMenu->addAction("Source", [this] { addChainElement(new SourceFilter()); });
    addMenu->addAction("Keyword", [this] { addChainElement(new KeywordFilter()); });

    addMenu->addSection("Systems");
    addMenu->addAction("Memory Storage", [this] { addChainElement(new MemoryStorage()); });

    addMenu->addSection("Outputs");
    addMenu->addAction("Table View", [this] {
        if (!_logModel->linked())
            addChainElement(new TableView(_logModel));
    });

    profileMenu->addMenu(addMenu);

    _profileButton->setMenu(profileMenu);
}
