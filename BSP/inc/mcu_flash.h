#ifndef __MCU_FLASH_H__
#define __MCU_FLASH_H__

#include <stdint.h>
//#define ApplicationAddress     0x08020000

#define FLASH_Sector_0_Adress  0x08000000  //16k
#define FLASH_Sector_1_Adress  0x08004000  //16k
#define FLASH_Sector_2_Adress  0x08008000  //16k
#define FLASH_Sector_3_Adress  0x0800C000  //16k
#define FLASH_Sector_4_Adress  0x08010000  //64k
#define FLASH_Sector_5_Adress  0x08020000  //128k
#define FLASH_Sector_6_Adress  0x08040000  //128k
#define FLASH_Sector_7_Adress  0x08060000  //128k

#define FLASH_Sector_8_Adress  0x08080000  //128k
#define FLASH_Sector_9_Adress  0x080A0000  //128k
#define FLASH_Sector_10_Adress 0x080C0000  //128k
#define FLASH_Sector_11_Adress 0x080E0000  //128k

#define FLASH_Sector_0     ((uint16_t)0x0000) /*!< Sector Number 0 */
#define FLASH_Sector_1     ((uint16_t)0x0008) /*!< Sector Number 1 */
#define FLASH_Sector_2     ((uint16_t)0x0010) /*!< Sector Number 2 */
#define FLASH_Sector_3     ((uint16_t)0x0018) /*!< Sector Number 3 */
#define FLASH_Sector_4     ((uint16_t)0x0020) /*!< Sector Number 4 */
#define FLASH_Sector_5     ((uint16_t)0x0028) /*!< Sector Number 5 */
#define FLASH_Sector_6     ((uint16_t)0x0030) /*!< Sector Number 6 */
#define FLASH_Sector_7     ((uint16_t)0x0038) /*!< Sector Number 7 */
#define FLASH_Sector_8     ((uint16_t)0x0040) /*!< Sector Number 8 */
#define FLASH_Sector_9     ((uint16_t)0x0048) /*!< Sector Number 9 */
#define FLASH_Sector_10    ((uint16_t)0x0050) /*!< Sector Number 10 */
#define FLASH_Sector_11    ((uint16_t)0x0058) /*!< Sector Number 11 */
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

typedef  void (*pFunction)(void);

void RestartSys(void);
unsigned char OwnFlashSave(uint32_t save_addr,uint8_t *p,uint16_t number);
void OwnFlashRead(uint32_t read_addr,uint8_t * read_buf,uint16_t read_number);
unsigned char OwnSaveUpgrade(uint32_t save_addr,uint8_t *p,uint16_t number);
void OwnDelSector(void);
void WriteApplicationFlag(unsigned char ch);
#endif
