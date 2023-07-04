#!/usr/bin/env python
'''
  Copyright (C) 2016 Bastille Networks

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
'''


import codecs, time, logging
from lib import common

# Parse command line arguments and initialize the radio
common.init_args('./nrf24-sniffer.py')
common.parser.add_argument('-a', '--address', type=str, help='Address to sniff, following as it changes channels', required=True)
common.parser.add_argument('-t', '--timeout', type=float, help='Channel timeout, in milliseconds', default=100)
common.parser.add_argument('-k', '--ack_timeout', type=int, help='ACK timeout in microseconds, accepts [250,4000], step 250', default=250)
common.parser.add_argument('-r', '--retries', type=int, help='Auto retry limit, accepts [0,15]', default=1, choices=range(0, 16), metavar='RETRIES')
common.parser.add_argument('-p', '--ping_payload', type=str, help='Ping payload, ex 0F:0F:0F:0F', default='0F:0F:0F:0F', metavar='PING_PAYLOAD')
common.parse_and_init()

# Parse the address
address = codecs.decode(common.args.address.replace(':', ''), 'hex')
address_string = ':'.join('{:02X}'.format(b) for b in address[::-1])
if len(address) < 2:
  raise Exception('Invalid address: {0}'.format(common.args.address))

# Put the radio in sniffer mode (ESB w/o auto ACKs)
common.radio.enter_sniffer_mode(address)

# Convert channel timeout from milliseconds to seconds
timeout = float(common.args.timeout) / float(1000)

common.radio.set_channel(common.channels[0])

# Sweep through the channels and decode ESB packets in pseudo-promiscuous mode
last_tune = time.time()
channel_index = 0
while True:

  # Increment the channel
  if len(common.channels) > 1 and time.time() - last_tune > timeout:
    channel_index = (channel_index + 1) % (len(common.channels))
    common.radio.set_channel(common.channels[channel_index])
    last_tune = time.time()

  # Receive payloads
  value = common.radio.receive_payload()
  if len(value) >= 5:

    # Log the packet
    logging.info('{0: >2}  {1: >2}  {2}'.format(
              common.channels[channel_index],
              len(value),
              # ''.join(format(b, '0>8b') for b in value)))
              ':'.join('{:02X}'.format(b) for b in value)))