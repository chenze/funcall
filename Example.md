## Notices ##
  * eval() is not fully supported - argument cannot not be catched. The argument returned to callback is always null.
  * if you want catch the return value, please ensure that you have variable assignment for the function being monitored. for example, $a=func(); is ok if you want catch the return value in post callbacks and without "$a=" you will get null.
  * include**/require** return the resolved path to callbacks instead of the argument. for example, for include '../include.php', you will see /path/to/include.php instead of ../include.php

### Example 1 ###
```
<?php
if(!extension_loaded('funcall')) {
        dl('funcall.so');
}
function my_func($arg1,$arg2) {
    usleep(20000);
    echo "step 002\n";
    return $arg1.$arg2;
}
class my_class {
    function f1() {
        return true;
    }
}
function pre_cb($args) {
    var_dump($args);
    echo "step 001\n";
}
function post_cb($args,$result,$process_time) {
    var_dump($args);
    var_dump($result);
    echo 'step 003 (cost:',$process_time,")\n";
}

fc_add_pre('my_func','pre_cb');
fc_add_post('my_func','post_cb');
$ret=my_func('php','c');

fc_add_post('trim','post_cb');
$ret=trim("abc\n");
echo $ret;

fc_add_pre('my_class::f1','pre_cb');
fc_add_post('my_class::f1','post_cb');
$my_class=new my_class;
$ret=$my_class->f1();

var_dump(fc_list());
?>

```

### Example 2 ###
```
$mq_time=$mq_count=0;
function mysql_query_monitor($args,$result,$process_time) {
    global $mq_time,$mq_count;
    $mq_time+=$process_time;
    $mq_count++;
}
function mysql_query_log() {
    global $mq_time,$mq_count;
    file_put_contents('/tmp/mq.log',$mq_count."\n".$mq_time);
}
fc_add_post('mysql_query','mysql_query_monitor');
register_shutdown_function('mysql_query_log');
```