<?php
if(!extension_loaded('funcall')) {
	dl('funcall.so');
}
function my_func($arg1,$arg2) {
    usleep(20000);
    echo "step 002\n";
    return $arg1.$arg2;
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
fc_add_post('trim','post_cb');
var_dump(fc_list());
my_func('php','c');
echo trim("abc\n");
?>
