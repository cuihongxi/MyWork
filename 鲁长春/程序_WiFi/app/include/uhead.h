/**
 * ����ͷ�ļ�
 */

#ifndef		__UHEAD_H
#define		__UHEAD_H

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "c_types.h"
#include "os_type.h"
#include "mem.h"

#define	DEBUG_LEVEL 1

#if  DEBUG_LEVEL > 0
	#include "uart.h"
	#define		UART_INIT()			uart_init(115200,115200)
	#define  	debug(...) 			os_printf(__VA_ARGS__)
#else
	#define  	debug(...)
	#define		UART_INIT()
#endif


#define		malloc			os_zalloc
#define		free			os_free
#define		strlen(a)		os_strlen((const char*)a)		// ��ȡ�ַ�������
#define	 	memcpy			os_memcpy		//�ڴ濽��
#define		mem_set			os_memset
#endif
