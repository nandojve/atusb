#ifndef ATSPI_H
#define	ATSPI_H


#include <stdint.h>
#include <usb.h>


int atspi_error(void);
int atspi_clear_error(void);

usb_dev_handle *atspi_open(void);
void atspi_close(usb_dev_handle *dev);

void atspi_reset(usb_dev_handle *dev);
void atspi_reset_rf(usb_dev_handle *dev);

void atspi_reg_write(usb_dev_handle *dev, uint8_t reg, uint8_t value);
uint8_t atspi_reg_read(usb_dev_handle *dev, uint8_t reg);

#endif /* !ATSPI_H */
