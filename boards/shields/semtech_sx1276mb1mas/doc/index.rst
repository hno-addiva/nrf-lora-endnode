.. _semtech_sx1276mb1mas:

Semtech SX1276MB1MAS LoRa Shield
################################

Overview
********

The Semtech SX1272MB2DAS LoRa shield is an Arduino
compatible shield based on the SX1272 LoRa transceiver
from Semtech.

More information about the shield can be found
at the `mbed SX1272MB2xAS website`_.

Pins Assignment of the Semtech SX1272MB2DAS LoRa Shield
=======================================================

+-----------------------+-----------------+
| Shield Connector Pin  | Function        |
+=======================+=================+
| A0                    | SX1276 RESET    |
+-----------------------+-----------------+
| D10                   | SX1276 SPI NSS  |
+-----------------------+-----------------+
| D11                   | SX1276 SPI MOSI |
+-----------------------+-----------------+
| D12                   | SX1276 SPI MISO |
+-----------------------+-----------------+
| D13                   | SX1276 SPI SCK  |
+-----------------------+-----------------+
| D2                    | SX1276 DIO0     |
+-----------------------+-----------------+
| D3                    | SX1276 DIO1     |
+-----------------------+-----------------+
| D4                    | SX1276 DIO2     |
+-----------------------+-----------------+
| D5                    | SX1276 DIO3     |
+-----------------------+-----------------+
| D8                    | SX1276 DIO4     |
+-----------------------+-----------------+
| A3                    | SX1276 DIO4(alt)|
+-----------------------+-----------------+
| D9                    | SX1276 DIO5     |
+-----------------------+-----------------+
| A4                    | SX1276 RXTX     |
+-----------------------+-----------------+

Requirements
************

This shield can only be used with a board which provides a configuration
for Arduino connectors and defines node aliases for SPI and GPIO interfaces
(see :ref:`shields` for more details).

Programming
***********

Set ``-DSHIELD=semtech_sx1276mb1mas`` when you invoke ``west build``. For
example:

.. zephyr-app-commands::
   :zephyr-app: samples/lorawan/class_a
   :board: nucleo_f429zi
   :shield: semtech_sx1276mb1mas
   :goals: build

Notes
*****

The RXTX signal is not connected on the boards we have, instead driven internally by SX1276.

References
**********

.. target-notes::

.. _mbed SX1276MB1xAS website:
   https://os.mbed.com/components/SX1276MB1xAS/

.. _Semtech SX1276 Product Page (includes SX1276MB1MAS reference design info):
   https://www.semtech.com/products/wireless-rf/lora-connect/sx1276
