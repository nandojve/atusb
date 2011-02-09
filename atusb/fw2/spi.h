#ifndef SPI_H
#define	SPI_H

#include <stdint.h>

void spi_begin(void);
uint8_t spi_io(uint8_t v);
void spi_end(void);
void spi_init(void);

#define	spi_send(v)	(void) spi_io(v)
#define	spi_recv(v)	spi_io(0)

#endif /* !SPI_H */
