#ifndef SLEEP_H
#define SLEEP_H

/*
Sleep functions

By Christoffer Rappmann, christoffer.rappmann@gmail.com
*/

void sleepSetup();
void sleepNow(int sToSleep);
void print_wakeup_reason();

extern RTC_DATA_ATTR int lastWaterDay;
extern RTC_DATA_ATTR int bootCount;

#endif