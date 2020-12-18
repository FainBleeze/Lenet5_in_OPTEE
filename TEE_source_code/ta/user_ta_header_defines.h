/**
 * Org:   Nankai University
 * Name:  Liu Zhuang
 * Email: 3331353075@qq.com
 * Date； 2020/12/11
 */

/*
 * The name of this file must NOT be modified
 */

#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

/* To get the TA UUID definition */
#include <lenet5_ta.h>

#define TA_UUID				TA_LENET5_UUID

/*
 * TA properties: multi-instance TA, no specific attribute
 * TA_FLAG_EXEC_DDR is meaningless but mandated.
 */
#define TA_FLAGS			TA_FLAG_EXEC_DDR

/* Provisioned stack size */
#define TA_STACK_SIZE			(1024 * 1024)

/* Provisioned heap size for TEE_Malloc() and friends */
#define TA_DATA_SIZE			(512 * 1024)

/* The gpd.ta.version property */
#define TA_VERSION	"1.0"

/* The gpd.ta.description property */
#define TA_DESCRIPTION	"Lenet5 in OP-TEE"

/* Extra properties */
#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "gp.ta.description", \
	USER_TA_PROP_TYPE_STRING, \
        "Lenet5 TA by LZ." }, \
    { "gp.ta.version", \
	USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif /* USER_TA_HEADER_DEFINES_H */
