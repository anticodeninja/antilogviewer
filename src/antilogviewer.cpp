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
#include "chain_elements/terminator.h"

AntiLogViewer::AntiLogViewer(QWidget *parent)
    : QMainWindow(parent)
    , _autoScroll(true)
    , _logModel(new TableModel())
    , _terminator(new Terminator(_logModel))
{
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

    _chain.append(new UdpSocket());
    _chain.append(new MemoryStorage());
    _chain.append(new LevelFilter());
    _chain.append(new KeywordFilter());
    updateChain(0, 0, _chain.count());

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

QWidget* AntiLogViewer::createToolSet(ChainElement *element)
{
    auto frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);

    auto layout = new QGridLayout(frame);
    element->createUI(layout);

    auto ctrName = new QLabel(element->name());

    auto configMenu = new QMenu();
    configMenu->addAction("Up");
    configMenu->addAction("Down");
    configMenu->addAction("Remove");

    auto ctrConfig = new QPushButton("config");
    ctrConfig->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    ctrConfig->setMenu(configMenu);
    ctrConfig->setFlat(true);

    layout->addWidget(ctrName, 0, 0, Qt::AlignLeft);
    layout->addWidget(ctrConfig, 0, layout->columnCount() - 1, Qt::AlignRight);

    return frame;
}

void AntiLogViewer::updateChain(int start, int remove, int add)
{
    for (auto i = remove; i >= 0; --i)
        _filtersLayout->removeItem(_filtersLayout->itemAt(start + i));

    for (auto i = 0; i < add; ++i)
        _filtersLayout->insertWidget(start + i, createToolSet(_chain[start + i]));

    for (auto i = 0; i < _chain.count() - 1; ++i)
        _chain[i]->setNext(_chain[i + 1]);
    _chain[_chain.count() - 1]->setNext(_terminator);
}

void AntiLogViewer::configureProfileButton()
{
    auto sourceMenu = new QMenu("Source");
    sourceMenu->addAction("UDP Source", [this](){
        _chain.append(new UdpSocket());
        updateChain(_chain.count() - 1, 0, 1);
    });

    auto filterMenu = new QMenu("Filter");
    filterMenu->addAction("Level", [this](){
        _chain.append(new LevelFilter());
        updateChain(_chain.count() - 1, 0, 1);
    });
    filterMenu->addAction("Logger");
    filterMenu->addAction("Keyword");

    auto addMenu = new QMenu("Add");
    addMenu->addMenu(sourceMenu);
    addMenu->addMenu(filterMenu);
    addMenu->addAction("Memory Storage", [this](){
        _chain.append(new MemoryStorage());
        updateChain(_chain.count() - 1, 0, 1);
    });

    auto profileMenu = new QMenu();
    // TODO Add profiles from configuration
    profileMenu->addAction("Default");
    profileMenu->addSeparator();
    profileMenu->addMenu(addMenu);
    profileMenu->addAction("Save As");
    profileMenu->addAction("Delete");
    _profileButton->setMenu(profileMenu);
}
