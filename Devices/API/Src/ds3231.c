/**
 * @file    ds3231.c
 * @brief   Implementación del driver DS3231, utilizando ds3231_port.c.
 */

#include "ds3231.h"

/* -------------------------------------------------------------------------- */
/*  API pública                                                               */
/* -------------------------------------------------------------------------- */

static DS3231_Status DS3231_parse_hal_status(HAL_StatusTypeDef hal_status)
{
    switch (hal_status)
    {
        case HAL_OK:      return DS3231_OK;
        case HAL_TIMEOUT: return DS3231_TIMEOUT;
        case HAL_ERROR:   return DS3231_ERROR;
        case HAL_BUSY:    return DS3231_BUSY;
        default:          return DS3231_ERROR;
    }
}

/** -------------------------------------------------------------------------- 
* Funciones de inicializacion, solo verifica la presencia del device en el Bus de I²C.                                  
* ---------------------------------------------------------------------------- 
*/
DS3231_Status DS3231_Init(void)
{
    return DS3231_parse_hal_status(DS3231_is_ready());
}

/** -------------------------------------------------------------------------- 
* Funciones de lectura y escritura de tiempo                                        
* ---------------------------------------------------------------------------- 
*/
DS3231_Status DS3231_ReadTime(DS3231_Time *time)
{
    if (!time) return DS3231_INVALID_PARAM;

    DS3231_Status status = DS3231_OK;
    uint8_t buf[3];

    status = DS3231_parse_hal_status(DS3231_register_block_read(DS3231_REG_SECONDS, buf,  sizeof(buf)));
    if (status != DS3231_OK) return status;

    time->seconds = (uint8_t)bcd2dec(buf[0] & 0x7F);
    time->minutes = (uint8_t)bcd2dec(buf[1] & 0x7F);
    time->hours   = (uint8_t)bcd2dec(buf[2] & 0x3F);
    time->day     = (uint8_t)(buf[3] & 0x07); // El dia no hace falta convertirlo, porque es el dia semanal (1...7)
    time->date    = (uint8_t)bcd2dec(buf[4] & 0x3F);
    time->month   = bcd2dec(buf[5] & 0x1F); // Enmascaro ya qque el bit7 es el centenario.
    time->year    = bcd2dec(buf[6]);

    return status;
}

static DS3231_Status DS3231_SetTimeBCD(const DS3231_Time *time)
{
    if (!time) return DS3231_INVALID_PARAM;

    DS3231_Status status = DS3231_OK;
    uint8_t buf[DS3231_MAX_BLOCK_WRITE];

    buf[0] = DS3231_REG_SECONDS;
    buf[1] = (uint8_t)(time->seconds & 0x7F);
    buf[2] = (uint8_t)(time->minutes & 0x7F);
    buf[3] = (uint8_t)(time->hours   & 0x3F);
    buf[4] = (uint8_t)(time->day     & 0x07);
    buf[5] = (uint8_t)(time->date    & 0x3F);
    buf[6] = time->month;                     
    buf[7] = time->year;

    status = DS3231_parse_hal_status(DS3231_register_block_write(buf,  sizeof(buf)));
    return status;
}

DS3231_Status DS3231_SetTime(uint8_t year, uint8_t month, uint8_t date,
                                    uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if ((sec > 59 || min > 59 || hour > 23) || 
        (day < 1  || day > 7) ||
        (date < 1 || date > 31) || 
        (month < 1 || month > 12) || 
        year > 99)
    {
        return DS3231_INVALID_PARAM;
    }

    DS3231_Time time;
    time.seconds = dec2bcd(sec);
    time.minutes = dec2bcd(min);
    time.hours   = dec2bcd(hour);
    time.day     = (uint8_t)(day & 0x07);
    time.date    = dec2bcd(date);
    time.month   = dec2bcd(month);
    time.year    = dec2bcd(year);

    return DS3231_SetTimeBCD(&time);
}

/** -------------------------------------------------------------------------- 
* Funcion de lectura de la temperatura                                       
* ---------------------------------------------------------------------------- 
*/
DS3231_Status DS3231_GetTemperature(float *temp)
{
    if (!temp)
        return DS3231_INVALID_PARAM;

    uint8_t buf[DS3231_TEMP_BUF_SIZE];

    if (DS3231_parse_hal_status(DS3231_register_block_read(DS3231_REG_TEMP_MSB, buf, sizeof(buf))) != DS3231_OK)
        return DS3231_ERROR;

    /** - Conversion de valores a grados celsius.
    *   El MSB contiene la parte entera con signo.
    *   Los 2 bits altos del LSB contienen la fracción en pasos de 0.25°C.
    */
    *temp = (int8_t)buf[0] + ((buf[1] >> 6) * 0.25f);

    return DS3231_OK;
}

/* -------------------------------------------------------------------------- */
/* Funciones de lectura y control de Estado                                   */
/* -------------------------------------------------------------------------- */


DS3231_Status DS3231_GetStatus(uint8_t *status)
{
    if (!status) return DS3231_INVALID_PARAM;
    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_STATUS, status)) != DS3231_OK)
        return DS3231_ERROR;
    return DS3231_OK;
}


DS3231_Status DS3231_ClearStatus(uint8_t mask)
{
    uint8_t status;
    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_STATUS, &status)) != DS3231_OK) 
        return DS3231_ERROR;

    status &= ~mask; // Limpia solo los bits de la máscara

    if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_STATUS, status)) != DS3231_OK) 
        return DS3231_ERROR;

    return DS3231_OK;
}

/* -------------------------------------------------------------------------- */
/* Funciones de lectura y control de Control                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Lee el registro de Control (0x0E).
 */
DS3231_Status DS3231_GetControl(uint8_t *control)
{
    if (!control) return DS3231_INVALID_PARAM;

    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_CONTROL, control)) != DS3231_OK)
        return DS3231_ERROR;

    return DS3231_OK;
}

/**
 * @brief Configura los bits indicados por la máscara.
 * @param mask Mascara con los bits a configurar (ej: DS3231_CTRL_INTCN | DS3231_CTRL_A1IE)
 */
DS3231_Status DS3231_UpdateControl(uint8_t mask)
{
    uint8_t ctrl;

    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_CONTROL, &ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    ctrl |= mask;

    if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_CONTROL, ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    return DS3231_OK;
}

/**
 * @brief Limpia los bits indicados por la máscara.
 * @param mask Contiene los bits a limpiar.
 */
DS3231_Status DS3231_ClearControl(uint8_t mask)
{
    uint8_t ctrl;

    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_CONTROL, &ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    ctrl &= ~mask;

    if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_CONTROL, ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    return DS3231_OK;
}

/* -------------------------------------------------------------------------- */
/* Habilita la salida de salida de 32 KHz                                     */
/* -------------------------------------------------------------------------- */

DS3231_Status DS3231_Enable32KHz(bool enable)
{
    uint8_t status, control;

    if (enable) {
        // Leo el registro de control, para verificar que el oscilador esté encendido (EOSC = 0), en caso de que no lo esté lo enciendo.
        if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_CONTROL, &control)) != DS3231_OK) return DS3231_ERROR;

        if (control & DS3231_CTRL_EOSC) {
            // Si el bit de EOSC esta en 1, lo limpio y escribo nuevamente el registro de control.
            control &= ~DS3231_CTRL_EOSC;
            if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_CONTROL, control)) != DS3231_OK)
                return DS3231_ERROR;
        }
    }

    // Leo el estado del registro de estado.
    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_STATUS, &status)) != DS3231_OK) 
        return DS3231_ERROR;

    if (enable) 
        status |=  DS3231_STATUS_EN32KHZ;
    else        
        status &= ~DS3231_STATUS_EN32KHZ;

    if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_STATUS, status)) != DS3231_OK) 
        return DS3231_ERROR;

    return DS3231_OK;
}


/* -------------------------------------------------------------------------- */
/* Configura la frecuencia de la onda cuadrada                                */
/* -------------------------------------------------------------------------- */

DS3231_Status DS3231_SetSQWFreq(uint8_t rs_bits)
{
    uint8_t ctrl;

    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_CONTROL, &ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    // Verifico que el oscilador esté encendido (EOSC = 0), en caso de que no lo esté lo enciendo.
    if (ctrl & DS3231_CTRL_EOSC)
        ctrl &= ~DS3231_CTRL_EOSC;

    // Limpio el bit INTCN, para asegurarme que el modo sea SQW y no INT.
    ctrl &= ~ DS3231_CTRL_INTCN;

    // Limpio bits RS1 y RS2
    ctrl &= (uint8_t)~(DS3231_CTRL_RS1 | DS3231_CTRL_RS2);

    // Configuro la nueva nueva frecuencia
    ctrl |= (rs_bits & (DS3231_CTRL_RS1 | DS3231_CTRL_RS2));

    if (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_CONTROL, ctrl)) != DS3231_OK)
        return DS3231_ERROR;

    return DS3231_OK;
}

/* -------------------------------------------------------------------------- */
/* Funciones de lectura y configuracion del envejicimiento                    */
/* -------------------------------------------------------------------------- */

DS3231_Status DS3231_SetAging(int8_t offset)
{
    return (DS3231_parse_hal_status(DS3231_register_write(DS3231_REG_AGING, offset)) == DS3231_OK) ? DS3231_OK : DS3231_ERROR;
}

DS3231_Status DS3231_GetAging(int8_t *offset)
{
    if (!offset) 
        return DS3231_INVALID_PARAM;

    uint8_t value;

    if (DS3231_parse_hal_status(DS3231_register_read(DS3231_REG_AGING, &value)) != DS3231_OK)
        return DS3231_ERROR;

    *offset = (int8_t)value;
    return DS3231_OK;
}
