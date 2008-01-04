/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_funcall.h"

ZEND_DECLARE_MODULE_GLOBALS(funcall)

/* True global resources - no need for thread safety here */
static int le_funcall;

ZEND_DLEXPORT void fc_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);
ZEND_DLEXPORT void (*fc_zend_execute_internal)(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);

ZEND_DLEXPORT void fc_execute(zend_op_array *op_array TSRMLS_DC);
ZEND_DLEXPORT void (*fc_zend_execute)(zend_op_array *op_array TSRMLS_DC);

#define NEW_FN_LIST(li,fname,len) li=emalloc(sizeof(fc_function_list));                   \
    li->name=emalloc(len+1);\
    strcpy(li->name,fname);\
    MAKE_STD_ZVAL(li->func);\
    convert_to_string(li->func);\
    Z_STRVAL_P(li->func)=emalloc(len);\
    strcpy(Z_STRVAL_P(li->func),fname);\
    Z_STRLEN_P(li->func)=len;\
    li->next=NULL

#define NEW_CB_LIST(li,fname,len) li=emalloc(sizeof(fc_callback_list));                   \
    li->name=emalloc(len);\
    strcpy(li->name,fname);\
    MAKE_STD_ZVAL(li->func);\
    convert_to_string(li->func);\
    Z_STRVAL_P(li->func)=emalloc(len);\
    strcpy(Z_STRVAL_P(li->func),fname);\
    Z_STRLEN_P(li->func)=len;\
    li->next=NULL

/* {{{ funcall_functions[]
 *
 * Every user visible function must have an entry in funcall_functions[].
 */
zend_function_entry funcall_functions[] = {
	PHP_FE(fc_add_start,	NULL)
	PHP_FE(fc_list,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in funcall_functions[] */
};
/* }}} */

/* {{{ funcall_module_entry
 */
zend_module_entry funcall_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"funcall",
	funcall_functions,
	PHP_MINIT(funcall),
	PHP_MSHUTDOWN(funcall),
	PHP_RINIT(funcall),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(funcall),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(funcall),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FUNCALL
ZEND_GET_MODULE(funcall)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    //STD_PHP_INI_ENTRY("funcall.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_funcall_globals, funcall_globals)
    //STD_PHP_INI_ENTRY("funcall.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_funcall_globals, funcall_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_funcall_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_funcall_init_globals(zend_funcall_globals *funcall_globals)
{
	funcall_globals->global_value = 0;
	funcall_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(funcall)
{
    fc_zend_execute=zend_execute;
    zend_execute=fc_execute;

    fc_zend_execute_internal=zend_execute_internal;
    zend_execute_internal=fc_execute_internal;

	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(funcall)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(funcall)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(funcall)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(funcall)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "funcall support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string fc_add_start(string function,string callback)
   Return true if successfully add */
PHP_FUNCTION(fc_add_start)
{
    char *function_name;
    char *callback_name;
    int function_len, callback_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &function_name, &function_len, &callback_name, &callback_len) == FAILURE) {
		return;
	}


    fc_function_list *tmp_gfl,*gfl,*new_gfl;
    fc_callback_list *cl=NULL,*new_cl;
    tmp_gfl=FCG(fc_start_list); 
    if (!tmp_gfl) {
        NEW_FN_LIST(FCG(fc_start_list),function_name,function_len);
        gfl=FCG(fc_start_list);
    } else {
        gfl=FCG(fc_start_list);
         while (1) {
            if (!strcmp(gfl->name,function_name)) {
                cl=gfl->callback_ref;
                break;
            }
            if (!gfl->next) {
                NEW_FN_LIST(new_gfl,function_name,function_len);
                gfl->next=new_gfl;
                gfl=new_gfl;
                break;
            }
            gfl=gfl->next;
        }
    }

    if (!cl) {
        NEW_CB_LIST(cl,callback_name,callback_len);
        gfl->callback_ref=cl;
    } else {
        while (1) {
            if (!strcmp(cl->name,callback_name)) {
                RETURN_FALSE
            }
            if (!cl->next) {
                NEW_CB_LIST(new_cl,callback_name,callback_len);
                cl->next=new_cl;
                break;
            }
            cl=cl->next;
        }
    }
	RETURN_TRUE;
}
PHP_FUNCTION(fc_list)
{
    fc_function_list *gfl;
    gfl=FCG(fc_start_list); 
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

char *get_current_function_name(zend_op_array *op_array TSRMLS_DC) 
{
    char *fname;
    int l;
    l=strlen(op_array->function_name);
    if (op_array->scope && strlen(op_array->scope->name)>0) {
        l+=strlen(op_array->scope->name)+2;
        fname=emalloc(l);
        strcpy(fname,op_array->scope->name);
        strcat(fname,"::");
        strcat(fname,op_array->function_name);
    } else {
        fname=emalloc(l);
        strcpy(fname,op_array->function_name);
    }
    //fname=emalloc(strlen(op_array->function_name));
    //op_array->function_name;
    return fname;
}
ZEND_API void fc_execute(zend_op_array *op_array TSRMLS_DC) 
{
    fc_function_list *fl_start;
    fc_callback_list *cl;
    char *current_function;
    fl_start=FCG(fc_start_list); 
    zval *retval;
    zval *args[1];

    MAKE_STD_ZVAL(args[0]);
    MAKE_STD_ZVAL(retval);

    current_function=get_current_function_name(op_array TSRMLS_CC);

    while (fl_start) {
        if (!strcmp(fl_start->name,current_function)) {
            cl=fl_start->callback_ref;
            while (cl) {
                //if (zend_hash_find(CG(function_table), cl->name, strlen(cl->name) + 1, (void **) &func)!=FAILURE) {
                //printf("ii%s|\n",func->common.function_name);
                //} 
                //cl->func->type=ZEND_USER_FUNCTION;
                //TSRMSLS_FETCH();
                //if(call_user_function_ex(EG(function_table), NULL, func, &retval, 0, NULL, 0,NULL TSRMLS_CC) != SUCCESS)
                //{
                //printf("callok%s|\n",func->common.function_name);
                //}
                //cl->func->type=IS_STRING;
                if(call_user_function_ex(EG(function_table), NULL, cl->func, &retval, 0, NULL, 0,NULL TSRMLS_CC) != SUCCESS)
                {
                    printf("ok%d|%s----\n",IS_STRING,Z_STRVAL_P(cl->func));
                }
                cl=cl->next;
            }
            break;
        }
        fl_start=fl_start->next;
    }
    //fc_zend_execute(op_array TSRMLS_CC);
    execute(op_array TSRMLS_CC);
}
ZEND_API void fc_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC) 
{
    execute_internal(execute_data_ptr, return_value_used TSRMLS_CC);
    //fc_zend_execute_internal(execute_data_ptr, return_value_used TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
