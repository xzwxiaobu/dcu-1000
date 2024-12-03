#ifndef _SWCH_IODEF_H
#define _SWCH_IODEF_H


/*
 * 输入GPIO定义
 */
typedef struct
{
    int16u B00 : 1;    /* 00 */
    int16u B01 : 1;    /* 01 */
    int16u B02 : 1;    /* 02 */
    int16u B03 : 1;    /* 03 */
    int16u B04 : 1;    /* 04 */
    int16u B05 : 1;    /* 05 */
    int16u B06 : 1;    /* 06 */
    int16u B07 : 1;    /* 07 */
    int16u B08 : 1;    /* 08 */
    int16u B09 : 1;    /* 09 */
    int16u B10 : 1;    /* 10 */
    int16u B11 : 1;    /* 11 */
    int16u B12 : 1;    /* 12 */
    int16u B13 : 1;    /* 13 */
    int16u B14 : 1;    /* 14 */
    int16u B15 : 1;    /* 15 */
}tGPIO_Bits;

typedef struct
{
    /* RB */
    int16u              : 2;    /* 00 - 01          */
    int16u Q0_Reset     : 1;    /* RB2  RESET/SELECT1-Q */
    int16u IN_PN0       : 1;    /* RB3  IN-PN0      */
    int16u IN_2         : 1;    /* RB4  IN-2        */
    int16u              : 1;    /* RB5              */
    int16u Q1_Switch    : 1;    /* RB6  Q1-SWITCH   */
    int16u Q2_Switch    : 1;    /* RB7  Q2-SWITCH   */
    int16u              : 3;    /* 8 - 10           */
    int16u Q3_PN0       : 1;    /* RB11 Q-PN0       */
    int16u              : 4;    /* 12 - 15          */
    
    /* RC */    
    int16u              : 13;   /* 00 - 12              */
    int16u K5_NC        : 1;    /* 13: RC13 K-S5  K5_NC */
    int16u K6_NC        : 1;    /* 14: RC14 K-S6  K6_NC */
    int16u              : 1;    /* 15                   */
    
    /* RD */    
    int16u K4_NC        : 1;    /* 00: RD0 K-S4   K4_NC */
    int16u DCU_SF_NC    : 1;    /* 01: RD1 K-S7   K11_NC DCU-SF继电器触点 */
    int16u              : 4;    /* 02 - 05 */
    int16u K12_NC       : 1;    /* 06  RD6  K-S8  K12_NC    */
    int16u K13_NC       : 1;    /* 07  RD7  K-S9  K13_NC    */
    int16u K0_NC        : 1;    /* 08: RD8  K-S0  K0_NC     */
    int16u K1_NC        : 1;    /* 09: RD9  K-S1  K1_NC     */
    int16u K2_NC        : 1;    /* 10: RD10 K-S2  K2_NC     */
    int16u K3_NC        : 1;    /* 11: RD11 K-S3  K3_NC     */
    int16u              : 4;    /* 12 - 15 */
        
    /* RE */    
    int16u K23_NC       : 1;    /* 00: RE0  K-S12  K23_NC               */
    int16u DCU_SF_Coil  : 1;    /* 01: RE1  K11-PN DCU-SF 继电器线圈    */
    int16u DCU_BY_Coil  : 1;    /* 02: RE2  K21-PN DCU-BY 继电器线圈    */
    int16u K0_Coil      : 1;    /* 03: RE3  K0-N   K0   继电器线圈      */
    int16u              : 1;    /* 04: RE4    */
    int16u K123_Coil    : 1;    /* 05: RE5  K1-N   K123 继电器线圈并联  */
    int16u K456_Coil    : 1;    /* 06: RE6  K2-N   K456 继电器线圈并联  */
    int16u IN_0         : 1;    /* 07: RE7  IN-0                        */
    int16u              : 8;    /* 08 - 15 */
    
    /* RF */    
    int16u DCU_BY_NC    : 1;    /* 00: RF0  K-S10   K21_NC DCU-BY继电器触点 */
    int16u K22_NC       : 1;    /* 01: RF1  K-S11   K22_NC    */
    int16u              : 14;   /* 02 - 15              */
    
    /* RG */    
    int16u              : 6;    /* 00 - 05              */
    int16u IN_1         : 1;    /* 06 : RG6 IN-1        */
    int16u              : 9;    /* 07 - 15              */
}tSFSW_GPIO_Fun;


typedef struct
{
    /* RB */
    int16u          : 2;    /* 00 - 01              */
    int16u Q00      : 1;    /* RB2  RESET/SELECT1-Q */
    int16u I21      : 1;    /* RB3  IN-PN0          */
    int16u I20      : 1;    /* RB4  IN-2            */
    int16u          : 1;    /* RB5                  */
    int16u Q01      : 1;    /* RB6  Q1-SWITCH       */
    int16u Q02      : 1;    /* RB7  Q2-SWITCH       */
    int16u          : 3;    /* 8 - 10               */
    int16u Q03      : 1;    /* RB11 Q-PN0           */
    int16u          : 4;    /* 12 - 15              */
    
    /* RC */    
    int16u          : 13;   /* 00 - 12              */
    int16u I05      : 1;    /* 13: RC13 K-S5  K5_NC */
    int16u I06      : 1;    /* 14: RC14 K-S6  K6_NC */
    int16u          : 1;    /* 15                   */
    
    /* RD */    
    int16u I04      : 1;    /* 00: RD0 K-S4   K4_NC */
    int16u I07      : 1;    /* 01: RD1 K-S7   K11_NC DCU-SF继电器触点 */
    int16u          : 4;    /* 02 - 05                  */
    int16u I08      : 1;    /* 06  RD6  K-S8  K12_NC    */
    int16u I09      : 1;    /* 07  RD7  K-S9  K13_NC    */
    int16u I00      : 1;    /* 08: RD8  K-S0  K0_NC     */
    int16u I01      : 1;    /* 09: RD9  K-S1  K1_NC     */
    int16u I02      : 1;    /* 10: RD10 K-S2  K2_NC     */
    int16u I03      : 1;    /* 11: RD11 K-S3  K3_NC     */
    int16u          : 4;    /* 12 - 15 */
        
    /* RE */    
    int16u I12      : 1;    /* 00: RE0  K-S12  K23_NC               */
    int16u I13      : 1;    /* 01: RE1  K11-PN DCU-SF 继电器线圈    */
    int16u I14      : 1;    /* 02: RE2  K21-PN DCU-BY 继电器线圈    */
    int16u I15      : 1;    /* 03: RE3  K0-N   K0   继电器线圈      */
    int16u          : 1;    /* 04: RE4          */
    int16u I17      : 1;    /* 05: RE5  K1-N   K123 继电器线圈并联  */
    int16u I18      : 1;    /* 06: RE6  K2-N   K456 继电器线圈并联  */
    int16u I16      : 1;    /* 07: RE7  IN-0    */
    int16u          : 8;    /* 08 - 15 */
    
    /* RF */    
    int16u I10      : 1;    /* 00: RF0  K-S10   K21_NC DCU-BY继电器触点 */
    int16u I11      : 1;    /* 01: RF1  K-S11   K22_NC */
    int16u          : 14;   /* 02 - 15 */
    
    /* RG */    
    int16u          : 6;    /* 00 - 05          */
    int16u I19      : 1;    /* 06 : RG6 IN-1    */
    int16u          : 9;    /* 07 - 15          */
}tSFSW_GPIO_Io;


typedef union
{
    tSFSW_GPIO_Fun  funBits;
    tSFSW_GPIO_Io   ioBits;
    tGPIO_Bits      Gpios[6];
    int16u          Words[6];
}tSFSW_GPIOs;


#endif	/* _SWCH_IODEF_H */
