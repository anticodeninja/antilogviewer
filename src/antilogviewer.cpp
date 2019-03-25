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

#include "table_model.h"
#include "chain_elements/udp_socket.h"
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
    setWindowState(Qt::WindowMaximized);

    auto logTable = new QTableView();
    logTable->setCornerButtonEnabled(false);
    logTable->setWordWrap(false);
    logTable->setSelectionMode(QAbstractItemView::SingleSelection);
    logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    logTable->setModel(_logModel);
    logTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
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
    auxPanel->setMaximumWidth(300);
    auto auxLayout = new QGridLayout(auxPanel);
    auxLayout->addWidget(filtersScroll, 0, 0, 1, 2);
    auxLayout->addWidget(ctrAutoScroll, 1, 0);
    auxLayout->addWidget(_profileButton, 1, 1);

    auto mainSplitter = new QSplitter();
    mainSplitter->addWidget(logTable);
    mainSplitter->addWidget(auxPanel);
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
    if (!_chain.isEmpty())
        _chain.last()->setNext(element);
    _chain.append(element);

    auto frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);

    auto layout = new QGridLayout(frame);
    element->createUI(layout);

    auto configMenu = new QMenu();
    configMenu->addAction("Up", [this, element] { moveChainElement(element, true); });
    configMenu->addAction("Down", [this, element] { moveChainElement(element, false); });
    configMenu->addAction("Remove", [this, element] { removeChainElement(element); });

    auto ctrConfig = new QPushButton(element->name());
    ctrConfig->setMenu(configMenu);
    ctrConfig->setFlat(true);

    layout->addWidget(ctrConfig, 0, 0, 1, layout->columnCount());

    _filtersLayout->addWidget(frame);
}

void AntiLogViewer::moveChainElement(ChainElement *element, bool up)
{
    auto index = _chain.indexOf(element);
    if (up ? index == 0 : index == _chain.count() - 1)
        return;

    auto first = up ? index - 1 : index;
    auto second = up ? index: index + 1;

    if (first > 0)
        _chain[first - 1]->setNext(_chain[second]);
    _chain[first]->setNext(_chain[second]->getNext());
    _chain[second]->setNext(_chain[first]);

    _filtersLayout->insertWidget(first, _filtersLayout->itemAt(second)->widget());
    std::swap(_chain[first], _chain[second]);
}

void AntiLogViewer::removeChainElement(ChainElement *element)
{
    auto index = _chain.indexOf(element);
    delete _filtersLayout->itemAt(index)->widget();

    if (index > 0)
        _chain[index - 1]->setNext(element->getNext());

    _chain.removeAt(index);
    delete element;
}

void AntiLogViewer::configureProfileButton()
{
    auto profileMenu = new QMenu();

    auto addMenu = profileMenu->addMenu("Add");

    auto sourceMenu = addMenu->addMenu("Source");
    sourceMenu->addAction("UDP Source", [this] { addChainElement(new UdpSocket()); });

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
    profileMenu->addSeparator();
    profileMenu->addMenu(addMenu);
    profileMenu->addAction("Save As");
    profileMenu->addAction("Delete");
    _profileButton->setMenu(profileMenu);
}
