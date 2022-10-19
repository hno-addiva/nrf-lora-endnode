.. _semtech_sx1261mb2bas:

Semtech SX1261MB2BAS LoRa Shield
################################

Overview
********

The Semtech SX1261MB2BAS LoRa shield is an Arduino
compatible shield based on the SX1261 LoRa transceiver
from Semtech.

More information about the shield can be found
at the `mbed SX126xMB2xAS website`_.

Pins Assignment of the Semtech SX1272MB2DAS LoRa Shield
=======================================================

+-----------------------+-----------------+
| Shield Connector Pin  | Function        |
+=======================+=================+
| A0                    | SX1261 RESET    |
+-----------------------+-----------------+
| D7                    | SX1261 SPI NSS  |
+-----------------------+-----------------+
| D11                   | SX1261 SPI MOSI |
+-----------------------+-----------------+
| D12                   | SX1261 SPI MISO |
+-----------------------+-----------------+
| D13                   | SX1261 SPI SCK  |
+-----------------------+-----------------+
| D3                    | SX1261 BUSY     |
+-----------------------+-----------------+
| D5                    | SX1261 DIO1     |
+-----------------------+-----------------+
| D8                    | ANT_SW(*)       |
+-----------------------+-----------------+
| D9                    | D9 header*      |
+-----------------------+-----------------+
| D10                   | D10 header*     |
+-----------------------+-----------------+
| A4                    | A5 header*      |
+-----------------------+-----------------+
| A5                    | A6 header*      |
+-----------------------+-----------------+
| D18 / SDA             | SDA header      |
+-----------------------+-----------------+
| D19 / SCL             | SCL header      |
+-----------------------+-----------------+
| A0                    | FREQ_SEL        |
+-----------------------+-----------------+
| A1                    | DEVICE_SEL      |
+-----------------------+-----------------+
| A2                    | XTAL_SEL        |
+-----------------------+-----------------+

Requirements
************

This shield can only be used with a board which provides a configuration
for Arduino connectors and defines node aliases for SPI and GPIO interfaces
(see :ref:`shields` for more details).

Programming
***********

Set ``-DSHIELD=semtech_sx1261mb2bas`` when you invoke ``west build``. For
example:

.. zephyr-app-commands::
   :zephyr-app: samples/lorawan/class_a
   :board: nucleo_f429zi
   :shield: semtech_sx1261mb2bas
   :goals: build

Notes
*****

The ANT_SW signal is optional, can be driven internally by SX1261. Is wired
on the board but can be tied high to let the SX1261 control the antenna direction.

FREQ_SEL, DEVICE_SEL and XTAL_SEL indicates the board model, for designs
supporing whole range of SX126xMB2xAS boards. There is no active functionality
on these pins.

References
**********

.. target-notes::

.. _mbed SX126xMB2xAS website:
   https://os.mbed.com/components/SX126xMB2xAS/

.. _Semtech SX1261 Product Page (includes SX1261MB2BAS reference design info):
   https://www.semtech.com/products/wireless-rf/lora-connect/sx1261
