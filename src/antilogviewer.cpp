// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include "antilogviewer.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QSplitter>
#include <QHeaderView>
#include <QScrollBar>
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
#include "table_view_hacks.h"
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

    _logTable = new QTableView();
    _logTable->setCornerButtonEnabled(false);
    _logTable->setContextMenuPolicy(Qt::CustomContextMenu);
    _logTable->setWordWrap(false);
    _logTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _logTable->setItemDelegate(new TableViewColumnStyledItem(_logModel));
    _logTable->setModel(_logModel);
    _logTable->horizontalHeader()->setStretchLastSection(true);
    _logTable->verticalHeader()->setDefaultSectionSize(_logTable->verticalHeader()->minimumSectionSize());

    auto details = new QPlainTextEdit();
    details->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    details->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    details->setAutoFillBackground(true);
    details->setContextMenuPolicy(Qt::CustomContextMenu);

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
    leftSplitter->addWidget(_logTable);
    leftSplitter->addWidget(details);
    leftSplitter->setStretchFactor(0, 6);
    leftSplitter->setStretchFactor(1, 1);

    auto mainSplitter = new QSplitter();
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(auxPanel);
    mainSplitter->setStretchFactor(0, 8);
    mainSplitter->setStretchFactor(1, 1);
    setCentralWidget(mainSplitter);

    connect(_logModel, &QAbstractTableModel::rowsInserted, [this] {
        if (_autoScroll)
            _logTable->scrollToBottom();
    });

    connect(_logTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            [this, details](const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous)

        auto rowId = current.row();
        if (rowId < 0 || rowId >= _logModel->rowCount())
            return;
        auto item = _logModel->get(rowId);

        details->setPlainText(QString("%0#%1 %2\n%3\n%4")
            .arg(QDateTime::fromMSecsSinceEpoch(item->Timestamp).toString("yyyy-MM-dd HH:mm:ss.zzz"))
            .arg(item->Id)
            .arg(ColorNames[static_cast<int>(item->Level)])
            .arg(item->Source)
            // It can look strange, but is tradeoff to get good table performance
            .arg(QString(item->Message).replace(NEWLINE_CHAR, '\n')));
    });

    connect(_logTable->verticalScrollBar(), &QScrollBar::valueChanged, [this, ctrAutoScroll](int value) {
        if (_logTable->verticalScrollBar()->maximum() != value)
            ctrAutoScroll->setChecked(false);
    });

    connect(_logTable, &QTableView::customContextMenuRequested,
            [this](const QPoint &pos) {
        auto index = _logTable->indexAt(pos);
        if (!index.isValid())
            return;

        auto item = _logModel->get(index.row());

        auto context= new QMenu(this);
        for (auto i = 0; i < _chain.size(); ++i) {
            _chain[i]->createMenuOnEntry(context, item);
        }
        context->exec(_logTable->viewport()->mapToGlobal(pos));
    });

    connect(details, &QPlainTextEdit::customContextMenuRequested,
            [this, details](const QPoint &pos) {
        auto context= new QMenu(this);
        auto text = details->textCursor().selectedText();
        for (auto i = 0; i < _chain.size(); ++i) {
            _chain[i]->createMenuOnSelection(context, text);
        }
        context->addAction("Copy", [details](){
            details->copy();
        });
        context->addAction("Select All", [details](){
            details->selectAll();
        });
        context->exec(details->mapToGlobal(pos));
    });

    connect(ctrAutoScroll, &QPushButton::toggled, [this](bool state) {
        _autoScroll = state;
        if (_autoScroll)
            _logTable->scrollToBottom();
    });

    loadConfiguration();
}

AntiLogViewer::~AntiLogViewer() = default;

void AntiLogViewer::addChainElement(ChainElement *element)
{
    auto index = 0;
    for (; index < _chain.length(); ++index) {
        if (_chain[index]->type() > element->type()) {
            break;
        }
    }

    insertChainElement(element, index);
    updateChainSlots();
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

        auto ctrConfig = new QPushButton(element->fullname());
        ctrConfig->setMenu(configMenu);
        ctrConfig->setFlat(true);

        layout->addWidget(ctrConfig, 0, 0, 1, layout->columnCount());
        element->setWidget(frame);
    }

    _filtersLayout->insertWidget(position, element->widget());
    updateChainSlots();
}

void AntiLogViewer::moveChainElement(ChainElement *element, int delta)
{
    auto index = _chain.indexOf(element);
    auto newIndex = qBound(0, index + delta, _chain.count() - 1);
    if (index == newIndex)
        return;

    removeChainElement(element, false);
    insertChainElement(element, newIndex);
    updateChainSlots();
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
        updateChainSlots();
    }
}

void AntiLogViewer::updateChainSlots()
{
    QMap<QString, int> counters;

    foreach (auto& element, _chain) {
        auto slotId = counters.value(element->name(), 1);
        counters[element->name()] = slotId + 1;

        element->setSlotId(slotId);
        auto layout = dynamic_cast<QGridLayout*>(element->widget()->layout());
        auto widget = dynamic_cast<QPushButton*>(layout->itemAtPosition(0, 0)->widget());
        widget->setText(element->fullname());
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

bool AntiLogViewer::loadProfile(const QString& profileName)
{
    if (!_profiles.contains(profileName))
    {
        qWarning() << "Could not load profile" << profileName;
        return false;
    }

    while (!_chain.empty())
        removeChainElement(_chain.last(), true);

    auto chain = _profiles[profileName];
    for (auto i = 0; i < chain.size(); ++i) {
        auto elementSection = chain[i].toObject();
        if (!elementSection["name"].isString())
            continue;

        auto elementName = elementSection["name"].toString();
        ChainElement* element = nullptr;
        if (elementName == "Udp Socket") {
            element = new UdpSocket();
        } else if (elementName == "Text Input") {
            element = new TextInput();
        } else if (elementName == "Level Filter") {
            element = new LevelFilter();
        } else if (elementName == "Source Filter") {
            element = new SourceFilter();
        } else if (elementName == "Keyword Filter") {
            element = new KeywordFilter();
        } else if (elementName == "Memory Storage") {
            element = new MemoryStorage();
        } else if (elementName == "Table View") {
            if (!_logModel->linked())
            {
                element = new TableView(_logTable, _logModel);
                _logModel->clear();
            }
        }

        if (element == nullptr)
            continue;

        if (elementSection["data"].isObject())
            element->load(elementSection["data"].toObject());
        insertChainElement(element, _chain.size());
    }

    return true;
}

void AntiLogViewer::saveProfile(QString profileName)
{
    bool ok = true;
    if (profileName.isEmpty()) {
        QStringList profilesNames;
        for (auto i = _profiles.constBegin(); i != _profiles.constEnd(); ++i)
            profilesNames.append(i.key());

        profileName = QInputDialog::getItem(this, "Save Profile", "Profile Name",
                                            profilesNames, 0, true, &ok);
    }

    if (!ok || profileName.isEmpty())
        return;

    QJsonArray chain;
    foreach (auto& element, _chain) {
        QJsonObject elementSection;
        elementSection["name"] = element->name();

        QJsonObject elementData;
        element->save(elementData);
        if (!elementData.empty())
            elementSection["data"] = elementData;

        chain.append(elementSection);
    }

    _profiles[profileName] = chain;
    configureProfileButton();
    saveConfiguration(false);
}

void AntiLogViewer::generateDefaultProfile()
{
    TableView *tableView;

    insertChainElement(new UdpSocket(), 0);
    insertChainElement(new MemoryStorage(), 1);
    insertChainElement(new LevelFilter(), 2);
    insertChainElement(new KeywordFilter(), 3);
    insertChainElement(new SourceFilter(), 4);
    insertChainElement(tableView = new TableView(_logTable, _logModel), 5);

    tableView->setDefaultSettings();
    updateChainSlots();

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
        const auto& profileName = i.key();
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
            addChainElement(new TableView(_logTable, _logModel));
    });

    profileMenu->addMenu(addMenu);

    _profileButton->setMenu(profileMenu);
}
