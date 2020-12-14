/**
 * Org:   Nankai University
 * Name:  Liu Zhuang
 * Email: 3331353075@qq.com
 * Date； 2020/12/11
 */
#ifndef TA_LENET5_H
#define TA_LENET5_H

/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 * UUID: b13bc418-3b81-11eb-adc1-0242ac120002
 */
#define TA_LENET5_UUID \
	{ 0xb13bc418, 0x3b81, 0x11eb, \
		{ 0xad, 0xc1, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02} }

/* The function IDs implemented in this TA */
#define TA_LENET5_CMD_INITIALIZE		0
#define TA_LENET5_CMD_TRAIN_BATCH		1
#define TA_LENET5_CMD_PREDICT		    2

#endif /*TA_LENET5_H*/
