<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('funcall')) {
	dl('funcall.so');
}
class c {
function aa() {
echo "aa()\n";
}
}
function bbb() {
    echo "---------cb-------------\n";
    return true;
}
fc_add_start('c::aa','bbb');
//fc_list('bbb');
$c=new c;
$c->aa();
//$a=trim('aa ');
//echo $a;
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
