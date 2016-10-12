/* Final.c
 * Created: 4/8/2014 5:38:26 PM
 * Author: pmmartin
 */ 

#include "master.h"
#define BAUD BAM  //implemented in master.h

int p;
char string[50];

int main(void)
{	oi_t *sensor_data = oi_alloc();
	init_all(sensor_data);
	lprintf("Good Enough");
	oi_update_sensor(sensor_data);
	while(1)
	{	handle_cmd(sensor_data);
		//servtst();
	}
}