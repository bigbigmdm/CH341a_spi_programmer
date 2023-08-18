# CH341a_spi_programmer
QT5 Linux programmer software for the readind/writing SPI NOR FLASH use the CH341a device.

The Prog24 is a free I2C EEPROM programmer tool for CH341A device based on [QhexEdit2](https://github.com/Simsys/qhexedit2) and
modify [ch341 SPI c-programmer tools](https://github.com/setarcos/ch341prog).

This is a GUI program used widget QhexEditor. For setting the chip parameters you can use the `Detect` button for reading chip parameters (JEDEC information reading) or manually setting it.

The chip database format compatible with EZP2019, EZP2020, EZP2023, Minpro I, XP866+ programmers. You can edit the database use the [EZP Chip data Editor](https://github.com/bigbigmdm/EZP2019-EZP2025_chip_data_editor)

![CH341A EEPROM programmer](img/ch341_spi_programmer.png)

```
mkdir build
cd build
cmake ..
make -j4
sudo make install
```

