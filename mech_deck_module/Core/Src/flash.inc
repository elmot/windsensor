#define FLASH_KEY1                0x45670123U                          /*!< Flash key1 */
#define FLASH_KEY2                0xCDEF89ABU                          /*!< Flash key2: used with FLASH_KEY1to unlock the FLASH registers access */

inline void FLASH_CLEAR_FLAG_OPTVERR() {
    WRITE_REG(FLASH->SR, FLASH_SR_OPTVERR);
}

void FLASH_Unlock()
{

    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
    {
        /* Authorize the FLASH Registers access */
        WRITE_REG(FLASH->KEYR, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR, FLASH_KEY2);

        /* Verify Flash is unlocked */
        if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
        {
            Error_Handler();
        }
    }
}

void FLASH_Lock()
{
    /* Set the LOCK Bit to lock the FLASH Registers access */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout
  * @retval HAL_StatusTypeDef HAL Status
  */
void FLASH_WaitForLastOperation()
{
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset. */
    while(FLASH->SR & FLASH_SR_BSY)
    {
    }

    /* Check FLASH End of Operation flag  */
    if (FLASH->SR & FLASH_SR_EOP)
    {
        /* Clear FLASH End of Operation pending bit */
        FLASH->SR |= FLASH_SR_EOP;
    }
}
