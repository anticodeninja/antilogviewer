// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include <QtTest>

#include "helpers.h"

class Utils : public QObject
{
    Q_OBJECT

private slots:
    void splitOnChunksTest_data();
    void splitOnChunksTest();
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

QTEST_APPLESS_MAIN(Utils)

#include "tst_utils.moc"
