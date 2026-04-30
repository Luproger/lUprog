/*
 * spi_driver.h
 *
 *  Created on: 30 Apr 2026
 *      Author: Luproger
 */

#ifndef AVR_PROGRAMMER_SPI_DRIVER_H_
#define AVR_PROGRAMMER_SPI_DRIVER_H_


// INIT
void spi_check_conf();
void spi_prog_init();
void spi_prog_deinit();

// FLASH
void spi_fl_Write();
void spi_fl_Read();
void spi_fl_Verify();

// EEPROM
void spi_ee_Write();
void spi_ee_Read();
void spi_ee_Verify();

// FUSEBIT
void spi_fb_Write();
void spi_fb_Read();
void spi_fb_Verify();

// LOCKBIT
void spi_lb_Write();
void spi_lb_Read();
void spi_lb_Verify();

// CONFIG
void spi_cfg_Write();
void spi_cfg_Read();
void spi_cfg_Verify();

#endif /* AVR_PROGRAMMER_SPI_DRIVER_H_ */
