/**
  ******************************************************************************
  * @file           shell.c
  * @author         ��ô��
  * @brief          shell �����������֧��  TAB �����ȫ���������Ҽ�ͷ ��BACKSPACE��ɾ
  * @note
  * <pre>
  * ʹ�ò���:
  *    0.��ʼ��Ӳ�����֡�
  *    1.��дӲ����Ӧ��void puts(char * buf , uint16_t len) ���ͺ�����
  *    2.shell_init(sign,puts) ��ʼ�������־��Ĭ�������
  *    3.�½�һ��  shellinput_t shellx , ��ʼ����� shell_input_init(&shellx,puts,...);
  *    4.���յ�һ�����ݺ󣬵��� shell_input(shellx,buf,len)
  *    *.  ��Ҫע����������ú� shell_register_command ����ע�ᡣ
  *    *.. shell_register_confirm() ��ע���ѡ������([Y/N]ѡ��)
  * </pre>
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 GoodMorning
  *
  ******************************************************************************
  */
/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "shell.h"
#include "containerof.h"

fmt_puts_t current_puts = NULL;
fmt_puts_t default_puts = NULL;

/* Private types ------------------------------------------------------------*/

union uncmd {
	struct {// ����ŷ�Ϊ�����������  
		uint32_t CRC2      : 8;
		uint32_t CRC1      : 8;///< ��ʮ��λΪ���� crc У����
		uint32_t Sum       : 5;///< �����ַ����ܺ�
		uint32_t Len       : 5;///< �����ַ��ĳ��ȣ�5 bit ��������Ȳ��ܳ���31���ַ�
		uint32_t FirstChar : 6;///< �����ַ��ĵ�һ���ַ�
	}part;
	uint32_t ID;               ///< �ɴ˺ϲ�Ϊ 32 λ��������
};

/* Private macro ------------------------------------------------------------*/

#define VERSION      "V2.0.4"

#if USE_AVL_TREE 
	#define NEXT(x)          avl_next(x)
	#define FIRST(root)      avl_first(root)
	#define ROOT(root)       ((root)->avl_node)
#else 
	#define NEXT(x)          ((x)->next)
	#define FIRST(root)      ((root)->next)
	#define ROOT(root)       ((root)->next)
#endif

/* Private variables --------------------------------------------------------*/

static const  unsigned char F_CRC8_Table[256] = {//����,��λ���� x^8+x^5+x^4+1
	0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e,
	0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d,
	0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8,
	0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb,
	0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13,
	0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50,
	0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c, 0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95,
	0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6,
	0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54,
	0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17,
	0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2,
	0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91,
	0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
	0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a,
	0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef,
	0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac
};

static const  unsigned char B_CRC8_Table[256] = {//����,��λ���� x^8+x^5+x^4+1
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

/// ������ʼ�㣬Ŀ¼��
static cmd_root_t shellcmdroot = {0};

/* Global variables ---------------------------------------------------------*/

/// Ĭ�������־�����޸�
char DEFAULT_INPUTSIGN[COMMANDLINE_MAX_LEN] = "~ # ";

/* Private function prototypes -----------------------------------------------*/
static void   shell_getchar     (struct shell_input * shellin , char ch);
static void   shell_backspace   (struct shell_input * shellin) ;
static void   shell_tab         (struct shell_input * shellin) ;
       void   shell_confirm     (struct shell_input * shellin ,char * info ,cmd_fn_t yestodo);

#if (COMMANDLINE_MAX_RECORD)//�����������ʷ��¼
	static char * shell_record(struct shell_input * shellin);
	static void   shell_show_history(struct shell_input * shellin,int32_t LastOrNext);
#else
#	define shell_record(x)
#	define shell_show_history(x,y)
#endif //#if (COMMANDLINE_MAX_RECORD)//�����������ʷ��¼

/* Gorgeous Split-line ------------------------------------------------------*/

/**
  * @brief    ����ƥ�䣬���� cmd �ҵ���Ӧ�Ŀ��ƿ�
  * @param    cmdindex : �����
  * @param    root     : ����������ʼ������
  * @return   ���� cmd �����ַ�����Ӧ�Ŀ��ƿ�
*/
static struct shellcommand *shell_search_cmd(cmd_root_t * root , uint32_t cmdindex)
{
	struct shellcommand * command ;
	cmd_entry_t *node = ROOT(root);

	#if USE_AVL_TREE 
		while (node) {
			command = container_of(node, struct shellcommand, node);
			if (cmdindex < command->ID)
				node = node->avl_left;
			else 
			if (cmdindex > command->ID)
				node = node->avl_right;
			else 
				return command;
		}
	#else 
		for ( ; node ; node = node->next ) {
			command = container_of(node, struct shellcommand, node);
			if (command->ID > cmdindex)
				return NULL;
			else
			if (command->ID == cmdindex)
				return command;
		}
	#endif 
  
	return NULL;
}

/**
  * @brief    ����������¼
  * @param    root     : ����������ʼ������
  * @param    newcmd   : ��������ƿ�
  * @return   �ɹ����� 0
*/
static int32_t shell_insert_cmd(cmd_root_t * root , struct shellcommand * newcmd)
{
	struct shellcommand * command ;
	cmd_entry_t **node = &ROOT(root) ;

	#if USE_AVL_TREE 
		/* ��ƽ�������������ѯϵͳ */
		cmd_entry_t *parent = NULL;

		/* Figure out where to put new node */
		while (*node) {
			command = container_of(*node, struct shellcommand, node);
			parent = *node;
			if (newcmd->ID < command->ID)
				node = &((*node)->avl_left);
			else 
			if (newcmd->ID > command->ID)
				node = &((*node)->avl_right);
			else
				return 1;
		}

		/* Add new node and rebalance tree. */
		avl_insert(root,&newcmd->node,parent,node);
	#else 
		/* �õ���������ѯϵͳ */
		for ( ; *node ; node = &((*node)->next) ) {
			command = container_of(*node, struct shellcommand, node);
			if (newcmd->ID == command->ID) 
				return -1;
			else
			if (command->ID > newcmd->ID)
				break ;
		}
		newcmd->node.next = *node; 
		*node = &newcmd->node;
	#endif 
	return 0;
}


/** 
  * @brief  ���������ʼ�ַ���Ϊ str ���������� 
  * @param  str      : ��ʼ�ַ���
  * @param  len      : ��ʼ�ַ�������
  * @param  checkout : ����ڴ�
  * @param  checkmax : ���������������������򷵻� 0 
  * @return ���ؼ���������Ŀ��
*/
static int32_t checkout(char * str,int32_t len,struct shellcommand ** checkout , int32_t checkmax)
{
	uint32_t index , end;
	int32_t      matchnums = 0;
	struct shellcommand * shellcmd = NULL;
	cmd_entry_t  * node = ROOT(&shellcmdroot);

	/* ����ĸ��ͬ���ҳ��Ȳ�С�� len �ĵ���Ϊ��ʼ�㣬��һ����ĸ��ͷ�ĵ���Ϊ������ */
	index = ((uint32_t)(*str)<<26) | (len << 21) ;
	end = (uint32_t)(*str + 1)<<26 ; 

	/* ���ҵ���ʼƥ��� */
	#if USE_AVL_TREE 
		/* index �����ڣ����ҽ������ shell_cmd ��� index �ô���Ϊ��ʼƥ��� */
		while ( node ){
			shellcmd = container_of(node,struct shellcommand, node);	
			node = (index < shellcmd->ID) ? node->avl_left : node->avl_right;
		}

		if (shellcmd)
		   node = &shellcmd->node ;
	#else 
		/* ���ҵ�����ĸ��ͬ�ĵ���Ϊ��ʼ�� */
		for ( ; node ; node = NEXT(node)) { 
			shellcmd = container_of(node, struct shellcommand, node);
			if (shellcmd->ID > index)
				break;
		}
	#endif

	for( ; node ; node = NEXT(node) ) {
		/* �Ա�������ַ��������ǰ len ���ַ��� str ��ͬ,������������ */
		shellcmd = container_of(node,struct shellcommand, node);
		if (shellcmd->ID > end) {
			break ;
		}

		if (0 == memcmp(shellcmd->name, str, len)){ 
			checkout[matchnums] = shellcmd;
			if (++matchnums > checkmax) {
				return 0;    
			}
		}
	}

	return matchnums;
}


#if (COMMANDLINE_MAX_RECORD) //�����������ʷ��¼

/**
  * @author   ��ô��
  * @brief    ��¼�˴����е��������
  * @param    
  * @return   ���ؼ�¼��ַ
*/
static char * shell_record(struct shell_input * shellin)
{	
	char *  history = &shellin->history[shellin->htywrt][0];
	
	shellin->htywrt  = (shellin->htywrt + 1) % COMMANDLINE_MAX_RECORD;
	shellin->htyread = shellin->htywrt;

	memcpy(history,shellin->cmdline,shellin->tail);
	history[shellin->tail] = 0;
	
	return history;
}


/**
  * @author   ��ô��
  * @brief    �����¼�ͷ����ʾ���������������˴�ֻ��¼������ε�����
  * @param    void
  * @return   don't care
*/
static void shell_show_history(struct shell_input * shellin,int32_t LastOrNext)
{
	int32_t len = 0;
	
	printf("\33[2K\r%s",shellin->sign);//"\33[2K\r" ��ʾ�����ǰ��

	if (!LastOrNext) //�ϼ�ͷ����һ������
		shellin->htyread = (!shellin->htyread) ? (COMMANDLINE_MAX_RECORD - 1) : (shellin->htyread - 1);
	else       //�¼�ͷ
	if (shellin->htyread != shellin->htywrt)
		shellin->htyread = (shellin->htyread + 1) % COMMANDLINE_MAX_RECORD;

	if (shellin->htyread != shellin->htywrt){ //����ʷ��¼�����������ڴ� 
		for (char * history = &shellin->history[shellin->htyread][0]; *history ; ++len)
			shellin->cmdline[len] = *history++;
	}
	
	shellin->cmdline[len] = 0; //��ӽ�����
	shellin->tail = len ;
	shellin->edit = len ;

	if (len)
		printl(shellin->cmdline,len); //��ӡ����������
}

#endif //#if (COMMANDLINE_MAX_RECORD) //�����������ʷ��¼



/** 
  * @brief    ���� table ������
  * @param    input
  * @return   don't care
*/
static void shell_tab(struct shell_input * shellin)
{
	struct shellcommand * match[10];  
	char  *  str = shellin->cmdline;
	int32_t  len = shellin->tail;
	int32_t matchnums = 0 ; 
	
	/* Shave off any leading spaces */
	for ( ; *str == ' ' ; ++str) {
		--len; 
	}

	if (*str == 0 || len == 0 ){
		return ;
	}

	/* ���û���������������ַ��������� */
	matchnums = checkout(str,len,match,10);
	if (!matchnums){ 
		return ; 
	}

	/* ����༭λ�ò���ĩ�ˣ��Ȱѹ���Ƶ�ĩ�� */
	if (shellin->edit != shellin->tail) { 
		printl(&shellin->cmdline[shellin->edit],shellin->tail - shellin->edit);
		shellin->edit = shellin->tail;
	}

	if (1 == matchnums){
		/* ���ֻ�ҵ���һ�����������ǰ������ַ�����ֱ�Ӳ�ȫ�����ӡ */
		for(char * fmt = match[0]->name + len ; *fmt ;++fmt){
			shell_getchar(shellin,*fmt);
		}
		shell_getchar(shellin,' ');
	}
	else {  
		/* �����ֹһ�����������ǰ������ַ�������ӡ������ͬ
		  �ַ��������б�����ȫ�ַ������ֱ���������ֵ� */
		for(int i = 0;i < matchnums; ++i) {
			printf("\r\n\t%s",match[i]->name); 
		}

		printf("\r\n%s%s",shellin->sign,shellin->cmdline); 
		for ( ; ; ) {
			/* �� match[] �к�����ͬ���ַ���ȫ�����뻺���� */
			for (int i = 1 ; i < matchnums ; ++i ) {
				if (match[0]->name[len] != match[i]->name[len]){
					return  ;
				}
			}
			shell_getchar(shellin,match[0]->name[len++]);
		}
	}
}


/**
  * @author   ��ô��
  * @brief    �����ǰ��ӡ�����������ݣ�����һ����λ
  * @param    shellin : ���뽻��
  * @return   don't care
*/
static void shell_backspace(struct shell_input * shellin)
{
	char   printbuf[COMMANDLINE_MAX_LEN*2]={0};//��ת�ڴ�
	char * print = &printbuf[1];
	char * printend = print + (shellin->tail - shellin->edit) + 1;
	char * edit = &shellin->cmdline[shellin->edit--] ;
	char * tail = &shellin->cmdline[shellin->tail--];

	/* ����������Ҽ�ͷʱ����Ҫ���ַ����������ƴ���������������
	   �� abUcd ��ɾ��U����Ҫ����cd������ӡ���� '\b' ʹ���ص� ab �� */
	for (char * cp = edit - 1 ; edit < tail ; *cp++ = *edit++) {
		*print++ = *edit;
		*printend++ = '\b';
	}

	printbuf[0] = '\b';
	*print = ' ';       //�������һ���ַ���ʾ
	*printend++ = '\b'; //������

	shellin->cmdline[shellin->tail] = 0;  //ĩ������ַ���������
	printl(printbuf,printend-printbuf);
}

/**
  * @author   ��ô��
  * @brief    �����м�¼����һ���ַ�
  * @param    shellin : ���뽻��
  * @param    ascii   : ���������ַ�
  * @return   don't care
*/
static void shell_getchar(struct shell_input * shellin , char ascii)
{
	if (shellin->tail + 1 >= COMMANDLINE_MAX_LEN){
		return ;
	}

	if (shellin->tail == shellin->edit) {
		shellin->cmdline[shellin->edit++] = ascii;
		shellin->cmdline[++shellin->tail] = 0;
		printl(&ascii,1);
	}
	else {
		/* ��ʵ else ��֧��ȫ���Դ��� tail == edit ����� */
		char  printbuf[COMMANDLINE_MAX_LEN*2]={0};
		char *tail = &shellin->cmdline[shellin->tail++];
		char *edit = &shellin->cmdline[shellin->edit++];
		char *print = printbuf + (tail - edit);
		char *printend = print + 1;

		/* ����������Ҽ�ͷʱ����Ҫ���ַ����������ƴ���������������
		   �� abcd ����bc����U����Ҫ����cd������ӡ���� '\b' ʹ���ص� abU �� */
		for (char *cp = tail - 1; cp >= edit ; *tail-- = *cp--) {
			*print-- = *cp;
			*printend++ = '\b';
		}

		/* �����ַ� */
		*print = ascii; 
		*edit  = ascii;
		shellin->cmdline[shellin->tail] = 0 ;
		printl(printbuf,printend - printbuf);
	}
}

/**
  * @author   ��ô��
  * @brief    �����н�������
  * @param    cmdroot : ����������ʼ������
  * @param    shellin : ���뽻��
  * @return   don't care
*/
static void shell_parse(cmd_root_t * cmdroot , struct shell_input * shellin)
{
	union uncmd unCmd ;
	uint32_t len = 0;
	uint32_t sum = 0;
	uint32_t fcrc8 = 0;
	uint32_t bcrc8 = 0;
	char  *  str = shellin->cmdline;
	struct shellcommand * cmdmatch;

	/* Shave off any leading spaces */
	for ( ; ' ' == *str ; ++str) ;	

	if (0 == *str)
		goto PARSE_END;

	unCmd.part.FirstChar = *str ;
	for (; (*str) && (*str != ' ') ; ++str ,++len) {
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
	}

	unCmd.part.Len = len;
	unCmd.part.Sum = sum;
	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;

	cmdmatch = shell_search_cmd(cmdroot,unCmd.ID);
	if (cmdmatch != NULL) {
		/* �ж��Ƿ�Ϊ��ѡ������� */
		shellcfm_t * confirm ;
		confirm = container_of(cmdmatch, struct shellconfirm, cmd);
		if (confirm->flag == CONFIRM_FLAG) {
			shell_confirm(shellin,confirm->prompt,cmdmatch->func);
		}
		else {
			cmdmatch->func(shellin->cmdline);
		}
	}
	else {
		printf("\r\n\tno reply:%s\r\n",shellin->cmdline);
	}
	
PARSE_END:
	shellin->tail = 0;//��յ�ǰ����������
	shellin->edit = 0;
	return ;
}

/**
  * @brief    ����̨����
  * @param    arg  : �������ڴ�
  * @return   don't care
*/
static void shell_clean_screen(void * arg)
{
	struct shell_input * shellin ; 
	shellin = container_of(arg, struct shell_input, cmdline);
	printf("\033[2J\033[%ld;%ldH%s",(int32_t)0,(int32_t)0,shellin->sign);
	return ;
}

/**
  * @author   ��ô��
  * @brief    ��ʾ����ע���˵�����
  * @param    arg  : �������ڴ�
  * @return   don't care
*/
static void shell_list_cmd(void * arg)
{
	struct shell_input * shellin ;
	struct shellcommand * cmd;
	uint32_t firstchar = 0;
	cmd_entry_t  * node ;
	
	for (node = FIRST(&shellcmdroot) ; node; node = NEXT(node)){
		cmd = container_of(node,struct shellcommand, node);
		if (firstchar != (cmd->ID & 0xfc000000)) {
			firstchar = cmd->ID & 0xfc000000;
			printf("\r\n(%c)------",(char)((firstchar>>26)|0x40));
		}
		printf("\r\n\t%s", cmd->name);
	}

	shellin = container_of(arg, struct shell_input, cmdline);
	printf("\r\n\r\n%s",shellin->sign);
}

/**
  * @author   ��ô��
  * @brief    ע��һ������źͶ�Ӧ������� 
  * @note     ǰ׺Ϊ '_' ��ʾ������ֱ�ӵ��ô˺���
  * @param    cmd_name : ������
  * @param    cmd_func : ��������Ӧ��ִ�к���
  * @param    newcmd   : ������ƿ��Ӧ��ָ��
  * @param    confirm  : �����Ƿ���Ҫȷ����Ϣ
  * @return   don't care
*/
void _shell_register(struct shellcommand * newcmd,char * cmd_name, cmd_fn_t cmd_func)
{
	char * str = cmd_name;
	union uncmd unCmd ;
	uint32_t clen;
	uint32_t fcrc8 = 0;
	uint32_t bcrc8 = 0;
	uint32_t sum = 0;

	for (clen = 0; *str ; ++clen,++str) {
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
	}

	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;
	unCmd.part.Len = clen;
	unCmd.part.Sum = sum;
	unCmd.part.FirstChar = *cmd_name;
	
	newcmd->ID = unCmd.ID;   //����������
	newcmd->name = cmd_name;
	newcmd->func = cmd_func;
	shell_insert_cmd(&shellcmdroot,newcmd);
}

/**
  * @author   ��ô��
  * @brief    �� "a b c d" ��ʽ����ȡΪ char*argv[] = {"a","b","c","d"};
  * @note     һ�㹩 getopt() ���������й������������ݽ�������
  * @param    str    : �����ַ���������������������ָ��
  * @param    argv   : ����ת���󻺴��ַ
  * @param    maxread: ����ȡ��
  * @return   ���ն�ȡ�����������
*/
int cmdline_strtok(char * str ,char ** argv ,int maxread)
{
	int argc = 0;

	for ( ; ' ' == *str ; ++str) ; //�����ո�
	
	for ( ; *str && argc < maxread; ++argc,++argv ) { //�ַ���Ϊ ��\0' ��ʱ��
	
		for (*argv = str ; ' ' != *str && *str ; ++str);//��¼���������Ȼ�������ǿ��ַ�
		
		for ( ; ' ' == *str; *str++ = '\0');//ÿ���������ַ����������������ո�		
	}
	
	return argc;
}


/**
  * @brief    ת����ȡ����ź��������������ַ���תΪ����
  * @param    str     �����ַ���������������������ָ��
  * @param    argv    ����ת���󻺴��ַ
  * @param    maxread ����ȡ��
  * @return   ���ݸ���
	  * @retval   >= 0         ��ȡ�������������������
	  * @retval   PARAMETER_ERROR(-2)  �������������������
	  * @retval   PARAMETER_HELP(-1)   ���������� ? ��
*/
int cmdline_param(char * str,int * argv,int maxread)
{
	int argc ;
	unsigned int  value;

	for ( ; ' ' == *str        ; ++str);//�����ո�
	for ( ; ' ' != *str && *str; ++str);//������һ������
	for ( ; ' ' == *str        ; ++str);//�����ո�

	if (*str == '?')
		return PARAMETER_HELP;//��������������ʺţ�����help

	for (argc = 0; *str && argc < maxread; ++argc , ++argv) { //�ַ���Ϊ ��\0' ��ʱ��
	
		*argv = 0;
		
		if ('0' == str[0] && 'x' == str[1]) { //"0x" ��ͷ��ʮ������ת��
			for ( str += 2 ;  ; ++str )  {
				if ( (value = *str - '0') < 10 ) // value �ȸ�ֵ�����ж� 
					*argv = (*argv << 4)|value;
				else
				if ( (value = *str - 'A') < 6 || (value = *str - 'a') < 6)
					*argv = (*argv << 4) + value + 10;
				else
					break;
			}
		}
		else { //ѭ�����ַ���תΪ���֣�ֱ���ַ���Ϊ 0 - 9
			unsigned int minus = ('-' == *str);//������ת��
			if (minus)
				++str;

			for (value = *str - '0'; value < 10 ; value = *(++str) - '0')
				*argv = (*argv * 10 + value);
			
			if (minus)
				*argv = -(*argv);
		}

		if ('\0' != *str && ' ' != *str)//������� 0 - 9 ���Ҳ��ǿո����Ǵ����ַ�
			return PARAMETER_ERROR;

		for ( ; ' ' == *str ; ++str);//�����ո�,�����ж���һ������
	}

	return argc;
}

/**
  * @author   ��ô��
  * @brief    ��ӭҳ
  * @param    shellin : ����
  * @param    recv    : Ӳ���������յ������ݻ�������ַ
  * @param    len     : Ӳ���������յ������ݳ���
  * @return   don't care
*/
void welcome_gets(struct shell_input * shellin,char * recv,int len)
{
	//��ӡһ����ӭҳlogo
	static const char consolologo[] = "\r\n\
  _____                        __\r\n\
 / ____\\                      /\\ \\\r\n\
/\\ \\___/   ____  ____  ____  _\\_\\ \\     ____\r\n\
\\ \\ \\     / __ \\/ __ \\/ ___\\/ __ \\ \\   / __ \\\r\n\
 \\ \\ \\___L\\ \\L\\ \\ \\/\\ \\____ \\ \\L\\ \\ \\_L\\  ___L\r\n\
  \\ \\______\\____/\\_\\ \\_\\____/\\____/\\____\\____/\r\n\
   \\/______/___/\\/_/\\/_/___/\\/___/\\/____/___/\r\n\
    COPYRIGHT(c):GoodMorning		2019/06\r\n\r\n" ;

	printl((char*)consolologo,sizeof(consolologo)-1);
	shellin->gets = cmdline_gets;
	cmdline_gets(shellin,recv,len);
	return ;
}

/**
  * @author   ��ô��
  * @brief    Ӳ���Ͻ��յ������ݵ������еĴ���
  * @param    shellin : ����
  * @param    recv    : Ӳ���������յ������ݻ�������ַ
  * @param    len     : Ӳ���������յ������ݳ���
  * @return   don't care
*/
void cmdline_gets(struct shell_input * shellin,char * recv,int len)
{
	int state = 0 ;

	for (char * end = recv + len ; recv < end ; ++recv) {
		if (0 == state) {
			/* ��ͨ�ַ������ڴ�;�����ж����⹦���ַ� */
			if (*recv > 0x1F && *recv < 0x7f)
				shell_getchar(shellin,*recv);
			else
			switch (*recv) {
        case KEYCODE_ENTER:
          if (shellin->tail){
            printf("\r\n");
            shell_record(shellin);
            shell_parse(&shellcmdroot ,shellin);
          }
          else{
            printf("\r\n%s",shellin->sign);
          }
          break;
        case KEYCODE_ESC :
          state = 1;
          break;
        case KEYCODE_CTRL_C:
          shellin->edit = 0;
          shellin->tail = 0;
          printf("^C\r\n%s",shellin->sign);
          break;
        case KEYCODE_BACKSPACE :
        case 0x7f: /* for putty */
          if (shellin->edit)
            shell_backspace(shellin);
          break;
        case KEYCODE_TAB:
          shell_tab(shellin);
          break;
        default: ;
			}
		}
		else 
		if (1 == state){ 
			/* �ж��Ƿ��Ǽ�ͷ���� */
			state = (*recv == '[') ? 2 : 0 ;
		}
		else{
			/* if (2 == state) ��Ӧ��ͷ���� */
			switch(*recv){  
        case 'A'://�ϼ�ͷ
          shell_show_history(shellin,0);
          break;
        case 'B'://�¼�ͷ
          shell_show_history(shellin,1);
          break;
        case 'C'://�Ҽ�ͷ
          if ( shellin->tail != shellin->edit)
            printl(&shellin->cmdline[shellin->edit++],1);
          break;
        case 'D'://���ͷ
          if (shellin->edit){
            --shellin->edit;
            printl("\b",1);
          }
          break;
        default:;
			} //switch ��ͷ����
		} // if (2 == state) //��Ӧ��ͷ����
	} //for ( ; len && *recv; --len,++recv)
	return ;
}

/**
  * @brief    ��������Ϣȷ�ϣ�������� y/Y ��ִ������
  * @param    shellin : ����
  * @param    buf     : Ӳ���������յ������ݻ�������ַ
  * @param    len     : Ӳ���������յ������ݳ���
  * @return   don't care
*/
static void confirm_gets(struct shell_input * shellin ,char * buf , int len)
{
	char * option = &shellin->cmdline[COMMANDLINE_MAX_LEN-1];

	if (0 == *option) { //������ [Y/y/N/n] ������������Ч
		if ('Y' == *buf || 'y' == *buf || 'N' == *buf || 'n' == *buf) {
			*option = *buf;
			printl(buf,1);
		}
	}
	else
	if (KEYCODE_BACKSPACE == *buf) { //���˼�
		printl("\b \b",3);
		*option = 0;
	}
	else
	if ('\r' == *buf || '\n' == *buf) {//���س�ȷ��
//		cmd_fn_t yestodo = (cmd_fn_t)shellin->apparg;
    cmd_fn_t yestodo;
    memcpy(&yestodo, &shellin->apparg, sizeof(cmd_fn_t));
 		char opt = *option ; 
		
		*option = 0 ;  //shellin->cmdline[COMMANDLINE_MAX_LEN-1] = 0;
		shellin->gets   = cmdline_gets;//���ݻع�Ϊ������ģʽ
		shellin->apparg = NULL;

		printl("\r\n",2);

		if ( 'Y' == opt || 'y' == opt) 
			yestodo(shellin->cmdline);
		else
			printf("cancel this operation\r\n");
	}
}

/**
  * @brief    ��������Ϣȷ�ϣ�������� y/Y ��ִ������
  * @param    shell  : ���뽻��
  * @param    info   : ѡ����Ϣ
  * @param    yestodo: ���� y/Y ������ִ�е�����
  * @return   don't care
*/
void shell_confirm(struct shell_input * shellin ,char * info ,cmd_fn_t yestodo)
{
	printf("%s [Y/N] ",info);
	shellin->gets = confirm_gets;// ��������ȡ�� confirm_gets
//	shellin->apparg = (void*)yestodo;
  memcpy(&shellin->apparg, &yestodo, sizeof(cmd_fn_t));
	shellin->cmdline[COMMANDLINE_MAX_LEN-1] = 0;
}

/**
  * @author   ��ô��
  * @brief    ��ʼ��һ�� shell ������Ĭ������Ϊ cmdline_gets
  * @param    shellin   : ��Ҫ��ʼ���� shell ���� 
  * @param    shellputs : shell ��Ӧ�������Ӵ��������
  * @param    ...       : �� gets �� sign �ض��壬��׷�� MODIFY_SIGN,"shell>>"
  * @return   don't care
*/
void shell_input_init(struct shell_input * shellin , fmt_puts_t shellputs,...)
{
	uint32_t arg  ;
	char * shellsign = DEFAULT_INPUTSIGN;
	shellgets_t shellgets = welcome_gets;
	
	va_list ap;
	va_start(ap, shellputs); //��������¶��� 

	arg = va_arg(ap, unsigned int) ;
	for (; MODIFY_MASK == (arg & (~0x0f)) ; arg = va_arg(ap, unsigned int) ) {
		if (MODIFY_SIGN == arg) //����ض��嵱ǰ�����������־
			shellsign = va_arg(ap, char*);
		else
		if (MODIFY_GETS == arg) //����ض��嵱ǰ��������������
    {
//      shellgets = (shellgets_t)va_arg(ap, void*);
      void* shellgets_ptr = va_arg(ap, void*);
      memcpy(&shellgets, &shellgets_ptr, sizeof(shellgets_t));
    }	
	}

	va_end(ap);

	shellin->tail = 0;
	shellin->edit = 0;
	shellin->puts = shellputs;
	shellin->gets = shellgets;
  shellin->gets = cmdline_gets;
	shellin->htywrt  = 0;
	shellin->htyread = 0;
	shellin->apparg  = NULL;
	strcpy(shellin->sign, shellsign);
}


/**
  * @author   ��ô��
  * @brief    shell ��ʼ��,ע�Ἰ�����������������ʼ����
  * @param    defaultsign : �ض���Ĭ�������־��Ϊ NULL ���޸�Ĭ�ϱ�־
  * @param    puts        : printf,printf,printl ��Ĭ���������Ӵ��������Ϊ NULL �򲻴�ӡ��Ϣ��
  * @return   don't care
*/
void shell_init(char * defaultsign ,fmt_puts_t puts)
{
	if (defaultsign)
		strncpy(DEFAULT_INPUTSIGN,defaultsign,COMMANDLINE_MAX_LEN);
	//strcpy(DEFAULT_INPUTSIGN,defaultsign);

	current_puts = puts ;
	default_puts = puts ;
	
	/* ע��һЩ�������� */
	shell_register_command("help"     ,shell_list_cmd);
	shell_register_command("clear"        ,shell_clean_screen);
}
