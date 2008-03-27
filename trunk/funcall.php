<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('funcall')) {
	dl('funcall.so');
}
class c {
function aa($p1,$p2,$p3,$p4) {
usleep(10000);
echo "aa()\n";
return "abc";
}
function a2($p1) {
echo "aa22()\n";
}
}
function bbb($a,$t,$r) {
echo trim("pp  \n");
    var_dump($a);
    var_dump($t);
    var_dump($r);
echo "ok\n";
    //return true;
}
fc_add_post('c::aa','bbb');
fc_add_post('trim','bbb');
fc_add_pre('c::aa','bbb');
fc_add_pre('trim','bbb');
//fc_list('bbb');
$c=new c;
$c->aa('aa','bb','cc','dd');
trim("xxxx a  ");
//$c->a2('aa');
//$a=trim('aa ');
//echo $a;
echo "ok\n";
exit;
$module = 'funcall';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo $br;
//fc_add_start('trim','surfchen_cb');
?>
