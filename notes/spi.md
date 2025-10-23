Serial peripheral interface (SPI)

SPI is an interface bus commonlu used to send data between microcontrollers and small peripherals such as registers, sensors, and SD cards.

It used seperate clock and data lines, along with a select line to choose the device you wish to talk to.

SPI is popular bc the receiving hardware can be a simple shift register. This is a much simpler (and cheaper) piece of hardwre than the full UART tha asynchronous serial requires.


SPI works by selecting the device to communicate with and using a clock signal to pup data out of the transmitting device (such as a sensor) to receiver (microcontroller) like a shift register.

in SPI, only one side generates the clock signal (usually called CLK or SCK or Serial Clock).
    - the side that generates the clock is called the "master" and the side that receives the clock is called the "slave"
    - There is always only one master (usually the microcontroller), but there can be multiple slaves.

When data is sent from the master to a slave, its sent on a data line called MOSI, for Master out/ Slave in

if the slave needs to send a response back to the master, the msater will continue to generate a prearragned number of clock cycles, and the slave will put the data onto a third data line called MISO, for Master In/ Slave Out.

One final line (total 4) is the Slave select (SS). for multiple slaves the master device will need multiple SS lines. To select a slave, the master device will keep that lien low and the rest of the SS liens high.

----
