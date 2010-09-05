#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>


enum {
	VDD_OFF	= 1 << 6,	/* VDD disable, PD06 */
	MxSx	= 1 << 8,	/* CMD, PD08 */
	CLK	= 1 << 9,	/* CLK, PD09 */
	SCLK	= 1 << 10,	/* DAT0, PD10 */
	SLP_TR	= 1 << 11,	/* DAT1, PD11 */
	IRQ	= 1 << 12,	/* DAT2, PD12 */
	nSEL	= 1 << 13,	/* DAT3/CD, PD13 */
};


#define SOC_BASE	0x10000000

#define	REG(n)	(*(volatile uint32_t *) (dsc->mem+(n)))

#define CGU(n)	REG(0x00000+(n))
#define GPIO(n)	REG(0x10000+(n))
#define MSC(n)	REG(0x21000+(n))

#define PDDATS	GPIO(0x314)	/* port D data set */
#define PDDATC	GPIO(0x318)	/* port D data clear */
#define PDFUNS	GPIO(0x344)	/* port D function set */
#define PDFUNC	GPIO(0x348)	/* port D function clear */
#define PDDIRS	GPIO(0x364)	/* port D direction set */
#define PDDIRC	GPIO(0x368)	/* port D direction clear */

#define MSC_STRPCL	MSC(0x00)	/* Start/stop MMC/SD clock */
#define MSC_CLKRT	MSC(0x08)	/* MSC Clock Rate */

#define CLKGR		CGU(0x0020)	/* Clock Gate */
#define MSCCDR		CGU(0x0068)	/* MSC device clock divider */


#define	PAGE_SIZE	4096


struct atusd_dsc {
	int fd;
	void *mem;
};


struct atusd_dsc *atusd_open(void)
{
	struct atusd_dsc *dsc;

	dsc = malloc(sizeof(*dsc));
	if (!dsc) {
		perror("malloc");
		exit(1);
	}

	dsc->fd = open("/dev/mem", O_RDWR);
	if (dsc->fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	dsc->mem = mmap(NULL, PAGE_SIZE*3*16, PROT_READ | PROT_WRITE,
	    MAP_SHARED, dsc->fd, SOC_BASE);
        if (dsc->mem == MAP_FAILED) {
                perror("mmap");
                exit(1);
        }

	/* set the output levels */
	PDDATS = nSEL | VDD_OFF;
	PDDATC = SCLK | SLP_TR;

	/* take the GPIOs away from the MMC controller */
	PDFUNC = MxSx | SCLK | SLP_TR | IRQ | nSEL;
	PDFUNS = CLK;

	/* set the pin directions */
	PDDIRC = IRQ;
	PDDIRS = MxSx | CLK | SCLK | SLP_TR | nSEL;

	/* enable power */
	PDDATC = VDD_OFF;

	/* set the MSC clock to 316 MHz / 21 = 16 MHz */
	MSCCDR = 20;
	/*
	 * Enable the MSC clock. We need to do this before accessing any
	 * registers of the MSC block !
	 */
	CLKGR &= ~(1 << 7);
	/* bus clock = MSC clock / 1 */
	MSC_CLKRT = 0;
	/* start MMC clock output */
	MSC_STRPCL = 2;

	return dsc;
}


void atusd_close(struct atusd_dsc *dsc)
{
	/* stop the MMC clock */
	MSC_STRPCL = 1;

	/* cut the power */
	PDDATS = VDD_OFF;

	/* make all MMC pins inputs */
	PDDIRC = MxSx | CLK | SCLK | SLP_TR | IRQ | nSEL;
}
