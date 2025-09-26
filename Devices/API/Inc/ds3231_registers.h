/**
 * @file    ds3231_registers.h
 * @brief   Definiciones de registros y mascaras para el RTC DS3231.
 * @details
 *  Este archivo contiene las direcciones de los registros y máscaras de bits
 *  usadas internamente por el driver DS3231.
 */

#ifndef DS3231_REGISTERS_H
#define DS3231_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup DS3231_registers.h Definiciones de registros
 *  @{
 */

/* Tamaños definidos para buffers. */
#define DS3231_MAX_BLOCK_WRITE   (8)
#define DS3231_MAX_BLOCK_READ    (7)
#define DS3231_ALARM1_BUF_SIZE   (4)
#define DS3231_ALARM2_BUF_SIZE   (3)
#define DS3231_TEMP_BUF_SIZE     (2)

/* -------------------------------------------------------------------------- */
/* Direcciones de Registros del DS3231                                        */
/* -------------------------------------------------------------------------- */
/**
 * Nota: Los registros de hora y fecha están en formato BCD (Binary-Coded Decimal)
 */
#define DS3231_REG_SECONDS       (0x00)  /**< Segundos */
#define DS3231_REG_MINUTES       (0x01)  /**< Minutos */
#define DS3231_REG_HOURS         (0x02)  /**< Horas */
#define DS3231_REG_DAY           (0x03)  /**< Día de la semana */
#define DS3231_REG_DATE          (0x04)  /**< Día del mes */
#define DS3231_REG_MONTH         (0x05)  /**< Mes, bit7 = Century */
#define DS3231_REG_YEAR          (0x06)  /**< Año (00..99) */
#define DS3231_REG_ALARM_1       (0x07)  /**< Inicio Alarm 1 */
#define DS3231_REG_ALARM_2       (0x0B)  /**< Inicio Alarm 2 */
#define DS3231_REG_CONTROL       (0x0E)  /**< Registro de Control */
#define DS3231_REG_STATUS        (0x0F)  /**< Registro de Estado */
#define DS3231_REG_AGING         (0x10)  /**< Registro de corrección de frecuencia */
#define DS3231_REG_TEMP_MSB      (0x11)  /**< Temperatura MSB */
#define DS3231_REG_TEMP_LSB      (0x12)  /**< Temperatura LSB */

/* -------------------------------------------------------------------------- */
/* Bit Map del Registro de Control (0x0E)                                     */
/* -------------------------------------------------------------------------- */
#define DS3231_CTRL_EOSC         (1 << 7)  /**< Oscilador apagado = 1, encendido = 0 */
#define DS3231_CTRL_BBSQW        (1 << 6)  /**< Señal SQW alimentada desde la batería */
#define DS3231_CTRL_CONV         (1 << 5)  /**< Forzar conversión manual de temperatura */
#define DS3231_CTRL_RS2          (1 << 4)  /**< Frecuencia de SQW */
#define DS3231_CTRL_RS1          (1 << 3)  /**< Frecuencia de SQW */
#define DS3231_CTRL_INTCN        (1 << 2)  /**< Control de interrupciones: 1 = INT/SQW como INT */
#define DS3231_CTRL_A2IE         (1 << 1)  /**< Habilita interrupción de Alarm 2 */
#define DS3231_CTRL_A1IE         (1 << 0)  /**< Habilita interrupción de Alarm 1 */

/* Frecuencia de la señal cuadrada */
#define DS3231_SQW_1HZ           (0)                                         /**< RS2=0, RS1=0 */
#define DS3231_SQW_1024HZ        (DS3231_CTRL_RS1)                           /**< RS2=0, RS1=1 */
#define DS3231_SQW_4096HZ        (DS3231_CTRL_RS2)                           /**< RS2=1, RS1=0 */
#define DS3231_SQW_8192HZ        (DS3231_CTRL_RS2 | DS3231_CTRL_RS1)         /**< RS2=1, RS1=1 */

/* -------------------------------------------------------------------------- */
/* Bit Map del Registro de Estado (0x0F)                                      */
/* -------------------------------------------------------------------------- */
#define DS3231_STATUS_OSF        (1 << 7)  /**< Flag de que el oscilador se detuvo. */
#define DS3231_STATUS_EN32KHZ    (1 << 3)  /**< Salida de 32kHz habilitada. */
#define DS3231_STATUS_BSY        (1 << 2)  /**< Conversión de temperatura en progreso. */
#define DS3231_STATUS_A2F        (1 << 1)  /**< Flag de la Alarma 2. */
#define DS3231_STATUS_A1F        (1 << 0)  /**< Flag de la Alarma 1. */

/** @} */ // end of group DS3231_Registers

#ifdef __cplusplus
}
#endif

#endif /* DS3231_REGISTERS_H */