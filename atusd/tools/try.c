struct atusd_dsc;


int main(void)
{
	struct atusd_dsc *dsc;
	char tmp;

	dsc = atusd_open();
	read(1, tmp, 1);
	atusd_close(dsc);

	return 0;
}
