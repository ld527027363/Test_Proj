#ifndef __UPGRADE_H
#define __UPGRADE_H

typedef struct
{ 
     unsigned char bVersion;
     unsigned char softVersion;
	 unsigned char arrUrl[48];

}UpgradeStruct;

void UpgradeOperation(void);

extern unsigned int g_iTotalBinLength;
extern unsigned int g_iTotalLength;
extern unsigned int g_iStart, g_iEndpos;

extern UpgradeStruct UpgradeList;
extern volatile unsigned char g_bWifiUpgradeFlag;
#endif
