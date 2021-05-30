// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

#include <QtTest>

#include "helpers.h"
#include "chain_elements/source_filter.h"
#include "test_sink.h"

class Utils : public QObject
{
    Q_OBJECT

private slots:
    void splitOnChunksTest_data();
    void splitOnChunksTest();

    void sourceFilterEmptyTest();
    void sourceFilterPassTest();
    void sourceFilterColorTest();

    void parseRangeIdealTest();
    void parseRangeMergeTest();
    void parseRangeEmptyTest();
    void parseRangePartialTest();
};

void Utils::splitOnChunksTest_data() {
    QTest::addColumn<QString>("source");
    QTest::addColumn<int>("num");
    QTest::addColumn<QString>("parts");
    QTest::addColumn<QString>("chunks");

    QTest::newRow("1 chunk w/o anything") << "Parent" << 1 << "" << "";
    QTest::newRow("1 chunk w/ parts") << "Parent" << 1 << "Parent" << "";
    QTest::newRow("1 chunk w/ chunks") << "Parent" << 1 << "" << "Parent";
    QTest::newRow("1 chunk w/ everyting") << "Parent" << 1 << "Parent" << "Parent";

    QTest::newRow("2 chunks w/o anything") << "Parent.Child" << 2 << "" << "";
    QTest::newRow("2 chunks w/ parts") << "Parent.Child" << 2 << "Parent,Parent.Child" << "";
    QTest::newRow("2 chunks w/ chunks") << "Parent.Child" << 2 << "" << "Parent,Child";
    QTest::newRow("2 chunks w/ everything") << "Parent.Child" << 2 << "Parent,Parent.Child" << "Parent,Child";
}

void Utils::splitOnChunksTest() {
    QFETCH(QString, source);
    QFETCH(int, num);
    QFETCH(QString, parts);
    QFETCH(QString, chunks);

    auto parts_splitted = parts.split(",", QString::SkipEmptyParts);
    auto use_parts = parts_splitted.length() > 0;
    auto chunks_splitted = chunks.split(",", QString::SkipEmptyParts);
    auto use_chunks = chunks_splitted.length() > 0;

    int index = 0;
    QString part;
    QString chunk;
    auto action = [&] { return splitOnChunks(source, index, use_parts ? &part : nullptr, use_chunks ? &chunk : nullptr); };

    for (auto i = 0; i < num; ++i)
    {
        QCOMPARE(action(), true);
        if (use_parts)
            QCOMPARE(parts_splitted[i], part);
        if (use_chunks)
            QCOMPARE(chunks_splitted[i], chunk);
    }

    QCOMPARE(action(), false);
}

std::shared_ptr<LogItem> testItem() {
    auto logItem = std::make_shared<LogItem>();
    logItem->Type = LogItemType::Log;
    logItem->Source = "Parent.Child 1";
    logItem->Level = LogLevel::Debug;
    logItem->Color = LogColor::Debug;
    return logItem;
}

void Utils::sourceFilterEmptyTest()
{
    // Arrange
    auto logItem = testItem();
    SourceFilter filter;
    TestSink sink;
    filter.setNext(&sink);

    // Act
    filter.accept(logItem);

    // Assert
    QCOMPARE(filter.allSources(), QSet<QString>({"Parent.Child 1"}));
    QCOMPARE(filter.sources(), QList<QString>());
    QCOMPARE(sink.last(), logItem);
}

void Utils::sourceFilterPassTest()
{
    // Arrange
    auto logItem = testItem();
    QJsonObject config;
    config["sources"] = QJsonArray({"Parent.Child 2"});
    SourceFilter filter;
    filter.load(config);
    TestSink sink;
    filter.setNext(&sink);

    // Act
    filter.accept(logItem);

    // Assert
    QCOMPARE(filter.allSources(), QSet<QString>({"Parent.Child 1", "Parent.Child 2"}));
    QCOMPARE(filter.sources(), QList<QString>({"Parent.Child 2"}));
    QCOMPARE(sink.last().use_count(), 0);
}

void Utils::sourceFilterColorTest()
{
    // Arrange
    auto logItem = testItem();
    QJsonObject config;
    config["mode"] = static_cast<int>(ChainElementMode::Mark1);
    config["sources"] = QJsonArray({"Parent.Child 2"});
    SourceFilter filter;
    filter.load(config);
    TestSink sink;
    filter.setNext(&sink);

    // Act
    filter.accept(logItem);

    // Assert
    QCOMPARE(filter.allSources(), QSet<QString>({"Parent.Child 1", "Parent.Child 2"}));
    QCOMPARE(filter.sources(), QList<QString>({"Parent.Child 2"}));
    QCOMPARE(sink.last(), logItem);
    QCOMPARE(sink.last()->Color, LogColor::Debug);

    // Arrange
    auto logItem2 = testItem();
    logItem2->Source = "Parent.Child 2";

    // Act
    filter.accept(logItem2);

    // Assert
    QCOMPARE(filter.allSources(), QSet<QString>({"Parent.Child 1", "Parent.Child 2"}));
    QCOMPARE(filter.sources(), QList<QString>({"Parent.Child 2"}));
    QCOMPARE(sink.last(), logItem2);
    QCOMPARE(sink.last()->Color, LogColor::Custom1);
}

void Utils::parseRangeIdealTest() {
    auto ranges = std::move(parseRange("1, 3,100-123"));
    QCOMPARE(ranges.size(), 3);
    QCOMPARE(ranges[0], std::make_tuple(1, 1));
    QCOMPARE(ranges[1], std::make_tuple(3, 3));
    QCOMPARE(ranges[2], std::make_tuple(100, 123));
}

void Utils::parseRangeMergeTest() {
    auto ranges = std::move(parseRange("5-7,1,2,99-300,50-100"));
    QCOMPARE(ranges.size(), 3);
    QCOMPARE(ranges[0], std::make_tuple(1, 2));
    QCOMPARE(ranges[1], std::make_tuple(5, 7));
    QCOMPARE(ranges[2], std::make_tuple(50, 300));
}

void Utils::parseRangeEmptyTest() {
    auto ranges = std::move(parseRange(""));
    QCOMPARE(ranges.size(), 0);
}

void Utils::parseRangePartialTest() {
    auto ranges = std::move(parseRange("-100,150-"));
    QCOMPARE(ranges.size(), 2);
    QCOMPARE(ranges[0], std::make_tuple(1, 100));
    QCOMPARE(ranges[1], std::make_tuple(150, std::numeric_limits<quint64>::max()));
}

QTEST_APPLESS_MAIN(Utils)

#include "tst_utils.moc"
