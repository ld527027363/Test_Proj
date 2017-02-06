
#include "global.h"

#include "mcu_flash.h"
#include "special_defs.h"


void RestartSys(void)
{
    unsigned int JumpAddress;
	
    pFunction Jump_To_Application;     
    JumpAddress = *(vu32*) (0x8000000+4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(vu32*) 0x8000000); 
    Jump_To_Application();
}

uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
  
    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;  
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;  
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;  
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;  
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;  
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;  
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;  
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;  
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;  
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;  
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;  
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;  
    }
    return sector;
}

void OwnDelSector(void)
{
    uint32_t StartSector;
    StartSector = GetSector(FLASH_Sector_6_Adress);
    FLASH_Unlock(); 
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    while (FLASH_EraseSector(StartSector, VoltageRange_3) != FLASH_COMPLETE)
    {
    }
    FLASH_Lock();
}

unsigned char OwnSaveUpgrade(uint32_t save_addr,uint8_t *p,uint16_t number)
{
    uint32_t save_addr_temp;
    save_addr_temp = save_addr;
    FLASH_Unlock(); 
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    while (save_addr < number+save_addr_temp)
    {
        if (FLASH_ProgramByte(FLASH_Sector_6_Adress+save_addr, *p++) == FLASH_COMPLETE)  
        {
            save_addr = save_addr + 1;
        }
        else //Ð´Ê§°Ü
        { 
            FLASH_Lock();
            return 0;
        }
    }
    FLASH_Lock();
    return 1;
}

unsigned char OwnFlashSave(uint32_t save_addr,uint8_t *p,uint16_t number)
{
    uint32_t StartSector, save_addr_temp;
    StartSector = GetSector(save_addr);
    save_addr_temp = save_addr;
    FLASH_Unlock(); 
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    while (FLASH_EraseSector(StartSector, VoltageRange_3) != FLASH_COMPLETE)
    {
    }
    while (save_addr < number+save_addr_temp)
    {
        if (FLASH_ProgramByte(save_addr, *p++) == FLASH_COMPLETE)  
        {
            save_addr = save_addr + 1;
        }
        else //Ð´Ê§°Ü
        { 
            FLASH_Lock();
            return 0;
        }
    }
    FLASH_Lock();
    return 1;
}

void OwnFlashRead(uint32_t read_addr,uint8_t * read_buf,uint16_t read_number)
{
    uint32_t Address_ready;
    Address_ready = read_addr;
    while (Address_ready < read_addr+read_number)
    {
        *read_buf++ = (*(__IO uint8_t*)Address_ready); 
        Address_ready = Address_ready + 1;
    }  
}

#define APPLICATIONFALG  ADDR_FLASH_SECTOR_3
unsigned char CheckApplicationFlag(void)
{
    unsigned char bTemp[2];
    OwnFlashRead(APPLICATIONFALG, bTemp, 1);
    if(bTemp[0] == 0x55) //ÓÐÓ¦ÓÃÉý¼¶
    {
        return 1;
    }
    else return 0;
}

void WriteApplicationFlag(unsigned char ch)
{
    unsigned char bTemp[2];
    bTemp[0] = ch;
    OwnFlashSave(APPLICATIONFALG,  bTemp, 1);
}
