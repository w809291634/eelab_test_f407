/**
  ******************************************************************************
  * @file           shell.h
  * @author         ��ô��
  * @brief          ���������ͷ�ļ�
  * ʹ�ò��裺
  * <pre>
  * ʹ�ò��裺
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
#ifndef __SHELL_H__
#define __SHELL_H__

// ����Ϊ shell �������Ļ�����
#include "string.h"
#include "extend.h"
#include "stm32f4xx.h"

/* Public variables ---------------------------------------------------------*/
extern char DEFAULT_INPUTSIGN[]; // Ĭ�Ͻ�����־
typedef void (*fmt_puts_t)(const char * strbuf,unsigned short len);//
#define printl(ptr,len)  do{if (current_puts) current_puts(ptr,len);}while(0)
extern  fmt_puts_t current_puts;
extern  fmt_puts_t default_puts;

/* Public macro (���к�)------------------------------------------------------------*/

/* ---- option (������) ---- */

/**
 * @brief Ϊ 1 ʱʹ��ƽ����������в���ƥ�䣬����ʹ�õ�����
 * @note  �ö�����������ѯϵͳҪ������죬����Ҫ���� avltree.c �ļ�֧�֣���
 *        ������� 800~1000bytes �� ROM ռ�á�������ע�������϶�ʱ���á� 
*/
#define USE_AVL_TREE            0

/// ������ϲ������ַ����������¼����
#define COMMANDLINE_MAX_LEN     50

/// ����̨��¼��Ŀ������Ϊ 0 ʱ����¼
#define COMMANDLINE_MAX_RECORD  4

/* ---- option end ---- */

// һЩ��ֵ��
#define KEYCODE_END               35
#define KEYCODE_HOME              36
#define KEYCODE_CTRL_C            0x03
#define KEYCODE_BACKSPACE         0x08   //���̵Ļ��˼�
#define KEYCODE_TAB               '\t'   //���̵�tab��
#define KEYCODE_NEWLINE           0x0A
#define KEYCODE_ENTER             0x0D   //���̵Ļس���
#define KEYCODE_ESC               0x1b


#define MODIFY_MASK  0xABCD4320
#define FUNC_NAME(_1, _2, _3, NAME,...) NAME
#define FUNC(...)                       FUNC_NAME(__VA_ARGS__,PRINTF3,PRINTF2)(__VA_ARGS__)


/**
  * @author   ��ô��
  * @brief    ������̨ע������
  * @note     ���ú�ע�������ͬʱ���½�һ���������Ӧ�Ŀ��ƿ�
  * @param    name  : ���ƣ�����Ϊ�����ַ���ָ��
  * @param    func  : ����ִ�к�����@see cmd_fn_t
*/
#define shell_register_command(name,func)\
do{\
	static struct shellcommand newcmd = {0};\
	_shell_register(&newcmd,name,func);     \
}while(0)


/**
  * @author   ��ô��
  * @brief    ������̨ע��һ����ѡ�������Ҫ���� [Y/N/y/n] ��ִ�ж�Ӧ������
  * @note     ���ú�ע�������ͬʱ���½�һ���������Ӧ�Ŀ��ƿ�
  * @param    name  : ���ƣ�����Ϊ�����ַ���ָ��
  * @param    func  : ����ִ�к�����@see cmd_fn_t
*/
#define shell_register_confirm(name,func,info)\
do{\
	static struct shellconfirm confirm = {  \
	.prompt = info ,.flag = CONFIRM_FLAG }; \
	_shell_register(&confirm.cmd,name,func);\
}while(0)


/// ����Ϊ shell_input_init() ���ú�
#define MODIFY_SIGN (MODIFY_MASK|0x1)
#define MODIFY_GETS (MODIFY_MASK|0x2)

/// ��ʷ�������⣬���ݾɰ汾����
#define SHELL_INPUT_INIT(...) shell_input_init(__VA_ARGS__)


/// shell ��ڶ�Ӧ���ڣ�������������Ӷ�Ӧ�ĵط����
#define shell_input(shellin,buf,len) \
do{\
	if ((shellin)->gets) {\
		current_puts = (shellin)->puts;        \
		(shellin)->gets((shellin),(buf),(len));\
		current_puts = default_puts;           \
	}\
}while(0)


/* Public types ------------------------------------------------------------*/

enum INPUT_PARAMETER {
	PARAMETER_ERROR = -2,
	PARAMETER_HELP = -1,
};


/// �����Ӧ�ĺ������ͣ�����Ϊʲô�������Ϊ void *,�Ҳ��ǵ���
typedef void (*cmd_fn_t)(void * arg);


#if USE_AVL_TREE     // ����������avl�����в���ƥ��ʱ��Ҫ avltree.c ֧��
	#include "avltree.h"
	typedef struct avl_node cmd_entry_t ;
	typedef struct avl_root cmd_root_t ;
#else                   // ������ڵ㣬����������
	struct slist{struct slist * next;} ;
	typedef struct slist cmd_entry_t ;
	typedef struct slist cmd_root_t ;
#endif


/// ����ṹ�壬����ע��ƥ������
typedef struct shellcommand {
	cmd_entry_t   node    ; ///< ������������㣬������������������������
	char *        name    ; ///< ��¼ÿ�������ַ������ڴ��ַ
	cmd_fn_t      func    ; ///< ��¼����� cmd_fn_t ��Ӧ���ڴ��ַ
	unsigned long  ID      ; ///< �� name �ַ�������ѹ���õ��� ID �ţ�ƥ�����ֱ��ַ���Ч�ʸߡ�
}
shellcmd_t;


/// ��ȷ��ѡ�������ṹ��
typedef struct shellconfirm {
	struct shellcommand  cmd; /// ��Ӧ��������ڴ�
	char * prompt ;
	#define CONFIRM_FLAG 0x87654321U
	unsigned long flag   ;           /// ȷ����ʾ��Ϣ
}
shellcfm_t ;

/// �����ṹ�壬���ݵ����������һ��
typedef struct shell_input
{
	/// ָ������������,��ʼ��Ĭ��Ϊ cmdline_gets() ,��������
	void (*gets)(struct shell_input * , char * ,int );

	/// ָ����������Ӧ������ӿڣ����ڻ��� telnet �����
	fmt_puts_t puts;

	/// app���ò���������ô�þ���ô��
	void *  apparg;

	/// �������������
	char    sign[COMMANDLINE_MAX_LEN];

	// ��������صĲ���
	char          cmdline[COMMANDLINE_MAX_LEN]; ///< �������ڴ�
	unsigned char edit                        ; ///< ��ǰ�����б༭λ��
	unsigned char tail                        ; ///< ��ǰ�����������β tail

	#if (COMMANDLINE_MAX_RECORD) //�����������ʷ��¼
		unsigned char htywrt  ;  ///< ��ʷ��¼д
		unsigned char htyread ; ///< ��ʷ��¼��
		char    history[COMMANDLINE_MAX_RECORD][COMMANDLINE_MAX_LEN]; ///< ��ʷ��¼�ڴ�
	#endif
}
shellinput_t;


typedef	void (*shellgets_t)(struct shell_input * , char * ,int );

/* Public variables ---------------------------------------------------------*/


/* Public function prototypes ������ýӿ� -----------------------------------*/

//ע������������һ�㲻ֱ�ӵ��ã��ú� shell_register_command() ��ӵ���
void _shell_register(struct shellcommand * newcmd,char * cmd_name, cmd_fn_t cmd_func);


/**
  * @author   ��ô��
  * @brief    Ӳ���Ͻ��յ������ݵ������еĴ���
  * @param    shellin : ����
  * @param    recv    : Ӳ���������յ������ݻ�������ַ
  * @param    len     : Ӳ���������յ������ݳ���
  * @return   void
*/
void cmdline_gets(struct shell_input * ,char * ,int );

//���������в�����ع��ܺ���
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
int  cmdline_param(char * str,int * argv,int maxread);

/**
  * @author   ��ô��
  * @brief    �� "a b c d" ��ʽ����ȡΪ char*argv[] = {"a","b","c","d"};�Թ�getopt()����
  * @param    str    : �����ַ���������������������ָ��
  * @param    argv   : ����ת���󻺴��ַ
  * @param    maxread: ����ȡ��
  * @return   ���ն�ȡ�����������
*/
int  cmdline_strtok(char * str ,char ** argv ,int maxread);

// ��ʼ����غ���

/**
  * @author   ��ô��
  * @brief    shell ��ʼ��,ע�Ἰ�����������������ʼ����
  * @param    defaultsign : �ض���Ĭ�������־��Ϊ NULL ���޸�Ĭ�ϱ�־
  * @param    puts        : printf,printk,printl ��Ĭ���������Ӵ��������Ϊ NULL �򲻴�ӡ��Ϣ��
  * @return   don't care
*/
void shell_init(char * defaultsign ,fmt_puts_t puts);

/**
  * @author   ��ô��
  * @brief    ��ʼ��һ�� shell ������Ĭ������Ϊ cmdline_gets
  * @param    shellin   : ��Ҫ��ʼ���� shell ���� 
  * @param    shellputs : shell ��Ӧ�������Ӵ��������
  * @param    ...       : �� gets �� sign �ض��壬��׷�� MODIFY_SIGN,"shell>>"
  * @return   don't care
*/
void shell_input_init(struct shell_input * shellin , fmt_puts_t shellputs,...);


/**
  * @brief    ��������Ϣȷ�ϣ�������� y/Y ��ִ������
  * @param    shell  : ���뽻��
  * @param    info   : ѡ����Ϣ
  * @param    yestodo: ���� y/Y ������ִ�е�����
  * @return   void
*/
void shell_confirm(struct shell_input * shellin ,char * info ,cmd_fn_t yestodo) ;
void welcome_gets(struct shell_input * shellin,char * recv,int len);
#endif
