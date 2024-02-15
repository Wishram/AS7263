#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // for close
#include <fcntl.h> // for O_RDWR
#include <linux/i2c-dev.h> // for I2C_SLAVE

#define I2C_DEVICE "/dev/i2c-1" // Specify the I2C device (change if necessary)
#define AS726X_ADDRESS 0x49 // Specify the I2C address of the sensor

// Function to read raw data for a specific channel
int getChannel(int channelRegister, int fd) {
    uint8_t buffer[2]; // Assuming each channel reading is 2 bytes

    // Write the channel register to initiate the reading
    if (write(fd, &channelRegister, 1) != 1) {
        perror("Failed to write channel register");
        return -1;
    }
 
    // Read raw data from the sensor
    if (read(fd, buffer, 2) != 2) {
        perror("Failed to read from the sensor");
        return -1;
    }

    // Combine the two bytes into a single integer value
    return (buffer[0] << 8) | buffer[1];
}

int main() {
    int fd; // File descriptor for I2C communication

    // Open the I2C bus
    if ((fd = open(I2C_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    // Set the I2C slave address
    if (ioctl(fd, I2C_SLAVE, AS726X_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(fd);
        return 1;
    }

    // Read raw data for different channels
    int X_CIE = getChannel(0x08, fd);
    int Y_CIE = getChannel(0x0A, fd);
    int Z_CIE = getChannel(0x0C, fd);
    int NIR = getChannel(0x0E, fd);
    int Dark = getChannel(0x10, fd);
    int Clear = getChannel(0x12, fd);

    // Print the raw data
    printf("X_CIE: %d\n", X_CIE);
    printf("Y_CIE: %d\n", Y_CIE);
    printf("Z_CIE: %d\n", Z_CIE);
    printf("NIR: %d\n", NIR);
    printf("Dark: %d\n", Dark);
    printf("Clear: %d\n", Clear);

    close(fd); // Close the I2C bus before exiting
    return 0;
}