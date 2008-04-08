/*
  +----------------------------------------------------------------------+
  | funcall                                                              |
  +----------------------------------------------------------------------+
  | Copyright (c) 2008 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to the new BSD license, that is bundled  |
  | with this package in the file LICENSE.                               |
  +----------------------------------------------------------------------+
  | Author:Surf Chen <surfchen@gmail.com>                                |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FUNCALL_H
#define PHP_FUNCALL_H

extern zend_module_entry funcall_module_entry;
#define phpext_funcall_ptr &funcall_module_entry

#ifdef PHP_WIN32
#define PHP_FUNCALL_API __declspec(dllexport)
#else
#define PHP_FUNCALL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

typedef struct _fc_function_list {
    char *name;
    zval *func;
    struct _fc_callback_list *callback_ref;
    struct _fc_function_list *next;
} fc_function_list;
typedef struct _fc_callback_list {
    char *name;
    zval *func;
    struct _fc_callback_list *next;
} fc_callback_list;

PHP_MINIT_FUNCTION(funcall);
PHP_MSHUTDOWN_FUNCTION(funcall);
PHP_RINIT_FUNCTION(funcall);
PHP_RSHUTDOWN_FUNCTION(funcall);
PHP_MINFO_FUNCTION(funcall);

PHP_FUNCTION(fc_add_pre);
PHP_FUNCTION(fc_add_post);
PHP_FUNCTION(fc_list);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

*/
ZEND_BEGIN_MODULE_GLOBALS(funcall)
    fc_function_list *fc_pre_list;	
    fc_function_list *fc_post_list;	
    int in_callback;	
ZEND_END_MODULE_GLOBALS(funcall)

/* In every utility function you add that needs to use variables 
   in php_funcall_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as FUNCALL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define FCG(v) TSRMG(funcall_globals_id, zend_funcall_globals *, v)
#else
#define FCG(v) (funcall_globals.v)
#endif

#endif	/* PHP_FUNCALL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
