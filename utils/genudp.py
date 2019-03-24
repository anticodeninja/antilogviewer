#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
# Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

import time
import random
import socket

MESSAGES = [
    'Something bad happens',
    'Something strange happens',
    'Cannot connect to nowhere',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.',
]
LOGGERS = [
    'Parent',
    'Parent.Base 1',
    'Parent.Base 2',
]
LEVELS = [
    'TRACE',
    'DEBUG',
    'INFO',
    'WARN',
    'ERROR',
    'FATAL',
]
TEMPLATE = '<log4j:event logger="{logger}" level="{level}" timestamp="{timestamp}" thread="{thread}"><log4j:message>{message}</log4j:message><log4j:properties><log4j:data name="log4japp" value="test.exe" /><log4j:data name="log4jmachinename" value="test-machine" /></log4j:properties></log4j:event>'

SEND_INTERVAL = 0.2
BUNCH_SIZE = 1
counter = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while True:
    for i in range(BUNCH_SIZE):
        message = TEMPLATE.format(
            logger=random.choice(LOGGERS),
            level=random.choice(LEVELS),
            timestamp=time.time(),
            thread=random.randint(0, 10),
            message=random.choice(MESSAGES))
        sock.sendto(message.encode('utf-8'), ('127.0.0.1', 7071))
        counter += 1
    print('\r{}'.format(counter), end='', flush=True)
    time.sleep(SEND_INTERVAL)
