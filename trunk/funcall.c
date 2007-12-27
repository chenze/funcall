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

/* If you declare any globals in php_funcall.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(funcall)
*/

/* True global resources - no need for thread safety here */
static int le_funcall;

ZEND_API *fc_zend_execute;
ZEND_API *fc_zend_execute_internal;

/* {{{ funcall_functions[]
 *
 * Every user visible function must have an entry in funcall_functions[].
 */
zend_function_entry funcall_functions[] = {
	PHP_FE(fc_add,	NULL)
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
    fc_zend_execute=zend_execute;
    fc_zend_execute_internal=zend_execute_internal;
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
/* {{{ proto string fc_add(string function,string callback)
   Return true if successfully add */
PHP_FUNCTION(fc_add)
{
    char *function_name;
    char *callback_name;
    int function_len, callback_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &function_name, &function_len, &callback_name, &callback_len) == FAILURE) {
		return;
	}

	RETURN_TRUE;
}
PHP_FUNCTION(fc_list)
{
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

ZEND_API fc_execute(zend_op_array *op_array TSRMLS_CC) 
{
}
ZEND_API fc_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_CC) 
{
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
