// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "antilogviewer.h"

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
#include <QPushButton>
#include <QMenu>
#include <QDesktopWidget>

#include "table.h"
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

    auto logTable = new Table();
    logTable->setCornerButtonEnabled(false);
    logTable->setWordWrap(false);
    logTable->setSelectionMode(QAbstractItemView::SingleSelection);
    logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    logTable->setModel(_logModel);
    logTable->horizontalHeader()->setStretchLastSection(true);
    logTable->verticalHeader()->setDefaultSectionSize(logTable->verticalHeader()->minimumSectionSize());

    auto filters = new QWidget();
    filters->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    _filtersLayout = new QVBoxLayout(filters);
    _filtersLayout->setMargin(0);
    _filtersLayout->setSpacing(0);

    addChainElement(new UdpSocket());
    addChainElement(new MemoryStorage());
    addChainElement(new LevelFilter());
    addChainElement(new KeywordFilter());
    addChainElement(new SourceFilter());
    addChainElement(new TableView(_logModel));

    auto filtersScroll = new QScrollArea();
    filtersScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    filtersScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    filtersScroll->setWidget(filters);
    filtersScroll->setWidgetResizable(true);

    _profileButton = new QPushButton("&Profile");
    configureProfileButton();

    auto ctrAutoScroll = new QPushButton("AutoScroll");
    ctrAutoScroll->setCheckable(true);
    ctrAutoScroll->setChecked(_autoScroll);

    auto auxPanel = new QWidget();
    auto auxLayout = new QGridLayout(auxPanel);
    auxLayout->addWidget(filtersScroll, 0, 0, 1, 2);
    auxLayout->addWidget(ctrAutoScroll, 1, 0);
    auxLayout->addWidget(_profileButton, 1, 1);

    auto mainSplitter = new QSplitter();
    mainSplitter->addWidget(logTable);
    mainSplitter->addWidget(auxPanel);
    mainSplitter->setStretchFactor(0, 8);
    mainSplitter->setStretchFactor(0, 2);
    setCentralWidget(mainSplitter);

    connect(_logModel, &QAbstractTableModel::rowsInserted, [this, logTable]{
        if (_autoScroll)
            logTable->scrollToBottom();
    });

    connect(logTable->verticalScrollBar(), &QScrollBar::valueChanged, [logTable, ctrAutoScroll](int value){
        if (logTable->verticalScrollBar()->maximum() != value)
            ctrAutoScroll->setChecked(false);
    });

    connect(ctrAutoScroll, &QPushButton::toggled, [this, logTable](bool state) {
        _autoScroll = state;
        if (_autoScroll)
            logTable->scrollToBottom();
    });
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

void AntiLogViewer::configureProfileButton()
{
    auto profileMenu = new QMenu();

    auto addMenu = profileMenu->addMenu("Add");

    auto sourceMenu = addMenu->addMenu("Source");
    sourceMenu->addAction("UDP Source", [this] { addChainElement(new UdpSocket()); });
    sourceMenu->addAction("Text Input", [this] { addChainElement(new TextInput()); });

    auto filterMenu = addMenu->addMenu("Filter");
    filterMenu->addAction("Level", [this] { addChainElement(new LevelFilter()); });
    filterMenu->addAction("Source", [this] { addChainElement(new SourceFilter()); });
    filterMenu->addAction("Keyword", [this] { addChainElement(new KeywordFilter()); });

    auto systemMenu = addMenu->addMenu("System");
    systemMenu->addAction("Memory Storage", [this] { addChainElement(new MemoryStorage()); });

    auto outputMenu = addMenu->addMenu("Output");
    outputMenu->addAction("Table View", [this] {
        if (!_logModel->linked())
            addChainElement(new TableView(_logModel));
    });

    // TODO Add profiles from configuration
    profileMenu->addAction("Default");
    profileMenu->addAction("Save As");
    profileMenu->addSeparator();
    profileMenu->addMenu(addMenu);
    _profileButton->setMenu(profileMenu);
}
