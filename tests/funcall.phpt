--TEST--
Check for funcall
--SKIPIF--
<?php if (!extension_loaded("funcall")) print "skip"; ?>
--FILE--
<?php 
function trim_post_cb($args,$result,$proc_time) {
    var_dump($args);
    var_dump($result);
}
fc_add_post('trim','trim_post_cb');
trim('php ');
?>
--EXPECT--
array(1) {
  [0]=>
  string(4) "php "
}
string(3) "php"
