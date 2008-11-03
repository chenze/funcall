<?php 
/*
(user|system)class
    static method(none-args or have-args)
    none-static method(none-args or have-args)

(user|system)function(none-args or have-args)

include* require*
*/
//dl('funcall.so');
function m1() {
    return 0;
}
function m2($a,$b,$c) {
    echo "iii2\n";
    return 2;
}
class testc2 {
    public function testf() {
        echo 'testf';
    }
}
function pre_cb($args) {
    if (count($args)==0) {
        echo 'zero';
    } else if (count($args)==1) {
        echo 'trim';
    } else {
        $args[2]->testf();
    }
}
function post_cb($args,$result,$t) {
    if (count($args)==0) {
        echo 'zero';
    } else if (count($args)==1) {
        echo 'trim';
    } else {
        $args[2]->testf();
    }
    echo $result;
}
fc_add_pre('m1','pre_cb');
fc_add_pre('m2','pre_cb');
fc_add_post('m1','post_cb');
fc_add_post('m2','post_cb');

fc_add_post('trim','pre_cb');
fc_add_post('trim','post_cb');

$t2=new testc2;

//m1();
m2('abc',true,$t2);
//trim(' ok ');
?>
