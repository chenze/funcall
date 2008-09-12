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

/*ZhongGuoRen jia QQ qun:8335498*/

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
    ZVAL_STRING(li->func,fname,1)\
    li->next=NULL

#define NEW_CB_LIST(li,fname,len) li=emalloc(sizeof(fc_callback_list));                   \
    li->name=emalloc(len+1);\
    strcpy(li->name,fname);\
    MAKE_STD_ZVAL(li->func);\
    ZVAL_STRING(li->func,fname,1)\
    li->next=NULL

/* {{{ funcall_functions[]
 *
 * Every user visible function must have an entry in funcall_functions[].
 */
zend_function_entry funcall_functions[] = {
	PHP_FE(fc_add_pre,	NULL)
	PHP_FE(fc_add_post,	NULL)
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


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(funcall)
{
#ifdef ZTS
    ZEND_INIT_MODULE_GLOBALS(funcall,NULL,NULL);
#endif
    fc_zend_execute=zend_execute;
    zend_execute=fc_execute;
    FCG(in_callback)=0;

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
    FCG(in_callback)=0;
    FCG(fc_pre_list)=NULL;
    FCG(fc_post_list)=NULL;
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(funcall)
{

//fprintf(stderr,"shutdown\n");
    fc_function_list *f_list,*tmp_list;
    f_list=FCG(fc_pre_list); 
    while (f_list) {
        tmp_list=f_list->next;
        efree(f_list);
        f_list=tmp_list;
    }
    f_list=FCG(fc_post_list); 
    while (f_list) {
        tmp_list=f_list->next;
        efree(f_list);
        f_list=tmp_list;
    }
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
}
/* }}} */

/* {{{ proto boolean fc_add_pre(string function,string callback)
    Add a pre-callback. Return true if successfully */
PHP_FUNCTION(fc_add_pre)
{
    char *function_name;
    char *callback_name;
    int function_len, callback_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &function_name, &function_len, &callback_name, &callback_len) == FAILURE) {
		return;
	}
    fc_add_callback(function_name,function_len,callback_name,callback_len,0 TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean fc_add_post(string function,string callback)
   Add a post-callback.Return true if successfully  */
PHP_FUNCTION(fc_add_post)
{
    char *function_name;
    char *callback_name;
    int function_len, callback_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &function_name, &function_len, &callback_name, &callback_len) == FAILURE) {
		return;
	}
    fc_add_callback(function_name,function_len,callback_name,callback_len,1 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array fc_list()
   Return all callbacks like: 
    array(
        0=>array(
            'mysql_query'=>array(
                'pre_callback1','pre_callback2'
            )
        ),
        1=>array(
            'mysql_query'=>array(
                'post_callback1','post_callback2'
            )
        )
    );
*/
PHP_FUNCTION(fc_list)
{
    fc_function_list *pre_list,*post_list;
    pre_list=FCG(fc_pre_list); 
    post_list=FCG(fc_post_list); 

    zval *pre_arr,*post_arr;
    MAKE_STD_ZVAL(pre_arr);
    MAKE_STD_ZVAL(post_arr);
    array_init(pre_arr);
    array_init(post_arr);

    fc_callback_list *cb;
    zval *callback_list;

    while (pre_list) {
        cb=pre_list->callback_ref;
        MAKE_STD_ZVAL(callback_list);
        array_init(callback_list);
        while (cb) {
            add_next_index_string(callback_list,cb->name,1);
            cb=cb->next;
        }
        add_assoc_zval(pre_arr,pre_list->name,callback_list);
        pre_list=pre_list->next;
    }

    while (post_list) {
        //zend_printf("|%s__\n",post_list->name);
        cb=post_list->callback_ref;
        MAKE_STD_ZVAL(callback_list);
        array_init(callback_list);
        while (cb) {
            add_next_index_string(callback_list,cb->name,1);
            cb=cb->next;
        }
        add_assoc_zval(post_arr,post_list->name,callback_list);
        post_list=post_list->next;
    }

    array_init(return_value);
    add_next_index_zval(return_value,pre_arr);
    add_next_index_zval(return_value,post_arr);
}
/* }}} */


static double microtime(TSRMLS_D) {
    zval *t;
    zval *func,*return_as_double;
    zval **args[1];

    MAKE_STD_ZVAL(func);
    ZVAL_STRING(func, "microtime", 1);

    MAKE_STD_ZVAL(return_as_double);
    ZVAL_BOOL(return_as_double, 1);
    args[0]=&return_as_double;

    if(call_user_function_ex(EG(function_table), NULL, func, &t, 1, args, 0,NULL TSRMLS_CC) != SUCCESS) {
        return (double)0;
    }
    return Z_DVAL_P(t);
}


static char *get_current_function_name(TSRMLS_D) 
{
    char *current_function,*empty_function="";
    char *space;
    char *class_name;
    class_name=get_active_class_name(&space TSRMLS_CC); 

    if (strlen(space)==2) {
        char *fname = get_active_function_name(TSRMLS_C);
        current_function=emalloc(strlen(class_name)+3+strlen(fname));
        memset(current_function,0,strlen(class_name)+3+strlen(fname));
        strcpy(current_function,class_name);
        strcat(current_function,"::");
        strcat(current_function,fname);
    } else {
        current_function = get_active_function_name(TSRMLS_C);
    }
    if (!current_function) {
        return "NONE-FUNCTION";
    }
    return current_function;
}

static int get_current_function_args(zval **args[] TSRMLS_DC) {
    args[0] = (zval**)emalloc(sizeof(zval**));
    MAKE_STD_ZVAL(*args[0]);
    array_init(*args[0]);

    /*These get-args-code is borrowed from ZEND_FUNCTION(func_get_args)*/
    void **p = EG(argument_stack).top_element-2;
    int arg_count = (int)(zend_uintptr_t) *p;
    int i;
    for (i=0; i<arg_count; i++) {
        zval *element;

        ALLOC_ZVAL(element);
        *element = **((zval **) (p-(arg_count-i)));
        zval_copy_ctor(element);
        INIT_PZVAL(element);
        zend_hash_next_index_insert((*args[0])->value.ht, &element, sizeof(zval *), NULL);
    }
    return 1;
}

static int callback_existed(char *func_name TSRMLS_DC) {
    fc_function_list *pre_list,*post_list;
    pre_list=FCG(fc_pre_list); 
    post_list=FCG(fc_post_list); 

    while (pre_list) {
        if (!strcmp(pre_list->name,func_name)) {
            return 1;
        }
        pre_list=pre_list->next;
    }
    while (post_list) {
        if (!strcmp(post_list->name,func_name)) {
            return 1;
        }
        post_list=post_list->next;
    }
    return 0;
}
int fc_add_callback(
    char *function_name,
    int function_len,
    char *callback_name,
    int callback_len,
    int type TSRMLS_DC)
{
    fc_function_list *tmp_gfl,*gfl,*new_gfl;
    fc_callback_list *cl=NULL,*new_cl;
    if (type==0) {
        tmp_gfl=FCG(fc_pre_list); 
    } else {
        tmp_gfl=FCG(fc_post_list); 
    }
    if (!tmp_gfl) {
        if (type==0) {
            NEW_FN_LIST(FCG(fc_pre_list),function_name,function_len);
            gfl=FCG(fc_pre_list);
        } else {
            NEW_FN_LIST(FCG(fc_post_list),function_name,function_len);
            gfl=FCG(fc_post_list);
        }
    } else {
        if (type==0) {
            gfl=FCG(fc_pre_list);
        } else {
            gfl=FCG(fc_post_list);
        }
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
                return 0;
            }
            if (!cl->next) {
                NEW_CB_LIST(new_cl,callback_name,callback_len);
                cl->next=new_cl;
                break;
            }
            cl=cl->next;
        }
    }
	return 1;
}

static void fc_do_callback(char *current_function,zval *** args,int type TSRMLS_DC) {
    fc_function_list *fc_list;
    fc_callback_list *cl;
    int arg_count;
    if (type==0) {
        arg_count=1;
        fc_list=FCG(fc_pre_list); 
    } else {
        arg_count=3;
        fc_list=FCG(fc_post_list); 
    }
    zval *retval;
    MAKE_STD_ZVAL(retval);

    while (fc_list) {
        if (!strcmp(fc_list->name,current_function)) {
            cl=fc_list->callback_ref;
            while (cl) {
                FCG(in_callback)=1;
                if(call_user_function_ex(EG(function_table), NULL, cl->func, &retval, arg_count, args, 0,NULL TSRMLS_CC) != SUCCESS)
                {
                    //
                }
                FCG(in_callback)=0;
                cl=cl->next;
            }
            break;
        }
        fc_list=fc_list->next;
    }
 
}

ZEND_API void fc_execute(zend_op_array *op_array TSRMLS_DC) 
{
    char *current_function;
    current_function=get_current_function_name(TSRMLS_C);
    if (FCG(in_callback)==1 || callback_existed(current_function TSRMLS_CC)==0) {
        execute(op_array TSRMLS_CC);
    } else {
        zval ***args=NULL;
        args = (zval ***)safe_emalloc(3,sizeof(zval **), 0);

        get_current_function_args(args TSRMLS_CC);
        fc_do_callback(current_function,args,0 TSRMLS_CC);
        double start_time=microtime(TSRMLS_C);
        execute(op_array TSRMLS_CC);
        double process_time=microtime(TSRMLS_C)-start_time;

        zval *t;
        MAKE_STD_ZVAL(t);
        ZVAL_DOUBLE(t,process_time);
        args[2] = &t;

        args[1] = EG(return_value_ptr_ptr);

        fc_do_callback(current_function,args,1 TSRMLS_CC);
    }
    if (strchr(current_function,':')!=NULL) {
        efree(current_function);
    }
}

ZEND_API void fc_execute_internal(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC) 
{
    char *current_function;
    current_function=get_current_function_name(TSRMLS_C);
    if (FCG(in_callback)==1 || callback_existed(current_function TSRMLS_CC)==0) {
        execute_internal(execute_data_ptr, return_value_used TSRMLS_CC);
    } else {
        //zend_printf("|%s__\n",current_function);
        zval ***args=NULL;
        args = (zval ***)safe_emalloc(3,sizeof(zval **), 0);
        zval *t;
        zend_execute_data *ptr;
        zval **return_value_ptr;

        get_current_function_args(args TSRMLS_CC);
        fc_do_callback(current_function,args,0 TSRMLS_CC);

        double start_time=microtime(TSRMLS_C);
        execute_internal(execute_data_ptr, return_value_used TSRMLS_CC);
        double process_time=microtime(TSRMLS_C)-start_time;

        //zend_printf("|%ld++\n",process_time);
        MAKE_STD_ZVAL(t);
        ZVAL_DOUBLE(t,process_time);
        args[2] = &t;

        ptr = EG(current_execute_data);
        return_value_ptr = &(*(temp_variable *)((char *) ptr->Ts + ptr->opline->result.u.var)).var.ptr;
        args[1] = return_value_ptr;

        fc_do_callback(current_function,args,1 TSRMLS_CC);
    }
    if (strchr(current_function,':')!=NULL) {
        efree(current_function);
    }
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
