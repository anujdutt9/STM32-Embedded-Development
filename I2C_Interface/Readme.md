# I2C Interface

This folder implements the code for I2C interface of an Accelerometer sensor of an IMU with STM32F429ZI board.

## Steps for creating CubeMX Project

1. Create a new project in STM32 CubeMX.

2. Select the board: STM32F429ZITx

3. On the left side, under Connectivity, select I2C1.

4. Also, select LED for testing by setting the pin "PG13" or "PG14" to GPIO_OUTPUT.

5. Click on Generate Code and Select MDK ARM.
