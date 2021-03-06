#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
# Copyright 2019-2021 Artem Yamshanov, me [at] anticode.ninja

import argparse
import time
import random
import socket

MESSAGES = [
    'Something bad happens',
    'Something strange happens',
    'Cannot connect to nowhere',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation\n ullamco laboris nisi ut aliquip ex ea commodo consequat.',
    'Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolaborisnisiutaliquipexeacommodoconsequat.Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolaborisnisiutaliquipexeacommodoconsequat.Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolaborisnisiutaliquipexeacommodoconsequat.',
]
LOGGERS = [
    'Parent',
    'Parent.Base 1',
    'Parent.Base 1.Extra Base 1',
    'Parent.Base 1.Extraloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong Base 1',
    'Parent.Base 2',
    'Parent.Base 3',
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

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--target', default='127.0.0.1',
                        help='address of antilogviewer')
    parser.add_argument('-s', '--bunch-size', type=int, default=1,
                        help='amount of packets sended in one bunch')
    parser.add_argument('-i', '--send-interval', type=float, default=0.2,
                        help='interval between bunch sending in seconds')
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    counter = 0

    while True:
        for i in range(args.bunch_size):
            message = TEMPLATE.format(
                logger=random.choice(LOGGERS),
                level=random.choice(LEVELS),
                timestamp=int(time.time() * 1000.0),
                thread=random.randint(0, 10),
                message='{} {}'.format(counter, random.choice(MESSAGES)))
            sock.sendto(message.encode('utf-8'), (args.target, 7071))
            counter += 1
        print('\r{}'.format(counter), end='', flush=True)
        time.sleep(args.send_interval)


if __name__ == "__main__":
    main()
